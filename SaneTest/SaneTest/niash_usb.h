#pragma once

#include "niash_xfer.h"

#ifdef __cplusplus
extern "C"
{
#endif

void NiashLibUsbInit (TFnReportDevice* const pfnReportDevice);

int NiashLibUsbOpen (const char * const pszName, EScannerModel* const peModel);

void NiashLibUsbExit (int iHandle);

//Read an EPP parallel port register
void NiashLibUsbWriteReg (int iHandle, SANE_Byte bReg, SANE_Byte bData);

//Write an EPP parallel port register
void NiashLibUsbReadReg (int iHandle, SANE_Byte bReg, SANE_Byte * pbData);

void NiashLibUsbWriteBulk (int iHandle, SANE_Byte * pabData, int iSize);

void NiashLibUsbReadBulk (int iHandle, SANE_Byte * pabData, int iSize);

void NiashLibUsbWaitForInterrupt();

#ifdef __cplusplus
};
#endif