//#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wiamicro.h>
#include <vector>
#include <cassert>
#include <usbscan.h>
#include "hpgt33_h.h"

int main(const int argc, const char* const argv[])
{
    HMODULE hMicroDriver = LoadLibrary(L"hpgtmcro.dll");
    HRESULT (__stdcall* pMicroEntry)(LONG lCommand, _Inout_ PVAL pValue);
    HRESULT(__stdcall* pScan)(_Inout_ PSCANINFO pScanInfo, LONG lPhase, _Out_writes_bytes_(lLength) PBYTE pBuffer, LONG lLength, _Out_ LONG *plReceived);
    HRESULT(__stdcall* pSetPixelWindow)(_Inout_ PSCANINFO pScanInfo, LONG x, LONG y, LONG xExtent, LONG yExtent);

    pMicroEntry = reinterpret_cast<decltype(pMicroEntry)>(GetProcAddress(hMicroDriver, "MicroEntry"));
    pScan = reinterpret_cast<decltype(pScan)>(GetProcAddress(hMicroDriver, "Scan"));
    pSetPixelWindow = reinterpret_cast<decltype(pSetPixelWindow)>(GetProcAddress(hMicroDriver, "SetPixelWindow"));

    HKEY hRegKey;
    //DeviceData\TulipCLSID value in this key must be {F20FB4D8-0C85-454B-AA3F-4F0E803A0E5D}
    //0000 assumes the 3300C is the first scanner.
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Class\\{6bdd1fc6-810f-11d0-bec7-08002be2092f}\\0000", 0, KEY_READ, &hRegKey) != ERROR_SUCCESS)
    {
        int a = 0; a++;
    }

    //Usbscan0 assumes the 3300C is the first scanner.
    const HANDLE hScanner = CreateFile(L"\\\\.\\Usbscan0", GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, NULL);

    SCANINFO ScanInfo = {};

    ScanInfo.DeviceIOHandles[0] = hScanner;

    VAL Val = {};
    Val.pScanInfo = &ScanInfo;
    Val.pHandle = reinterpret_cast<HANDLE*>(&hRegKey);

    HRESULT Result = 0;

    Result = pMicroEntry(CMD_SETSTIDEVICEHKEY, &Val);
    Result = pMicroEntry(CMD_INITIALIZE, &Val);

    const LONG iRes = 300;
    Val.lVal = 300;
    Result = pMicroEntry(CMD_SETXRESOLUTION, &Val);
    Result = pMicroEntry(CMD_SETYRESOLUTION, &Val);

    const LONG iSizeX = iRes*ScanInfo.BedWidth / 1000;
    const LONG iSizeY = iRes*ScanInfo.BedHeight / 1000;
    Result = pSetPixelWindow(&ScanInfo, 0, 0, iSizeX, iSizeY);

    LONG iReceived;
    std::vector<BYTE> Buffer;
    const size_t iBytesPerPixel = 3;
    const size_t iBufSize = iSizeX * iSizeY * iBytesPerPixel;
    const size_t iNumLinesPerCall = 8;
    const size_t iDataPerCall = iSizeX * iNumLinesPerCall * iBytesPerPixel;
    const size_t iNumCalls = (iSizeY + iNumLinesPerCall - 1) / iNumLinesPerCall;

    Buffer.resize(iBufSize);
    std::vector<BYTE> Buffer2;
    Buffer2.resize(iBufSize);

    size_t iDataLeft = iBufSize;
    for (size_t i = 0; i < iNumCalls; i++, iDataLeft -= iDataPerCall)
    {
        const size_t iSize = min(iDataLeft, iDataPerCall);
        pScan(&ScanInfo, i == 0 ? SCAN_FIRST : SCAN_NEXT, &Buffer[i*iDataPerCall], iSize, &iReceived);
    }
    pScan(&ScanInfo, SCAN_FINISHED, nullptr, 0, nullptr);


    Result = pScan(&ScanInfo, SCAN_FINISHED, Buffer.data(), iBufSize, &iReceived);

    Result = pMicroEntry(CMD_UNINITIALIZE, &Val);

    RegCloseKey(hRegKey);
    CloseHandle(hScanner);
    FreeLibrary(hMicroDriver);

    FILE* f;
    fopen_s(&f, "dump.raw", "wb");
    fwrite(Buffer.data(), Buffer.size(), 1, f);
    fclose(f);
}