/*
  Copyright (C) 2001 Bertrik Sikken (bertrik@zonnet.nl)

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

  $Id: niash_xfer.h,v 1.7 2005/01/16 09:39:40 ullsig Exp $
*/

/*
    Provides a simple interface to read and write data from the scanner,
    without any knowledge whether it's a parallel or USB scanner
*/

#ifndef _NIASH_XFER_H_
#define _NIASH_XFER_H_
#include <Windows.h> //mkentie, OutputDebugString
#include <stdio.h>		/* for FILE * */

/* register codes for the USB - IEEE1284 bridge */
#define USB_SETUP       0x82
#define EPP_ADDR        0x83
#define EPP_DATA_READ   0x84
#define EPP_DATA_WRITE  0x85
#define SPP_STATUS      0x86
#define SPP_CONTROL     0x87
#define SPP_DATA        0x88


typedef enum
{
  eUnknownModel = 0,
  eHp3300c,
  eHp3400c,
  eHp4300c,
  eAgfaTouch
} EScannerModel;


typedef struct
{
  char *pszVendor;
  char *pszName;
  int iVendor;
  int iProduct;
  EScannerModel eModel;
} TScannerModel;

#ifdef __cplusplus //mkentie
extern "C"
{
#endif
	typedef int (TFnReportDevice)(TScannerModel * pModel,
		const char *pszDeviceName);


/* Creates our own DBG definitions, externs are define in main.c*/
#ifndef WITH_NIASH

//mkentie (also renamed from DBG because of WDK collision)
//Can't use template sprintf_s as we're in a .c file
#define NIASHDBG(f,...) { \
	char szBuf[1024]; \
	sprintf_s(szBuf, _countof(szBuf), __VA_ARGS__); \
	OutputDebugStringA(szBuf); \
}

extern FILE *DBG_MSG;
extern FILE *DBG_ERR;
extern FILE *BG_ASSERT;
#endif /* NO WITH_NIASH */

/* we do not make data prototypes */
#ifndef WITH_NIASH
/* list of supported models, the actual list is in niash_xfer.c */
extern TScannerModel ScannerModels[];
#endif /* NO WITH_NIASH */

STATIC void NiashXferInit (TFnReportDevice * pfnReport);
STATIC BOOL NiashXferOpen (HANDLE Handle, EScannerModel * peModel);
STATIC void NiashXferClose (HANDLE Handle);

STATIC void NiashWriteReg (HANDLE Handle, unsigned char bReg,
			   unsigned char bData);
STATIC void NiashReadReg (HANDLE Handle, unsigned char bReg,
			  unsigned char *pbData);
STATIC void NiashWriteBulk (HANDLE Handle, unsigned char *pabBuf,
			    int iSize);
STATIC DWORD NiashReadBulk (HANDLE Handle, unsigned char *pabBuf, int iSize);
STATIC void NiashWakeup (HANDLE Handle);

STATIC SANE_Bool MatchUsbDevice (int iVendor, int iProduct,
				 TScannerModel ** ppeModel);

#ifdef __cplusplus //mkentie
}
#endif

#endif /* _NIASH_XFER_H_ */
