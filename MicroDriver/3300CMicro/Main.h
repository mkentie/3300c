#pragma once

#define DBGMSG(...) \
{ \
    TCHAR szBuf[2048]; \
    _stprintf_s(szBuf, _T(__FUNCTION__) _T(": ") __VA_ARGS__); \
    OutputDebugString(szBuf); \
    OutputDebugString(_T("\n")); \
}



