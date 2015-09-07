#include "niash_winusb.h"

//Windows
#include <windows.h>
#include <comdef.h>
#include <devpkey.h>
#include <usbscan.h>
//C++
#include <cassert>
#include <memory>
#include <iostream>
#include <vector>

static HANDLE s_hDevice = INVALID_HANDLE_VALUE; //!< File handle to device

static const char* pszName = "\\\\.\\Usbscan0";

static struct
{
	unsigned char BulkIn;
	unsigned char BulkOut;
	unsigned char Interrupt;
} s_Pipes;

void NiashLibUsbInit(TFnReportDevice* const pfnReportDevice)
{
	TScannerModel Model;
	pfnReportDevice(nullptr, pszName);
}

int NiashLibUsbOpen(char const * const pszName, EScannerModel* const peModel)
{
	//Open a handle to the device (FILE_FLAG_OVERLAPPED for WinUSB)
	
	s_hDevice = CreateFileA(pszName, GENERIC_READ|GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, NULL);
	if(s_hDevice == INVALID_HANDLE_VALUE)
	{
		wprintf_s(L"CreateFile: %s\n", _com_error(GetLastError()).ErrorMessage());
		return -1;
	}

	DEVICE_DESCRIPTOR dd;
	DWORD dwBytesReturned;
	if(!DeviceIoControl(s_hDevice,IOCTL_GET_DEVICE_DESCRIPTOR,nullptr,0,&dd,sizeof(dd),&dwBytesReturned,nullptr))
	{
		wprintf_s(L"DeviceIoControl: %s\n", _com_error(GetLastError()).ErrorMessage());
		return -1;
	}

	//Report device to niash
	TScannerModel* pModel;
	MatchUsbDevice(dd.usVendorId,dd.usProductId,&pModel);
	*peModel = pModel->eModel;

	return 1;
}

void NiashLibUsbExit (const int iHandle)
{
	assert(iHandle==1);
	CloseHandle(s_hDevice);
}

void NiashLibUsbWriteReg(const int iHandle, const SANE_Byte bReg, SANE_Byte bData)
{
	assert(iHandle==1);
	assert(s_hDevice!=INVALID_HANDLE_VALUE);

	IO_BLOCK IoBlock;
	IoBlock.uOffset = bReg;
	IoBlock.uLength = 0x01;
	IoBlock.pbyData = &bData;

	if(!DeviceIoControl(s_hDevice,static_cast<DWORD>(IOCTL_WRITE_REGISTERS),&IoBlock,sizeof(IoBlock),nullptr,0,nullptr,nullptr) && GetLastError() != ERROR_IO_PENDING)
	{
		wprintf_s(L"DeviceIoControl: %s\n", _com_error(GetLastError()).ErrorMessage());		
		return;
	}
}

void NiashLibUsbReadReg(const int iHandle, const SANE_Byte bReg, SANE_Byte* const pbData)
{
	assert(iHandle==1);
	assert(s_hDevice!=INVALID_HANDLE_VALUE);

	IO_BLOCK IoBlock;
	IoBlock.uOffset = bReg;
	IoBlock.uLength = 0x01;
	IoBlock.pbyData = nullptr;

	OVERLAPPED Overlapped = {};
	if(!DeviceIoControl(s_hDevice,static_cast<DWORD>(IOCTL_READ_REGISTERS),&IoBlock,sizeof(IoBlock),pbData,1,nullptr,nullptr) && GetLastError() != ERROR_IO_PENDING)
	{
		wprintf_s(L"DeviceIoControl: %s\n", _com_error(GetLastError()).ErrorMessage());		
		return;
	}

}

static bool SetupBulkTransfer(bool bWrite, const int iSize)
{
	assert(s_hDevice!=INVALID_HANDLE_VALUE);

	//Send setup packet
	SANE_Byte abSetup[8] = { bWrite ? 0x01 : 0x00, bWrite ? 0x01 : 0x00, 0x00, 0x00, (iSize) & 0xFF, (iSize >> 8) & 0xFF, 0x00, 0x00 };

	IO_BLOCK IoBlock;
	IoBlock.uOffset = USB_SETUP;
	IoBlock.uLength = sizeof(abSetup);
	IoBlock.pbyData = abSetup;

	if(!DeviceIoControl(s_hDevice,static_cast<DWORD>(IOCTL_WRITE_REGISTERS),&IoBlock,sizeof(IoBlock),nullptr,0,nullptr,nullptr) && GetLastError() != ERROR_IO_PENDING)
	{
		wprintf_s(L"DeviceIoControl: %s\n", _com_error(GetLastError()).ErrorMessage());		
		return false;
	}

	return true;
}

void NiashLibUsbWriteBulk(const int iHandle, SANE_Byte* const pabData, const int iSize)
{
	assert(iHandle==1);
	assert(s_hDevice!=INVALID_HANDLE_VALUE);

	if(!SetupBulkTransfer(true, iSize))
	{
		return;
	}

	DWORD dwBytesWritten;
	if(!WriteFile(s_hDevice,pabData,iSize,&dwBytesWritten,nullptr))
	{
		wprintf_s(L"WriteFile: %s\n", _com_error(GetLastError()).ErrorMessage());		
		return;
	}
}

void NiashLibUsbReadBulk(const int iHandle, SANE_Byte* const pabData, const int iSize)
{
	assert(iHandle==1);
	assert(s_hDevice!=INVALID_HANDLE_VALUE);

	if(!SetupBulkTransfer(false, iSize))
	{
		return;
	}

	DWORD dwBytesRead;
	if(!ReadFile(s_hDevice,pabData,iSize,&dwBytesRead,nullptr))
	{
		wprintf_s(L"ReadFile: %s\n", _com_error(GetLastError()).ErrorMessage());		
		return;
	}
}

void NiashLibUsbWaitForInterrupt()
{
	
}
