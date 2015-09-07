#pragma once

class IScanner
{
public:
	virtual void GetCapabilities(VAL& Value) const = 0;
	virtual void FillSCANINFO(SCANINFO& ScanInfo) const = 0;
	virtual void GetStatus(VAL& Value) = 0;
	virtual void Reset(VAL& Value) = 0;
	virtual void Initialize(VAL& Value) = 0;
	virtual void Uninitialize() = 0;
	virtual void ScanStart(const SCANINFO& ScanInfo) = 0;
	virtual void ScanContinue(const SCANINFO& ScanInfo, BYTE* const pBuffer, const long lLength, long* const pReceived) = 0;
	virtual void ScanFinish(const SCANINFO& ScanInfo) = 0;
};