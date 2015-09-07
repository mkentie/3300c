#pragma once
#include "IScanner.h"
#include "niash_core.h"

class C3300C: public IScanner
{
public:
    explicit C3300C();

private:
    typedef std::array<unsigned char, HW_PIXELS * 3 * 2> CalibTable_t;
    static const size_t GAMMA_TABLE_SIZE = 4096;
    typedef std::array<unsigned char, GAMMA_TABLE_SIZE> GammaTable_t;

    /**
    Performs lamp warm-up and fills calibration table.
    */
    bool WarmupLamp(CalibTable_t& CalibTable);

    /**
    Generates a unity gamma table.
    */
    void BuildUnityGammaTable(GammaTable_t& GammaTable);

    /**
    Generates a gamma table.
    */
    void BuildGammaTable(GammaTable_t& GammaTable, const float fGamma);

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

    BGR m_PrevWhiteCalib = {};

    std::vector<BYTE> m_BackBuffer; //Backbuffer for scanned data, which we need to descramble
    size_t m_iLinesSkipped = 0;


//From IScanner
public:
    virtual void GetCapabilities(VAL& Value) const override;
    virtual void FillSCANINFO(SCANINFO& ScanInfo) const override;
    virtual void GetStatus(VAL& Value) override;
    virtual void Reset(VAL& Value) override;
    virtual void Initialize(VAL& Value) override;
    virtual void Uninitialize() override;
    virtual void ScanStart(const SCANINFO& ScanInfo) override;
    virtual void ScanContinue(const SCANINFO& ScanInfo, BYTE* const pBuffer, const long lLength, long* const pReceived) override;
    virtual void ScanFinish(const SCANINFO& ScanInfo) override;
};