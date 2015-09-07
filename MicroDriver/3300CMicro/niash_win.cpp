#include "stdafx.h"
#include "Main.h"
#include "niash_win.h"

static HANDLE s_hDevice = INVALID_HANDLE_VALUE; //!< File handle to device

void NiashLibUsbInit(TFnReportDevice* const /*pfnReportDevice*/)
{
    //No need to enumerate as we're attached to a specific device
}

int NiashLibUsbOpen(char const * const /*pszName*/, EScannerModel* const peModel)
{
    assert(s_hDevice != INVALID_HANDLE_VALUE);

    //Get device descriptor so we van get vid & pid
    DEVICE_DESCRIPTOR dd;
    DWORD dwBytesReturned;
    if(!DeviceIoControl(s_hDevice, static_cast<DWORD>(IOCTL_GET_DEVICE_DESCRIPTOR), nullptr, 0, &dd, sizeof(dd), &dwBytesReturned, nullptr))
    {
        DBGMSG(L"DeviceIoControl: %s\n", _com_error(GetLastError()).ErrorMessage());
        return -1;
    }

    //Report device to niash
    TScannerModel* pModel;
    if(MatchUsbDevice(dd.usVendorId, dd.usProductId, &pModel) != SANE_TRUE)
    {
        return -1;
    }
    *peModel = pModel->eModel;

    return 1;
}

void NiashLibUsbExit (const int iHandle)
{
    assert(iHandle == 1);
}

void NiashLibUsbWriteReg(const int iHandle, const SANE_Byte bReg, SANE_Byte bData)
{
    //Reverse-engineered behavior from Linux driver exactly matches IOCTL_WRITE_REGISTERS
    assert(iHandle == 1);
    assert(s_hDevice != INVALID_HANDLE_VALUE);

    IO_BLOCK IoBlock;
    IoBlock.uOffset = bReg;
    IoBlock.uLength = 0x01;
    IoBlock.pbyData = &bData;

    if (!DeviceIoControl(s_hDevice, static_cast<DWORD>(IOCTL_WRITE_REGISTERS), &IoBlock, sizeof(IoBlock), nullptr, 0, nullptr, nullptr) && GetLastError() != ERROR_IO_PENDING)
    {
        DBGMSG(L"DeviceIoControl: %s\n", _com_error(GetLastError()).ErrorMessage());
        return;
    }

    if (bReg == EPP_ADDR)
    {
        DBGMSG(L"Select register %x\n", bData);
    }
    if (bReg == EPP_DATA_WRITE)
    {
        DBGMSG(L"Write value %x\n", bData);
    }
}

void NiashLibUsbReadReg(const int iHandle, const SANE_Byte bReg, SANE_Byte* const pbData)
{
    //Reverse-engineered behavior exactly matches IOCTL_READ_REGISTERS
    assert(iHandle == 1);
    assert(s_hDevice != INVALID_HANDLE_VALUE);

    IO_BLOCK IoBlock;
    IoBlock.uOffset = bReg;
    IoBlock.uLength = 0x01;
    IoBlock.pbyData = nullptr;

    if (!DeviceIoControl(s_hDevice, static_cast<DWORD>(IOCTL_READ_REGISTERS), &IoBlock, sizeof(IoBlock), pbData, 1, nullptr, nullptr) && GetLastError() != ERROR_IO_PENDING)
    {
        DBGMSG(L"DeviceIoControl: %s\n", _com_error(GetLastError()).ErrorMessage());
        return;
    }

}

static bool SetupBulkTransfer(bool bWrite, const int iSize)
{
    assert(s_hDevice != INVALID_HANDLE_VALUE);

    //Send setup packet
    BYTE abSetup[8] = { static_cast<BYTE>(bWrite ? 0x01 : 0x00), static_cast<BYTE>(bWrite ? 0x01 : 0x00), 0x00, 0x00, static_cast<BYTE>(iSize & 0xFF), static_cast<BYTE>((iSize >> 8) & 0xFF), 0x00, 0x00 };

    IO_BLOCK IoBlock;
    IoBlock.uOffset = USB_SETUP;
    IoBlock.uLength = sizeof(abSetup);
    IoBlock.pbyData = abSetup;

    if (!DeviceIoControl(s_hDevice, static_cast<DWORD>(IOCTL_WRITE_REGISTERS), &IoBlock, sizeof(IoBlock), nullptr, 0, nullptr, nullptr) && GetLastError() != ERROR_IO_PENDING)
    {
        DBGMSG(L"DeviceIoControl: %s\n", _com_error(GetLastError()).ErrorMessage());
        return false;
    }

    return true;
}

void NiashLibUsbWriteBulk(const int iHandle, SANE_Byte* const pabData, const int iSize)
{
    assert(iHandle == 1);
    assert(s_hDevice != INVALID_HANDLE_VALUE);

    if (!SetupBulkTransfer(true, iSize))
    {
        return;
    }

    DWORD dwBytesWritten;
    if (!WriteFile(s_hDevice,pabData,iSize,&dwBytesWritten,nullptr))
    {
        DBGMSG(L"WriteFile: %s\n", _com_error(GetLastError()).ErrorMessage());
        return;
    }
}

DWORD NiashLibUsbReadBulk(const int iHandle, SANE_Byte* const pabData, const int iSize)
{
    assert(iHandle==1);
    assert(s_hDevice!=INVALID_HANDLE_VALUE);

    if (!SetupBulkTransfer(false, iSize))
    {
        return 0;
    }

    DWORD dwBytesRead;
    if (!ReadFile(s_hDevice, pabData,iSize,&dwBytesRead,nullptr))
    {
        DBGMSG(L"ReadFile: %s\n", _com_error(GetLastError()).ErrorMessage());
        return 0;
    }

    return dwBytesRead;
}

void NiashLibUsbSetDeviceHandle(const HANDLE DeviceHandle)
{
    s_hDevice = DeviceHandle;
}

