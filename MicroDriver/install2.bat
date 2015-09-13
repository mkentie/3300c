:bla
net stop stisvc
copy "%~dp0\x64\Debug\3300cMicro\3300cMicro.dll" "%windir%\system32"
net start stisvc
pause
goto bla