:bla
stampinf -f 3300c.inf -v *
inf2cat.exe /os:7_x64 /driver:.
signtool sign /f "../3300CMicro.cer" 3300c.cat
pause
goto bla