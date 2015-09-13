/*
Copyright (C) 2015 Marijn Kentie

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

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

C3300C::~C3300C()
{
    SetLamp(&m_HWParams, 0);
    NiashClose(&m_HWParams);
}

void C3300C::FillSCANINFO(SCANINFO& ScanInfo) const 
{
    /* From XP driver:
        ADF	0	long
        TPA	0	long
        Endorser	0	long
        OpticalXResolution	600	long
        OpticalYResolution	600	long
        BedWidth	8500	long
        BedHeight	11700	long
        IntensityRange	{lMin=0 lMax=1 lStep=1 }	_RANGEVALUE
        ContrastRange	{lMin=0 lMax=1 lStep=1 }	_RANGEVALUE
        SupportedCompressionType	0	long
        SupportedDataTypes	7	long
        WidthPixels	0	long
        WidthBytes	0	long
        Lines	0	long
        DataType	3	long
        PixelBits	24	long
        Intensity	0	long
        Contrast	0	long
        Xresolution	75	long
        Yresolution	75	long
        Window	{xPos=0 yPos=0 xExtent=0 ...}	_SCANWINDOW
        DitherPattern	0	long
        Negative	0	long
        Mirror	0	long
        AutoBack	1	long
        ColorDitherPattern	0	long
        ToneMap	0	long
        Compression	0	long
        RawDataFormat	0	long
        RawPixelOrder	1	long
        bNeedDataAlignment	1	long
        DelayBetweenRead	0	long

    */

    //MSDN suggests all members should be initialized and the MicroDriver sample does so too
    ScanInfo.ADF = 0;
    ScanInfo.TPA = 0;
    ScanInfo.Endorser = 0;
    ScanInfo.OpticalXResolution = HW_DPI;
    ScanInfo.OpticalYResolution = HW_DPI;
    ScanInfo.BedWidth = 8500; //From XP driver; trying to go wider just leads to corrupt lines
    ScanInfo.BedHeight = 11700; // 11700; //From XP driver
    ScanInfo.IntensityRange.lMin = 0; //These don't actually grey out the user interface or change the slider ranges...
    ScanInfo.IntensityRange.lMax = 0;
    ScanInfo.IntensityRange.lStep = 0;
    ScanInfo.ContrastRange.lMin = 0;
    ScanInfo.ContrastRange.lMax = 0;
    ScanInfo.ContrastRange.lStep = 0;
    ScanInfo.SupportedCompressionType = 0;
    ScanInfo.SupportedDataTypes = SUPPORT_COLOR;

    //XP driver uses 0 for all of these
    ScanInfo.Intensity = 0;
    ScanInfo.Contrast = 0;
    ScanInfo.Xresolution = 0;
    ScanInfo.Yresolution = 0;
    ScanInfo.Window.xPos = 0;
    ScanInfo.Window.yPos = 0;
    ScanInfo.Window.xExtent = 0;
    ScanInfo.Window.yExtent = 0;
    ScanInfo.WidthPixels = 0;
    ScanInfo.Lines = 0;
    ScanInfo.WidthBytes = 0;

    ScanInfo.DataType = WIA_DATA_COLOR;
    ScanInfo.PixelBits = 24;

    ScanInfo.DitherPattern = 0;
    ScanInfo.Negative = 0;
    ScanInfo.Mirror = 0;
    ScanInfo.AutoBack = 1; //From XP driver
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
    const unsigned char ButtonState = GetButtonState(&m_HWParams);

    Value.pGuid = (ButtonState & EScannerButtonBits::ButtonScan) ? &sm_ButtonGUIDs[0] : (ButtonState & EScannerButtonBits::ButtonPrint) ? &sm_ButtonGUIDs[1] : reinterpret_cast<GUID*>(const_cast<IID*>(&GUID_NULL));
}

void C3300C::Reset(VAL& /*Value*/)
{
    m_bStatusOk = true;
}

bool C3300C::Initialize(VAL& Value)
{
    DBGMSG();

    assert(Value.pScanInfo);
    const SCANINFO& ScanInfo = *Value.pScanInfo;
    m_HWParams.XferHandle = ScanInfo.DeviceIOHandles[0];
    if (NiashOpen(&m_HWParams, Value.szVal) < 0)
    {
        m_bStatusOk = false;
        return false;
    }

    //Already turn on lamp for quicker calibration once user starts to scan; auto turns off after 15 minutes
    SetLamp(&m_HWParams, 1);

    return true;
}

