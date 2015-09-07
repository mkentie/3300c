#include "stdafx.h"
#include "3300C.h"
#include "Main.h"
#include "niash_win.h"
#include "niash_ext.h"
#include "hpgt33tk.h"


std::array<GUID,2> C3300C::sm_ButtonGUIDs = {WIA_EVENT_SCAN_IMAGE,WIA_EVENT_SCAN_PRINT_IMAGE};
std::array<wchar_t*,2> C3300C::sm_pszButtonNames = {L"Scan", L"Print"};

C3300C::C3300C()
{
    DBGMSG();
}


void C3300C::FillSCANINFO(SCANINFO& ScanInfo) const 
{
    //MSDN suggests all members should be initialized and the MicroDriver sample does so too
    ScanInfo.ADF = 0;
    ScanInfo.TPA = 0;
    ScanInfo.Endorser = 0;
    ScanInfo.OpticalXResolution = HW_DPI;
    ScanInfo.OpticalYResolution = HW_DPI;
    ScanInfo.BedWidth = 5100*1000/600; //1000th inch, based on size of Windows XP scan
    ScanInfo.BedHeight = 7020*1000/600; //1000th inch, based on size of Windows XP scan
    ScanInfo.IntensityRange.lMin = 0;//These don't actually grey out the user interface or change the slider ranges...
    ScanInfo.IntensityRange.lMax = 0;
    ScanInfo.IntensityRange.lStep = 0;
    ScanInfo.ContrastRange.lMin = 0;
    ScanInfo.ContrastRange.lMax = 0;
    ScanInfo.ContrastRange.lStep = 0;
    ScanInfo.SupportedCompressionType = 0;
    ScanInfo.SupportedDataTypes = SUPPORT_COLOR;

    ScanInfo.Intensity = 0;
    ScanInfo.Contrast = 0;
    ScanInfo.Xresolution = 300;
    ScanInfo.Yresolution = 300;
    ScanInfo.Window.xPos = 0;
    ScanInfo.Window.yPos = 0;
    ScanInfo.Window.xExtent = (ScanInfo.Xresolution * ScanInfo.BedWidth) / 1000;
    ScanInfo.Window.yExtent = (ScanInfo.Yresolution * ScanInfo.BedHeight) / 1000;

    ScanInfo.WidthPixels = ScanInfo.Window.xExtent - ScanInfo.Window.xPos;
    ScanInfo.Lines = ScanInfo.Window.yExtent;
    ScanInfo.DataType = WIA_DATA_COLOR;
    ScanInfo.PixelBits = 24;
    ScanInfo.WidthBytes = ScanInfo.Window.xExtent * ScanInfo.PixelBits / 8;

    ScanInfo.DitherPattern = 0;
    ScanInfo.Negative = 0;
    ScanInfo.Mirror = 0;
    ScanInfo.AutoBack = 0;
    ScanInfo.ColorDitherPattern = 0;
    ScanInfo.ToneMap = 0;
    ScanInfo.Compression = 0;
    ScanInfo.RawDataFormat = WIA_PACKED_PIXEL; //Can't get PLANAR to do anything, not for r/b/g lines or for the whole image r/g/b underneath each other
    ScanInfo.RawPixelOrder = WIA_ORDER_BGR;
    ScanInfo.bNeedDataAlignment = TRUE; //0 is for scanners that don't scan all lines before moving down or something?
    ScanInfo.DelayBetweenRead = 0;
    ScanInfo.MaxBufferSize = 0;
}

void C3300C::GetCapabilities(VAL& Value) const 
{
    Value.lVal = static_cast<LONG>(sm_ButtonGUIDs.size()); //Number of buttons
    Value.pGuid = sm_ButtonGUIDs.data(); //Button GUIDs
    Value.ppButtonNames = sm_pszButtonNames.data(); //Button descriptions
}

