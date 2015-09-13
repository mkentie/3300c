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

#pragma once

#ifdef _DEBUG
#define DBGMSG(...) \
{ \
    wchar_t szBuf[2048]; \
    swprintf_s(szBuf, TEXT(__FUNCTION__) L": " __VA_ARGS__); \
    OutputDebugString(szBuf); \
    OutputDebugString(L"\n"); \
}
#else
#define DBGMSG(...)
#endif

namespace Helpers
{
    template <class T>
    constexpr float PixelsToDots(const T& Pixels, const size_t CurrentDPI, const size_t NativeDPI)
    {
        return Pixels * NativeDPI / static_cast<float>(CurrentDPI);
    }
}
