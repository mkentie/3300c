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
#include "niash_win.h"

void NiashLibUsbInit(TFnReportDevice* const /*pfnReportDevice*/)
{
    //No need to enumerate as we're attached to a specific device
}

BOOL NiashLibUsbOpen(const HANDLE Handle, EScannerModel* const peModel)
{
    assert(Handle != INVALID_HANDLE_VALUE);

    //Get device descriptor so we can get vid & pid
    DEVICE_DESCRIPTOR dd;
    DWORD dwBytesReturned;
    if(!DeviceIoControl(Handle, static_cast<DWORD>(IOCTL_GET_DEVICE_DESCRIPTOR), nullptr, 0, &dd, sizeof(dd), &dwBytesReturned, nullptr))
    {
        DBGMSG(L"DeviceIoControl: %s\n", _com_error(GetLastError()).ErrorMessage());
        return FALSE;
    }

    //Report device to niash
    TScannerModel* pModel;
    if(MatchUsbDevice(dd.usVendorId, dd.usProductId, &pModel) != SANE_TRUE)
    {
        return FALSE;
    }
    *peModel = pModel->eModel;

    return TRUE;
}

BOOL NiashLibUsbExit (const HANDLE Handle)
{
    assert(Handle != INVALID_HANDLE_VALUE);
    return TRUE;
}

BOOL NiashLibUsbWriteReg(const HANDLE Handle, const SANE_Byte bReg, SANE_Byte bData)
{
    //Reverse-engineered behavior from Linux driver exactly matches IOCTL_WRITE_REGISTERS
    assert(Handle != INVALID_HANDLE_VALUE);

    IO_BLOCK IoBlock;
    IoBlock.uOffset = bReg;
    IoBlock.uLength = 0x01;
    IoBlock.pbyData = &bData;

    DWORD dwBytesReturned; //Required for Win7

    if (!DeviceIoControl(Handle, static_cast<DWORD>(IOCTL_WRITE_REGISTERS), &IoBlock, sizeof(IoBlock), nullptr, 0, &dwBytesReturned, nullptr) && GetLastError() != ERROR_IO_PENDING)
    {
        DBGMSG(L"DeviceIoControl: %s\n", _com_error(GetLastError()).ErrorMessage());
        return FALSE;
    }

    return TRUE;
}

BOOL NiashLibUsbReadReg(const HANDLE Handle, const SANE_Byte bReg, SANE_Byte* const pbData)
{
    //Reverse-engineered behavior exactly matches IOCTL_READ_REGISTERS
    assert(Handle != INVALID_HANDLE_VALUE);

    IO_BLOCK IoBlock;
    IoBlock.uOffset = bReg;
    IoBlock.uLength = 0x01;
    IoBlock.pbyData = nullptr;

    DWORD dwBytesReturned; //Required for Win7

    if (!DeviceIoControl(Handle, static_cast<DWORD>(IOCTL_READ_REGISTERS), &IoBlock, sizeof(IoBlock), pbData, 1, &dwBytesReturned, nullptr) && GetLastError() != ERROR_IO_PENDING)
    {
        DBGMSG(L"DeviceIoControl: %s\n", _com_error(GetLastError()).ErrorMessage());
        return FALSE;
    }

    return TRUE;
}

static bool SetupBulkTransfer(const HANDLE Handle, bool bWrite, const int iSize)
{
    assert(Handle != INVALID_HANDLE_VALUE);

    //Send setup packet
    BYTE abSetup[8] = { static_cast<BYTE>(bWrite ? 0x01 : 0x00), static_cast<BYTE>(bWrite ? 0x01 : 0x00), 0x00, 0x00, static_cast<BYTE>(iSize & 0xFF), static_cast<BYTE>((iSize >> 8) & 0xFF), 0x00, 0x00 };

    IO_BLOCK IoBlock;
    IoBlock.uOffset = USB_SETUP;
    IoBlock.uLength = sizeof(abSetup);
    IoBlock.pbyData = abSetup;

    DWORD dwBytesReturned; //Required for Win7

    if (!DeviceIoControl(Handle, static_cast<DWORD>(IOCTL_WRITE_REGISTERS), &IoBlock, sizeof(IoBlock), nullptr, 0, &dwBytesReturned, nullptr) && GetLastError() != ERROR_IO_PENDING)
    {
        DBGMSG(L"DeviceIoControl: %s\n", _com_error(GetLastError()).ErrorMessage());
        return false;
    }

    return true;
}

BOOL NiashLibUsbWriteBulk(const HANDLE Handle, SANE_Byte* const pabData, const int iSize)
{
    assert(Handle != INVALID_HANDLE_VALUE);

    if (!SetupBulkTransfer(Handle, true, iSize))
    {
        return FALSE;
    }

    DWORD dwBytesWritten;
    if (!WriteFile(Handle,pabData,iSize,&dwBytesWritten,nullptr))
    {
        DBGMSG(L"WriteFile: %s\n", _com_error(GetLastError()).ErrorMessage());
        return FALSE;
    }

    return TRUE;
}

DWORD NiashLibUsbReadBulk(const HANDLE Handle, SANE_Byte* const pabData, const int iSize)
{
    assert(Handle != INVALID_HANDLE_VALUE);

    if (!SetupBulkTransfer(Handle, false, iSize))
    {
        return 0;
    }

    DWORD dwBytesRead;
    if (!ReadFile(Handle, pabData,iSize,&dwBytesRead,nullptr))
    {
        DBGMSG(L"ReadFile: %s\n", _com_error(GetLastError()).ErrorMessage());
        return 0;
    }

    return dwBytesRead;
}