void C3300C::GetStatus(VAL& Value) 
{
    Value.lVal = m_bStatusOk ? MCRO_STATUS_OK : MCRO_ERROR_GENERAL_ERROR;
    //const unsigned char ButtonState = GetButtonState(&m_HWParams);
    //Value.pGuid = (ButtonState & EScannerButtonBits::ButtonScan) ? &sm_ButtonGUIDs[0] : (ButtonState & EScannerButtonBits::ButtonPrint) ? &sm_ButtonGUIDs[1] : nullptr;
}

void C3300C::Reset(VAL& /*Value*/)
{
    m_bStatusOk = true;
}

void C3300C::Initialize(VAL& Value)
{
    DBGMSG();

    assert(Value.pScanInfo);	
    const SCANINFO& ScanInfo = *Value.pScanInfo;
    NiashLibUsbSetDeviceHandle(ScanInfo.DeviceIOHandles[0]);
    if (NiashOpen(&m_HWParams, Value.szVal) < 0)
    {
        m_bStatusOk = false;
        return;
    }

    //Already turn on lamp for quicker calibration once user starts to scan; auto turns off after 15 minutes
    SetLamp(&m_HWParams, 1);
}

void C3300C::Uninitialize()
{
    SetLamp(&m_HWParams, 0);
    NiashClose(&m_HWParams);
}

void C3300C::ScanStart(const SCANINFO& ScanInfo)
{
    DBGMSG();

    CalibTable_t CalibTable;
//     if (!WarmupLamp(CalibTable))
//     {
//         m_bStatusOk = false;
//         DBGMSG(L"WarmupLamp() failure.");
//         return;
//     }

    WriteGammaCalibTable(hpgt33tk::GammaTableR.data(), hpgt33tk::GammaTableG.data(), hpgt33tk::GammaTableB.data(), CalibTable.data(), 0, 0, &m_HWParams);

    TScanParams ScanParams;
    ScanParams.iDpi = ScanInfo.Xresolution;
    ScanParams.iLpi = ScanInfo.Yresolution;
    ScanParams.iTop = 480 + HW_DPI * ScanInfo.Window.yPos / ScanInfo.Yresolution;
    ScanParams.iLeft = 80 + HW_LPI * ScanInfo.Window.xPos / ScanInfo.Xresolution;
    ScanParams.iWidth = ScanInfo.Window.xExtent;
    ScanParams.iHeight = ScanInfo.Window.yExtent;
    ScanParams.iBottom = HP3300C_BOTTOM;
    ScanParams.fCalib = SANE_FALSE;

    m_iLinesSkipped = 0;

    if (InitScan(&ScanParams, &m_HWParams) != SANE_TRUE)
    {
        DBGMSG(L"InitScan() failure.");
        m_bStatusOk = false;
        return;
    }
}


void C3300C::ScanContinue(const SCANINFO& ScanInfo, BYTE* const pBuffer, const long lLength, long* const pReceived)
{
    const size_t iPixelsPerLine = ScanInfo.Window.xExtent;
    const size_t iBytesPerComponent = iPixelsPerLine;
    const size_t iBytesPerLine = 3 * iPixelsPerLine;

    m_BackBuffer.resize(lLength);

    const size_t iBytesReceived = NiashReadBulk(1, m_BackBuffer.data(), lLength);
    *pReceived = static_cast<DWORD>(iBytesReceived);

    //Convert Blue line / Red line / Green line to 24bit BGR, and flip along horizontal axis as well
    const size_t iLinesReceived = iBytesReceived / iBytesPerLine;
    for (size_t i = 0; i < iLinesReceived; i++)
    {
        if (m_iLinesSkipped == m_HWParams.iSkipLines)
        {
            const BYTE* const pBlue = &m_BackBuffer[i*iBytesPerLine];
            const BYTE* const pGreen = &m_BackBuffer[i*iBytesPerLine + iBytesPerComponent];
            const BYTE* const pRed = &m_BackBuffer[i*iBytesPerLine + 2 * iBytesPerComponent];
            BGR* const pTarget = reinterpret_cast<BGR*>(&pBuffer[i*iBytesPerLine]);
            for (size_t j = 0; j < iPixelsPerLine; j++)
            {
                BGR& Target = pTarget[ScanInfo.Window.xExtent - j - 1];
                Target.components.b = pBlue[j];
                Target.components.g = pGreen[j];
                Target.components.r = pRed[j];
            }
        }
        else
        {
            m_iLinesSkipped++;
        }
    }

    //DBGMSG(L"req: %d read: %d", lLength, *pReceived);
}

