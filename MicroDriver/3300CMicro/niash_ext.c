#include "niash_ext.h"

unsigned char GetButtonState(THWParams* const pHWParams)
{
    SANE_Byte Buttons;
    NiashReadReg(pHWParams->iXferHandle, 0x2E, &Buttons);
    return Buttons;
}