bool C3300C::ScanStart(const SCANINFO& ScanInfo)
{
    DBGMSG();

    CalibTable_t CalibTable;

    if (!WarmupLamp(CalibTable))
    {
        m_bStatusOk = false;
        DBGMSG(L"WarmupLamp() failure.");
        return false;
    }

    WriteGammaCalibTable(hpgt33tk::GammaTableR.data(), hpgt33tk::GammaTableG.data(), hpgt33tk::GammaTableB.data(), CalibTable.data(), 0, 0, &m_HWParams);

    TScanParams ScanParams;
    ScanParams.iDpi = ScanInfo.Xresolution;
    ScanParams.iLpi = ScanInfo.Yresolution;

    /*
    150 dpi: 66 pixels above start of document scanned, value written is 466 to my logs, 452 according to SANE driver docs, and in practice I needed 490 to pixel match XP driver.
    300 dpi: 132 pixels
    600 dpi: 264 pixels
    Always 14 pixel lines of garbage, so need to start that much before document
    ScanInfo.Window is in pixels
    ScanParams iTop/iLeft is in native dots (600 dpi horz, 1200 dpi vert)
    */

    ScanParams.iTop = 602 - static_cast<int>(Helpers::PixelsToDots(m_HWParams.iSkipLines, ScanInfo.Yresolution, HW_LPI) + Helpers::PixelsToDots(ScanInfo.Window.yPos, ScanInfo.Yresolution, HW_LPI));
    ScanParams.iLeft = 102 + static_cast<int>(Helpers::PixelsToDots(ScanInfo.Window.xPos, ScanInfo.Xresolution, HW_DPI)); //102 from XP driver log
    ScanParams.iWidth = ScanInfo.Window.xExtent;
    ScanParams.iHeight = ScanInfo.Window.yExtent;
    ScanParams.iBottom = HP3300C_BOTTOM; //Doesn't matter
    ScanParams.fCalib = SANE_FALSE;

    m_iLinesLeftToSkip = m_HWParams.iSkipLines;

    if (InitScan(&ScanParams, &m_HWParams) != SANE_TRUE)
    {
        DBGMSG(L"InitScan() failure.");
        m_bStatusOk = false;
        return false;
    }

    return true;
}

bool C3300C::ScanContinue(const SCANINFO& ScanInfo, BYTE* const pBuffer, const long lLength, long* const pReceived)
{
    const size_t iPixelsPerLine = ScanInfo.Window.xExtent;
    const size_t iBytesPerComponent = iPixelsPerLine;
    const size_t iBytesPerLine = 3 * iPixelsPerLine;

    m_BackBuffer.resize(lLength);

    size_t iNewLinesToProcess = 0;
    size_t iNewLinesToSkip = 0;

    //Keep reading until we've skipped all necessary lines; it's possible that in one read action, both lines-to-be-skipped and the first good lines are returned
    while (iNewLinesToProcess == 0)
    {
        const size_t iNewBytesReceived = NiashReadBulk(ScanInfo.DeviceIOHandles[0], m_BackBuffer.data(), lLength);

        if (iNewBytesReceived == 0) //Our USB functions' status doesn't propagate through the SANE driver layer, thankfully this is good enough to stop scanning
        {
            m_bStatusOk = false;
            return false;
        }

        assert(iNewBytesReceived % iBytesPerLine == 0);
        const size_t iNewLinesReceived = iNewBytesReceived / iBytesPerLine;

        iNewLinesToSkip = std::min(m_iLinesLeftToSkip, iNewLinesReceived);
        iNewLinesToProcess = iNewLinesReceived - iNewLinesToSkip;
        m_iLinesLeftToSkip -= iNewLinesToSkip;
    }

    //Convert Blue line / Red line / Green line to 24bit BGR, and flip along horizontal axis as well
    for (size_t iLine = 0; iLine < iNewLinesToProcess; iLine++)
    {
        const BYTE* const pBackBufferStart = &m_BackBuffer[(iNewLinesToSkip + iLine)*iBytesPerLine];
        const BYTE* const pBlue = &pBackBufferStart[0];
        const BYTE* const pGreen = &pBackBufferStart[iBytesPerComponent];
        const BYTE* const pRed = &pBackBufferStart[2*iBytesPerComponent];
        BGR* const pTarget = reinterpret_cast<BGR*>(&pBuffer[iLine*iBytesPerLine]);
        for (size_t iPixel = 0; iPixel < iPixelsPerLine; iPixel++)
        {
            BGR& Target = pTarget[ScanInfo.Window.xExtent - iPixel - 1];

            Target.components.b = pBlue[iPixel];
            Target.components.g = pGreen[iPixel];
            Target.components.r = pRed[iPixel];
        }
    }

    *pReceived = static_cast<DWORD>(iNewLinesToProcess * iBytesPerLine);

    //DBGMSG(L"req: %d read: %d", lLength, *pReceived);

    return true;
}

bool C3300C::ScanFinish(const SCANINFO& /*ScanInfo*/)
{
    m_BackBuffer.clear();
    m_BackBuffer.shrink_to_fit();
    FinishScan(&m_HWParams);

    DBGMSG(L"Finished.");

    return true;
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

//#ifndef _DEBUG
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
//#endif

    return true;
}
