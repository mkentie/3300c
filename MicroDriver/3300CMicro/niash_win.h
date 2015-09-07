//Niash back-end for Windows USB scanning support

#pragma once

#include "niash_xfer.h"

#ifdef __cplusplus
extern "C"
{
#endif

void NiashLibUsbInit(TFnReportDevice* const pfnReportDevice);

int NiashLibUsbOpen(const char* const pszName, EScannerModel* const peModel);

void NiashLibUsbExit(const int iHandle);

/**
Read an EPP parallel port register
*/
void NiashLibUsbWriteReg(const int iHandle, const SANE_Byte bReg, const SANE_Byte bData);

/**
Write an EPP parallel port register
*/
void NiashLibUsbReadReg(const int iHandle, const SANE_Byte bReg, SANE_Byte* const pbData);

void NiashLibUsbWriteBulk(const int iHandle, SANE_Byte* const pabData, const int iSize);

DWORD NiashLibUsbReadBulk(const int iHandle, SANE_Byte* const pabData, const int iSize);

#ifdef __cplusplus
};

//Custom extra functions

/**
Sets the Windows device handle which the back-end will use.
*/
void NiashLibUsbSetDeviceHandle(const HANDLE DeviceHandle);

#endif