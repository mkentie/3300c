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
#include "Main.h"
#include "3300C.h"

HRESULT MicroEntry(const LONG lCommand, PVAL const pValue)
{
    assert(pValue);

    //Handle separately so context can be created
    if (lCommand == CMD_INITIALIZE)
    {
        if (pValue->pScanInfo->DeviceIOHandles[0] == INVALID_HANDLE_VALUE)
        {
            return E_HANDLE;
        }

        assert(!pValue->pScanInfo->pMicroDriverContext);
        std::unique_ptr<IScanner> pScanner = std::make_unique<C3300C>();

        pScanner->FillSCANINFO(*pValue->pScanInfo);

        if (!pScanner->Initialize(*pValue))
        {
            return E_FAIL;
        }

        pValue->pScanInfo->pMicroDriverContext = pScanner.release();

        return S_OK;
    }

    IScanner* const pScanner = static_cast<IScanner*>(pValue->pScanInfo->pMicroDriverContext);

    switch (lCommand)
    {
    case CMD_GETCAPABILITIES:
        assert(pScanner);
        pScanner->GetCapabilities(*pValue);
        return S_OK;
    case CMD_SETDATATYPE:
        pValue->pScanInfo->DataType = pValue->lVal;
        return S_OK;
    case CMD_SETCONTRAST:
        pValue->pScanInfo->Contrast = pValue->lVal;
        return S_OK;
    case CMD_SETINTENSITY:
        pValue->pScanInfo->Intensity = pValue->lVal;;
        return S_OK;
    case CMD_SETXRESOLUTION:
        pValue->pScanInfo->Xresolution = pValue->lVal;
        return S_OK;
    case CMD_SETYRESOLUTION:
        pValue->pScanInfo->Yresolution = pValue->lVal;
        return S_OK;
    case CMD_RESETSCANNER: //Fallthrough
    case CMD_STI_DEVICERESET:
        assert(pScanner);
        pScanner->Reset(*pValue);
        return S_OK;
    case CMD_STI_DIAGNOSTIC:
        return S_OK;
    case CMD_UNINITIALIZE:
        delete pScanner;
        return S_OK;
    case CMD_STI_GETSTATUS:
        assert(pScanner);
        pScanner->GetStatus(*pValue);
        return S_OK;

    default:
         return E_NOTIMPL;
    }
}

HRESULT Scan(PSCANINFO const pScanInfo, const LONG lPhase, PBYTE const pBuffer, const LONG lLength, LONG* const plReceived)
{
    IScanner* const pScanner = static_cast<IScanner*>(pScanInfo->pMicroDriverContext);
    assert(pScanner);

    switch (lPhase)
    {
    case SCAN_FIRST:
        if (!pScanner->ScanStart(*pScanInfo))
        {
            return E_FAIL;
        }

        if (!pScanner->ScanContinue(*pScanInfo, pBuffer, lLength, plReceived))
        {
            return E_FAIL;
        }
        break;
    case SCAN_NEXT:
        if (!pScanner->ScanContinue(*pScanInfo, pBuffer, lLength, plReceived))
        {
            return E_FAIL;
        }
        break;
    case SCAN_FINISHED:
        if (!pScanner->ScanFinish(*pScanInfo))
        {
            return E_FAIL;
        }
        break;
    default:
        assert(false);
    }

    return S_OK;
}

HRESULT SetPixelWindow(PSCANINFO const pScanInfo, const LONG x, const LONG y, const LONG xExtent, const LONG yExtent)
{
    assert(pScanInfo);
    pScanInfo->Window.xPos = x;
    pScanInfo->Window.yPos = y;
    pScanInfo->Window.xExtent = xExtent;
    pScanInfo->Window.yExtent = yExtent;
    return S_OK;
}
