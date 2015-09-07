#include "niash_usb.h"

//Windows
#include <windows.h>
#include <winusb.h>
#include <setupapi.h>
#include <comdef.h>
#include <devpkey.h>
//C++
#include <cassert>
#include <memory>
#include <iostream>
#include <vector>

static const GUID s_NiashInterfaceClassGUID =  { 0x7BB55D8E, 0x9034, 0x4F62, { 0xAF, 0x70, 0x7E, 0x42, 0x86, 0xFD, 0x6D, 0x7D } };
static HANDLE s_hDevice = INVALID_HANDLE_VALUE; //!< File handle to device
static WINUSB_INTERFACE_HANDLE s_hWinUSB; //!< WinUSB handle to device
static TScannerModel* s_pScannerModel;

static struct
{
	unsigned char BulkIn;
	unsigned char BulkOut;
	unsigned char Interrupt;
} s_Pipes;

void NiashLibUsbInit(TFnReportDevice* const pfnReportDevice)
{
	class HDEVINFODeleter //RAII wrapper
	{
	public:
		typedef HDEVINFO pointer;
		void operator()(HDEVINFO h) {::SetupDiDestroyDeviceInfoList(h);}
	};

	//Get device
	std::unique_ptr<HDEVINFO, HDEVINFODeleter> hDevInfo(SetupDiGetClassDevs(&s_NiashInterfaceClassGUID, NULL, NULL, DIGCF_PRESENT|DIGCF_PROFILE|DIGCF_DEVICEINTERFACE));
	if(hDevInfo.get() == INVALID_HANDLE_VALUE)
	{
		wprintf_s(L"SetupDiGetClassDevs: %s\n", _com_error(GetLastError()).ErrorMessage());
		return;
	}
	
	//Get device info for the devices
	SP_DEVINFO_DATA DeviceInfoData;
	DeviceInfoData.cbSize = sizeof(DeviceInfoData);
	SetupDiEnumDeviceInfo(hDevInfo.get(),0,&DeviceInfoData);
	if(GetLastError()==ERROR_NO_MORE_ITEMS)
	{
		puts("No devices with the driver installed found.");
	}
	else
	{
		wprintf_s(L"SetupDiEnumDeviceInfo: %s\n", _com_error(GetLastError()).ErrorMessage());
	}

	//Get the first matching device interface of that device
	SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
	DeviceInterfaceData.cbSize = sizeof(DeviceInterfaceData);	
	if(!SetupDiEnumDeviceInterfaces(hDevInfo.get(), &DeviceInfoData, &s_NiashInterfaceClassGUID, 0, &DeviceInterfaceData))
	{
		wprintf_s(L"SetupDiEnumDeviceInterfaces: %s\n", _com_error(GetLastError()).ErrorMessage());
		return;
	}

	//Get size of detailed device interface data
	DWORD dwRequiredSize;
	if(!SetupDiGetDeviceInterfaceDetail(hDevInfo.get(), &DeviceInterfaceData, nullptr, 0, &dwRequiredSize, nullptr) && GetLastError() != ERROR_INSUFFICIENT_BUFFER)
	{
		wprintf_s(L"SetupDiGetDeviceInterfaceDetail: %s\n", _com_error(GetLastError()).ErrorMessage());
		return;
	}

	//SP_DEVICE_INTERFACE_DETAIL_DATA's actual size isn't declared
	std::unique_ptr<SP_DEVICE_INTERFACE_DETAIL_DATA_A> pInterfaceDetailData(reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_A*>(new BYTE[dwRequiredSize]));
	pInterfaceDetailData->cbSize = sizeof(*pInterfaceDetailData);
	if(!SetupDiGetDeviceInterfaceDetailA(hDevInfo.get(), &DeviceInterfaceData, pInterfaceDetailData.get(), dwRequiredSize, nullptr, nullptr))
	{
		wprintf_s(L"SetupDiGetDeviceInterfaceDetail: %s\n", _com_error(GetLastError()).ErrorMessage());
		return;
	}

	//Get name size
	ULONG ulPropType = DEVPROP_TYPE_STRING;
	if(!SetupDiGetDeviceProperty(hDevInfo.get(), &DeviceInfoData, &DEVPKEY_NAME, &ulPropType, nullptr, 0, &dwRequiredSize, 0) && GetLastError() != ERROR_INSUFFICIENT_BUFFER)
	{
		wprintf_s(L"SetupDiGetDeviceProperty: %s\n", _com_error(GetLastError()).ErrorMessage());
		return;
	}

	//Get device name
	std::vector<TCHAR> Buf(dwRequiredSize);
	if(!SetupDiGetDeviceProperty(hDevInfo.get(), &DeviceInfoData, &DEVPKEY_NAME, &ulPropType, reinterpret_cast<PBYTE>(Buf.data()), dwRequiredSize, 0, 0))
	{
		wprintf_s(L"SetupDiGetDeviceProperty: %s\n", _com_error(GetLastError()).ErrorMessage());
		return;
	}
	wprintf_s(L"Found device: %s ", Buf.data());
	printf_s("%s\n", pInterfaceDetailData->DevicePath);

	//Let driver recognize the device so it knows what parameters to use
	int vid = 0;
	int pid = 0;
	if(sscanf_s(pInterfaceDetailData->DevicePath,"\\\\?\\usb#vid_%x&pid_%x#", &vid, &pid) == 2 && MatchUsbDevice(vid,pid,&s_pScannerModel))
	{
		pfnReportDevice(s_pScannerModel, pInterfaceDetailData->DevicePath);
	}
}

