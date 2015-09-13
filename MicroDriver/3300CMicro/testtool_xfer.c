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

  $Id: testtool_xfer.c,v 1.5 2006/03/04 14:27:51 ullsig Exp $
*/

/*
        Provides a simple interface to read and write data from the scanner,
        without any knowledge whether it's a parallel or USB scanner
*/

#include <stdio.h>              /* printf */
#include <errno.h>              /* better error reports */
#include <string.h>             /* better error reports */
#include <stdlib.h> //mkentie

#include "niash_win.h" //mkentie
#include "niash_xfer.h"

/* list of supported models */
STATIC TScannerModel ScannerModels[] = {
  {"Hewlett-Packard", "ScanJet 3300C", 0x3F0, 0x205, eHp3300c}
  ,
  {"Hewlett-Packard", "ScanJet 3400C", 0x3F0, 0x405, eHp3400c}
  ,
  {"Hewlett-Packard", "ScanJet 4300C", 0x3F0, 0x305, eHp4300c}
  ,
  {"Silitek Corp.", "HP ScanJet 4300c", 0x47b, 0x1002, eHp3400c}
  ,
  {"Agfa", "Snapscan Touch", 0x6BD, 0x100, eAgfaTouch}
  ,
  {"Trust", "Office Scanner USB 19200", 0x47b, 0x1000, eAgfaTouch}
  ,
/* last entry all zeros */
  {0, 0, 0, 0, 0}
};

/*
  MatchUsbDevice
  ==============
        Matches a given USB vendor and product id against a list of
        supported scanners.

  IN  iVendor   USB vendor ID
          iProduct  USB product ID
  OUT *ppModel  Pointer to TScannerModel structure

  Returns TRUE if a matching USB scanner was found
*/
STATIC SANE_Bool
MatchUsbDevice (int iVendor, int iProduct, TScannerModel ** ppModel)
{
  TScannerModel *pModels = ScannerModels;

  NIASHDBG (DBG_MSG, "Matching USB device 0x%04X-0x%04X ... ", iVendor, iProduct);
  while (pModels->pszName != NULL)
        {
          if ((pModels->iVendor == iVendor) && (pModels->iProduct == iProduct))
        {
          NIASHDBG (DBG_MSG, "found %s %s\n", pModels->pszVendor,
                   pModels->pszName);
          *ppModel = pModels;
          return SANE_TRUE;
        }
          /* next model to match */
          pModels++;
        }
  NIASHDBG (DBG_MSG, "nothing found\n");
  return SANE_FALSE;
}


/*
  NiashXferInit
  ===============
    Initialises all registered data transfer modules, which causes
    them to report any devices found through the pfnReport callback.

  IN  pfnReport Function to call to report a transfer device
*/
void
NiashXferInit (TFnReportDevice * pfnReport)
{
  NiashLibUsbInit(pfnReport);
}


BOOL
NiashXferOpen (HANDLE Handle, EScannerModel * peModel)
{

  NIASHDBG (DBG_MSG, "NiashXferOpen\n");

  /* try each method */
  NIASHDBG (DBG_MSG, "probing\n");
  *peModel = eUnknownModel;
  return NiashLibUsbOpen(Handle, peModel);
}


void
NiashXferClose (HANDLE Handle)
{

  if (Handle <= 0)
    {
      return;
    }

  /* deinit the driver */
  if (Handle >= 0)
    {
      NiashLibUsbExit (Handle);
    }
}


void
NiashWriteReg (HANDLE Handle, SANE_Byte bReg, SANE_Byte bData)
{
  if (Handle <= 0)
    {
      return;
    }

  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x14);
  NiashLibUsbWriteReg (Handle, EPP_ADDR, bReg);
  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x14);
  NiashLibUsbWriteReg (Handle, EPP_DATA_WRITE, bData);
  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x14);
}


void
NiashReadReg (HANDLE Handle, SANE_Byte bReg, SANE_Byte * pbData)
{
  if (Handle <= 0)
    {
      return;
    }

  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x14);
  NiashLibUsbWriteReg (Handle, EPP_ADDR, bReg);
  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x34);
  NiashLibUsbReadReg (Handle, EPP_DATA_READ, pbData);
  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x14);
}


void
NiashWriteBulk (HANDLE Handle, SANE_Byte * pabBuf, int iSize)
{

  if (Handle == INVALID_HANDLE_VALUE)
    {
      return;
    }
  /* select scanner register 0x24 */
  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x14);
  NiashLibUsbWriteReg (Handle, EPP_ADDR, 0x24);
  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x14);
  /* do the bulk write */
  NiashLibUsbWriteBulk (Handle, pabBuf, iSize);
}


DWORD
NiashReadBulk (HANDLE Handle, SANE_Byte * pabBuf, int iSize)
{
    if (Handle == INVALID_HANDLE_VALUE)
    {
      return 0;
    }

  /* select scanner register 0x24 */
  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x14);
  NiashLibUsbWriteReg (Handle, EPP_ADDR, 0x24);
  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x14);
  /* do the bulk read */
  return NiashLibUsbReadBulk (Handle, pabBuf, iSize);
}


void
NiashWakeup (HANDLE Handle)
{
  SANE_Byte abMagic[] = { 0xA0, 0xA8, 0x50, 0x58, 0x90, 0x98, 0xC0, 0xC8,
    0x90, 0x98, 0xE0, 0xE8
  };
  int i;

    if (Handle == INVALID_HANDLE_VALUE)
    {
      return;
    }

  /* write magic startup sequence */
  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x14);
  for (i = 0; i < (int) sizeof (abMagic); i++)
    {
      NiashLibUsbWriteReg (Handle, SPP_DATA, abMagic[i]);
    }

  /* write 0x04 to scanner register 0x00 the hard way */
  NiashLibUsbWriteReg (Handle, SPP_DATA, 0x00);
  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x14);
  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x15);
  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x1D);
  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x15);
  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x14);

  NiashLibUsbWriteReg (Handle, SPP_DATA, 0x04);
  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x14);
  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x15);
  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x17);
  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x15);
  NiashLibUsbWriteReg (Handle, SPP_CONTROL, 0x14);
}