void C3300C::ScanFinish(const SCANINFO& /*ScanInfo*/)
{
    m_BackBuffer.shrink_to_fit();
    FinishScan(&m_HWParams);
    DBGMSG(L"Finished.");
}

bool C3300C::WarmupLamp(CalibTable_t& CalibTable)
{
    static const size_t MAX_ATTEMPTS = 9;
    static const size_t WAIT_TIME = 10;
    static const unsigned int MAX_DEVIATION = 15; //15% comes from Linux driver

    BGR WhiteCalib = {}; //Extra buffer so we can compare

    DBGMSG(L"Starting lamp warm-up.");

    SetLamp(&m_HWParams, 1);

    const bool bUsePrevValues = m_PrevWhiteCalib.components.b != 0; //Maybe we can reuse the previous calibration values
    if (!bUsePrevValues)
    {
        if (SimpleCalibExt(&m_HWParams, &CalibTable[0], &m_PrevWhiteCalib.components.b) != SANE_TRUE) //Scan initial values
        {
            return false;
        }
    }

    for (size_t i = 0; i < MAX_ATTEMPTS; i++)
    {
        if (i > 0 || !bUsePrevValues)
        {
            Sleep(WAIT_TIME * 1000); //Wait for lamp to warm up
        }

        //Depending on iteration, use the temporary buffer or reuse the member one
        const BGR& CalibPrev = (i % 2 == 0) ? m_PrevWhiteCalib : WhiteCalib;
        BGR& CalibCurr = (i % 2 == 1) ? m_PrevWhiteCalib : WhiteCalib;

        if (SimpleCalibExt(&m_HWParams, CalibTable.data(), &CalibCurr.components.b) != SANE_TRUE)
        {
            return false;
        }

        //Check if white values have changed (lamp warm-up = get brighter)
        unsigned int iMaxDiff = std::numeric_limits<unsigned int>::min();
        for (size_t j = 0; j < CalibCurr.size(); j++)
        {
            iMaxDiff = std::max<unsigned int>(iMaxDiff, CalibPrev.bgr[j] == 0 ? 100 : std::abs(CalibCurr.bgr[j] - CalibPrev.bgr[j]) * 100 / CalibPrev.bgr[j]); //Calculate deviation in percent
        }

        //If good, break
        if (iMaxDiff<MAX_DEVIATION)
        {
            m_PrevWhiteCalib = CalibCurr;
            DBGMSG(L"Lamp warmed up in %Iu attempts.", i + 1);
            break;
        }
    }

    return true;
}

void C3300C::BuildUnityGammaTable(GammaTable_t& GammaTable)
{
    for (size_t i = 0; i<GammaTable.size(); i++)
    {
        GammaTable[i] = static_cast<GammaTable_t::value_type>((std::numeric_limits<GammaTable_t::value_type>::max() + 1)*i / GammaTable.size());
    }
}

void C3300C::BuildGammaTable(GammaTable_t& GammaTable, const float fGamma)
{
    for (size_t i = 0; i < GammaTable.size(); i++)
    {
        GammaTable[i] = static_cast<GammaTable_t::value_type>((std::numeric_limits<GammaTable_t::value_type>::max() + 1) * pow(static_cast<float>(i) / GammaTable.size(), 1.0f / fGamma));
    }
}
