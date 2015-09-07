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

#include "niash_usb.h"
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

  DBG (DBG_MSG, "Matching USB device 0x%04X-0x%04X ... ", iVendor, iProduct);
  while (pModels->pszName != NULL)
        {
          if ((pModels->iVendor == iVendor) && (pModels->iProduct == iProduct))
        {
          DBG (DBG_MSG, "found %s %s\n", pModels->pszVendor,
                   pModels->pszName);
          *ppModel = pModels;
          return SANE_TRUE;
        }
          /* next model to match */
          pModels++;
        }
  DBG (DBG_MSG, "nothing found\n");
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


int
NiashXferOpen (const char *pszName, EScannerModel * peModel)
{
  int iHandle;

  DBG (DBG_MSG, "NiashXferOpen '%s'\n", pszName);

  /* try each method */
  DBG (DBG_MSG, "probing %s\n", pszName);
  *peModel = eUnknownModel;
  iHandle = NiashLibUsbOpen(pszName, peModel);
  return iHandle;
}


void
NiashXferClose (int iHandle)
{

  if (iHandle <= 0)
    {
      return;
    }

  /* deinit the driver */
  if (iHandle >= 0)
    {
      NiashLibUsbExit (iHandle);
    }
}


void
NiashWriteReg (int iHandle, SANE_Byte bReg, SANE_Byte bData)
{
  if (iHandle <= 0)
    {
      return;
    }

  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x14);
  NiashLibUsbWriteReg (iHandle, EPP_ADDR, bReg);
  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x14);
  NiashLibUsbWriteReg (iHandle, EPP_DATA_WRITE, bData);
  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x14);
}


void
NiashReadReg (int iHandle, SANE_Byte bReg, SANE_Byte * pbData)
{
  if (iHandle <= 0)
    {
      return;
    }

  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x14);
  NiashLibUsbWriteReg (iHandle, EPP_ADDR, bReg);
  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x34);
  NiashLibUsbReadReg (iHandle, EPP_DATA_READ, pbData);
  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x14);
}


void
NiashWriteBulk (int iHandle, SANE_Byte * pabBuf, int iSize)
{

  if (iHandle <= 0)
    {
      return;
    }
  /* select scanner register 0x24 */
  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x14);
  NiashLibUsbWriteReg (iHandle, EPP_ADDR, 0x24);
  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x14);
  /* do the bulk write */
  NiashLibUsbWriteBulk (iHandle, pabBuf, iSize);
}


void
NiashReadBulk (int iHandle, SANE_Byte * pabBuf, int iSize)
{
  if (iHandle <= 0)
    {
      return;
    }

  /* select scanner register 0x24 */
  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x14);
  NiashLibUsbWriteReg (iHandle, EPP_ADDR, 0x24);
  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x14);
  /* do the bulk read */
  NiashLibUsbReadBulk (iHandle, pabBuf, iSize);
}


void
NiashWakeup (int iHandle)
{
  SANE_Byte abMagic[] = { 0xA0, 0xA8, 0x50, 0x58, 0x90, 0x98, 0xC0, 0xC8,
    0x90, 0x98, 0xE0, 0xE8
  };
  int i;

  if (iHandle <= 0)
    {
      return;
    }

  /* write magic startup sequence */
  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x14);
  for (i = 0; i < (int) sizeof (abMagic); i++)
    {
      NiashLibUsbWriteReg (iHandle, SPP_DATA, abMagic[i]);
    }

  /* write 0x04 to scanner register 0x00 the hard way */
  NiashLibUsbWriteReg (iHandle, SPP_DATA, 0x00);
  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x14);
  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x15);
  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x1D);
  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x15);
  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x14);

  NiashLibUsbWriteReg (iHandle, SPP_DATA, 0x04);
  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x14);
  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x15);
  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x17);
  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x15);
  NiashLibUsbWriteReg (iHandle, SPP_CONTROL, 0x14);
}

