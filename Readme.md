#3300C

This project is a microdriver for the HP ScanJet 3300C scanner. It works with Windows 7 64-bit edition and later.

##Installation

Windows 8 and later:

Double click certificate.
Select install.
Select 'Local Machine'.
Select 'Place all certificates in the following store' and browse to 'Trusted Root Certification Authorities'

Can remove certificate after installing driver.


##Known issues
* Although they don't do anything, the brightness/contrast sliders in the Windows scan wizard aren't disabled, instead they show a range from -100 to 100.
* Greyscale and black/white scan modes don't aren't supported, yet the Windows scan wizard allows those options to be picked.
* Scans made using this driver are darker than those made with the original HP XP driver. Comparing USB data dumps, and writing a wrapper for the XP driver, leads me to conclude that the XP driver adjusts the image gamma in software, post-scan.
* The right and bottom edges of the scanned material are cropped off; also happens with the XP driver.
* Scanner backtracks every now and then at 300 DPI; also happens with the XP driver.
* 100 and 200 DPI settings aren't supported.

##Development
This driver is based on the SANE backend for the 3300C and related scanners. Bertrik Sikken, the original author of the backend, thankfully had made available documentation on how the scanner works.
This documentation, combined with the backend's source code (which included a handy test tool) allowed me to relatively easily port the code over to Windows.
The backend's source code was nicely structured:

1. Application interface (e.g. talking to SANE).
1. Scanner protocol (detection, read lines, write tables, etc.)
1. Parallel port level protocol (the scanner uses a USB<->parallel converter chip internally)
1. USB backend (talks to the OS)

By basically switching out components 1 and 4 for the Windows microdriver interface and a backend that talks to the Windows UsbScan driver (and many hours of problem solving), the driver was made to work on Windows.

##Source
The main driver source code is located in the [MicroDriver] directory. It contains a batch file to sign the driver, and one to (re)install it 'nicely'. It also contains a batch file that stops the still image service, copies the dll over an existing old one, and restarts the service. This worked just fine during development.
To build the microdriver you'll need the Windows WDK. To sign the driver you'll need to generate a new certificate and private key, see [text file](How I made the cert.txt).
The 'Misc' directory contains a file 'wiahost.reg'. This contains a registry change that'll make the still image server run as the executable 'wiahost.exe' instead of the default 'svchost.exe'. This makes it easier to attach a debugger. You need to create a copy of svchost.exe and rename it to wiahost.exe for this to work. See https://msdn.microsoft.com/en-us/library/windows/hardware/ff552727%28v=vs.85%29.aspx .

There are some additional tools/drivers:

* [SaneTest](SaneTest), the test tool from the SANE backend code, modified to compile under Windows. This is what I first used to get the scanner working. It supports two backends (switch by compiling different cpp/h files):
 * [niash_winusb](SaneTest/SaneTest/niash_winusb.cpp) This is for the [WinUSB driver](WinUSB Driver). The device will appear as a generic USB device in the Device Manager. This driver/backend was my first attempt at establishing communications with the scanner (well, after I managed to control the lamp from another test app); WinUSB is Microsoft's generic user-mode USB driver.
 * [niash_usbscan](SaneTest/SaneTest/niash_usbscan.cpp) This is for the basic [UsbScan.sys based driver](UsbScan Only Driver). This installs the device as a scanner, but only enables it to be accessed by user-mode code, nothing else. The code for this backend is basically the same as that used by the microdriver.
* [Wrapper](Wrapper). This wrapper, for use in conjunction with the [UsbScan.sys based driver](UsbScan Only Driver), calls into the original HP Windows XP microdriver. I used this to compare behavior, such as the scan brightness issue. I've also used this to debug the new microdriver when it was generating access violations on Windows 7.
 * Run regsvr32 on the [XP Driver](Misc/XP Driver)'s file hpgt33.dll.
 * Under the registry key "SYSTEM\\CurrentControlSet\\Control\\Class\\{6bdd1fc6-810f-11d0-bec7-08002be2092f}\\0000\\DeviceData" ('0000' assumes the 3300C is the first scanner) set the value "TulipCLSID" to "{F20FB4D8-0C85-454B-AA3F-4F0E803A0E5D}".