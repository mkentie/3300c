//Extra niash functions not provided by the Linux driver
#pragma once

#include "niash_core.h"

#ifdef __cplusplus
extern "C"
{
#endif

unsigned char GetButtonState(THWParams* const pHWParams);

#ifdef __cplusplus
};
#endif
