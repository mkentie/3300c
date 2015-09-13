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

//Niash back-end for Windows USB scanning support

#pragma once

#include "niash_xfer.h"

#ifdef __cplusplus
extern "C"
{
#endif

void NiashLibUsbInit(TFnReportDevice* const pfnReportDevice);

BOOL NiashLibUsbOpen(const HANDLE Handle, EScannerModel* const peModel);

BOOL NiashLibUsbExit(const HANDLE Handle);

/**
Read an EPP parallel port register
*/
BOOL NiashLibUsbWriteReg(const HANDLE Handle, const SANE_Byte bReg, const SANE_Byte bData);

/**
Write an EPP parallel port register
*/
BOOL NiashLibUsbReadReg(const HANDLE Handle, const SANE_Byte bReg, SANE_Byte* const pbData);

BOOL NiashLibUsbWriteBulk(const HANDLE Handle, SANE_Byte* const pabData, const int iSize);

DWORD NiashLibUsbReadBulk(const HANDLE Handle, SANE_Byte* const pabData, const int iSize);

#ifdef __cplusplus
};
#endif