:bla
stampinf -f .\winusbinstallation.inf -v *
inf2cat.exe /os:7_x64 /driver:.
signtool sign /f "../3300CMicro.cer" winusbinstallation.cat
pause
goto bla