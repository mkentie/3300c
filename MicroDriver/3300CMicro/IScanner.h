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

class IScanner
{
public:
    virtual ~IScanner() {};

    virtual void GetCapabilities(VAL& Value) const = 0;
    virtual void FillSCANINFO(SCANINFO& ScanInfo) const = 0;
    virtual void GetStatus(VAL& Value) = 0;
    virtual void Reset(VAL& Value) = 0;
    virtual bool Initialize(VAL& Value) = 0;
    virtual bool ScanStart(const SCANINFO& ScanInfo) = 0;
    virtual bool ScanContinue(const SCANINFO& ScanInfo, BYTE* const pBuffer, const long lLength, long* const pReceived) = 0;
    virtual bool ScanFinish(const SCANINFO& ScanInfo) = 0;
};