#include "stdafx.h"
#include "Main.h"
#include "3300C.h"

static std::unique_ptr<IScanner> s_pScanner(new C3300C());

 HRESULT MicroEntry(LONG lCommand, PVAL pValue)
{
    assert(pValue);
    assert(s_pScanner);
    switch (lCommand)
    {
    case CMD_GETCAPABILITIES:
        s_pScanner->GetCapabilities(*pValue);
        return S_OK;
    case CMD_INITIALIZE:
        if (pValue->pScanInfo->DeviceIOHandles[0]==INVALID_HANDLE_VALUE)
        {
            pValue->lVal = E_HANDLE;
            return E_HANDLE;
        }
        s_pScanner->FillSCANINFO(*pValue->pScanInfo);
        s_pScanner->Initialize(*pValue);
        return S_OK;	
    case CMD_SETDATATYPE:
        pValue->pScanInfo->DataType = pValue->lVal;
        return S_OK;
    case CMD_SETCONTRAST:
        pValue->pScanInfo->Contrast = pValue->lVal;
        return S_OK;
    case CMD_SETINTENSITY:
        pValue->pScanInfo->Intensity = pValue->lVal;
        return S_OK;
    case CMD_SETXRESOLUTION:
        pValue->pScanInfo->Xresolution = pValue->lVal;
        return S_OK;
    case CMD_SETYRESOLUTION:
        pValue->pScanInfo->Yresolution = pValue->lVal;
        return S_OK;
    case CMD_RESETSCANNER: //Fallthrough
    case CMD_STI_DEVICERESET:
        s_pScanner->Reset(*pValue);
        return S_OK;
    case CMD_STI_DIAGNOSTIC:
        return S_OK;
    case CMD_UNINITIALIZE:
        s_pScanner->Uninitialize();
        return S_OK;
    case CMD_STI_GETSTATUS:
        s_pScanner->GetStatus(*pValue);
        return S_OK;
    default:
    case CMD_SETSCANMODE: //TODO
         return E_NOTIMPL;
    }
}

HRESULT Scan(PSCANINFO pScanInfo, LONG lPhase, PBYTE pBuffer, LONG lLength, LONG* pReceived)
{
    assert(s_pScanner);
    if (lPhase == SCAN_FIRST)
    {
        s_pScanner->ScanStart(*pScanInfo);
        s_pScanner->ScanContinue(*pScanInfo, pBuffer, lLength, pReceived);
    }
    else if (lPhase == SCAN_NEXT)
    {
        s_pScanner->ScanContinue(*pScanInfo, pBuffer, lLength, pReceived);
    }
    else if (lPhase == SCAN_FINISHED)
    {
        s_pScanner->ScanFinish(*pScanInfo);
    }
    return S_OK;
}

HRESULT SetPixelWindow(PSCANINFO pScanInfo, LONG x, LONG y, LONG xExtent, LONG yExtent)
{
    assert(pScanInfo);
    pScanInfo->Window.xPos = x;
    pScanInfo->Window.yPos = y;
    pScanInfo->Window.xExtent = xExtent;
    pScanInfo->Window.yExtent = yExtent;
    return S_OK;
}
