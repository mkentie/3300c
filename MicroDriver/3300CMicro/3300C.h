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
#include "IScanner.h"
#include "niash_core.h"

class C3300C: public IScanner
{
public:
    explicit C3300C();
    virtual ~C3300C();

private:
    typedef std::array<unsigned char, HW_PIXELS * 3 * 2> CalibTable_t;
    static const size_t scm_iGammaTableSize = 4096;
    typedef std::array<unsigned char, scm_iGammaTableSize> GammaTable_t;

    /**
    Performs lamp warm-up and fills calibration table.
    */
    bool WarmupLamp(CalibTable_t& CalibTable);

    enum EScannerButtonBits //Note: button status is reset by scanner after being queried
    {
        ButtonScan = 0x04,
        ButtonPrint = 0x10
    };
    static std::array<GUID,2> sm_ButtonGUIDs;
    static std::array<wchar_t*,2> sm_pszButtonNames;

    THWParams m_HWParams = {}; //Hardware params filled by driver
    bool m_bStatusOk = true;

    union BGR
    {
        struct
        {
            BYTE b;
            BYTE g;
            BYTE r;
        } components;
        BYTE bgr[3];

        constexpr static size_t size() { return sizeof(BGR); }
    };

    BGR m_PrevWhiteCalib = {}; //Save calibration value so it can be reused

    std::vector<BYTE> m_BackBuffer; //Backbuffer for scanned data, which we need to descramble
    size_t m_iLinesLeftToSkip = 0; //Need to throw away garbage lines that always get returned


//From IScanner
public:
    virtual void GetCapabilities(VAL& Value) const override;
    virtual void FillSCANINFO(SCANINFO& ScanInfo) const override;
    virtual void GetStatus(VAL& Value) override;
    virtual void Reset(VAL& Value) override;
    virtual bool Initialize(VAL& Value) override;
    virtual bool ScanStart(const SCANINFO& ScanInfo) override;
    virtual bool ScanContinue(const SCANINFO& ScanInfo, BYTE* const pBuffer, const long lLength, long* const pReceived) override;
    virtual bool ScanFinish(const SCANINFO& ScanInfo) override;
};