int NiashLibUsbOpen(char const * const pszName, EScannerModel* const peModel)
{
	//Open a handle to the device (FILE_FLAG_OVERLAPPED for WinUSB)
	s_hDevice = CreateFileA(pszName, GENERIC_READ|GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	//s_hDevice = CreateFileA("\\\\.\\Usbscan0", GENERIC_READ|GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if(s_hDevice == INVALID_HANDLE_VALUE)
	{
		wprintf_s(L"CreateFile: %s\n", _com_error(GetLastError()).ErrorMessage());
		return -1;
	}

	//Open a WinUSB handle
	if(!WinUsb_Initialize(s_hDevice,&s_hWinUSB))
	{		
		wprintf_s(L"WinUsb_Initialize: %s\n", _com_error(GetLastError()).ErrorMessage());
		return -1;
	}

	//Get interface descriptor
	USB_INTERFACE_DESCRIPTOR USBInterfaceDescriptor;
	if(!WinUsb_QueryInterfaceSettings(s_hWinUSB,0,&USBInterfaceDescriptor))
	{
		wprintf_s(L"WinUsb_QueryInterfaceSettings: %s\n", _com_error(GetLastError()).ErrorMessage());
		return -1;
	}

	//Find pipes
	for(UCHAR i = 0;i < USBInterfaceDescriptor.bNumEndpoints;i++)
	{
		WINUSB_PIPE_INFORMATION Pipe;
		WinUsb_QueryPipe(s_hWinUSB,0,i,&Pipe);
		if(Pipe.PipeType == UsbdPipeTypeInterrupt)
		{
			s_Pipes.Interrupt = Pipe.PipeId;
		}
		else if(Pipe.PipeType == UsbdPipeTypeBulk)
		{
			if(USB_ENDPOINT_DIRECTION_IN(Pipe.PipeId))
			{
				s_Pipes.BulkIn = Pipe.PipeId;
			}
			else
			{
				s_Pipes.BulkOut = Pipe.PipeId;				
			}
		}
	}

	if(!s_Pipes.Interrupt || !s_Pipes.BulkIn || !s_Pipes.BulkOut)
	{
		puts("Not all required pipes found.");
		return -1;
	}

	assert(s_pScannerModel);
	*peModel = s_pScannerModel->eModel;

	return 1;
}

void NiashLibUsbExit (const int iHandle)
{
	assert(iHandle==1);
	WinUsb_Free(s_hWinUSB);
	CloseHandle(s_hDevice);
}

static void SendControlTransfer(WINUSB_SETUP_PACKET& Packet, BYTE* pBuf)
{
	assert(s_hWinUSB);
	DWORD dwBytesTransferred;
	if(!WinUsb_ControlTransfer(s_hWinUSB, Packet, pBuf, Packet.Length, &dwBytesTransferred, nullptr))
	{
		wprintf_s(L"WinUsb_ControlTransfer: %s\n", _com_error(GetLastError()).ErrorMessage());		
		return;
	}
}

void NiashLibUsbWriteReg(const int iHandle, const SANE_Byte bReg, SANE_Byte bData)
{
	assert(iHandle==1);
	WINUSB_SETUP_PACKET Packet;
	Packet.RequestType = BMREQUEST_HOST_TO_DEVICE<<7|BMREQUEST_VENDOR<<5; //Vendor type request, PC to device
	Packet.Request = 0x0c; //Send 1 byte
	Packet.Value = static_cast<USHORT>(bReg);
	Packet.Index = 0x00;	
	Packet.Length = 0x01;
	SendControlTransfer(Packet,&bData);
}

void NiashLibUsbReadReg(const int iHandle, const SANE_Byte bReg, SANE_Byte* const pbData)
{
	assert(iHandle==1);
	WINUSB_SETUP_PACKET Packet;
	Packet.RequestType = BMREQUEST_DEVICE_TO_HOST<<7|BMREQUEST_VENDOR<<5; //Vendor type request, device to PC
	Packet.Request = 0x0c; //Get 1 byte
	Packet.Value = static_cast<USHORT>(bReg);
	Packet.Index = 0x00;	
	Packet.Length = 0x01;
	SendControlTransfer(Packet,pbData);
}

void NiashLibUsbWriteBulk(const int iHandle, SANE_Byte* const pabData, const int iSize)
{
	assert(iHandle==1);
	assert(s_hWinUSB);
	assert(s_Pipes.BulkOut);

	//Send setup packet
	SANE_Byte abSetup[8] = { 0x01, 0x01, 0x00, 0x00, (iSize) & 0xFF, (iSize >> 8) & 0xFF, 0x00, 0x00 };
	WINUSB_SETUP_PACKET Packet;
	Packet.RequestType = BMREQUEST_HOST_TO_DEVICE<<7|BMREQUEST_VENDOR<<5; //Vendor type request, PC to device
	Packet.Request = 0x04; //Send multiple bytes
	Packet.Value = static_cast<USHORT>(USB_SETUP);
	Packet.Index = 0x00;	
	Packet.Length = 0x08;
	SendControlTransfer(Packet,abSetup);

	ULONG LengthTransferred;
	if(!WinUsb_WritePipe(s_hWinUSB, s_Pipes.BulkOut, pabData, iSize, &LengthTransferred, nullptr))
	{
		wprintf_s(L"WinUsb_WritePipe: %s\n", _com_error(GetLastError()).ErrorMessage());		
		return;
	}
}

void NiashLibUsbReadBulk (const int iHandle, SANE_Byte* const pabData, const int iSize)
{
	assert(iHandle==1);
	assert(s_hWinUSB);
	assert(s_Pipes.BulkIn);

	//Send setup packet
	SANE_Byte abSetup[8] = { 0x00, 0x00, 0x00, 0x00, (iSize) & 0xFF, (iSize >> 8) & 0xFF, 0x00, 0x00 };
	WINUSB_SETUP_PACKET Packet;
	Packet.RequestType = BMREQUEST_HOST_TO_DEVICE<<7|BMREQUEST_VENDOR<<5; //Vendor type request, PC to device
	Packet.Request = 0x04; //Send multiple bytes
	Packet.Value = static_cast<USHORT>(USB_SETUP);
	Packet.Index = 0x00;	
	Packet.Length = 0x08;
	SendControlTransfer(Packet,abSetup);

	ULONG LengthTransferred;
	if(!WinUsb_ReadPipe(s_hWinUSB, s_Pipes.BulkIn, pabData, iSize, &LengthTransferred, nullptr))
	{
		wprintf_s(L"WinUsb_WritePipe: %s\n", _com_error(GetLastError()).ErrorMessage());		
		return;
	}
}

void NiashLibUsbWaitForInterrupt()
{
	assert(s_hWinUSB);
	assert(s_Pipes.Interrupt);
	BYTE buf[1];
	ULONG LengthTransferred;
	if(!WinUsb_ReadPipe(s_hWinUSB,s_Pipes.Interrupt,buf,_countof(buf),&LengthTransferred,nullptr))
	{
		wprintf_s(L"WinUsb_ReadPipe: %s\n", _com_error(GetLastError()).ErrorMessage());		
		return;
	}
}
