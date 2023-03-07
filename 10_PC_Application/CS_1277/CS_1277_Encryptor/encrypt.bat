@echo off
set currdir=%~dp0
set fileinp=%1
set fileout=%2
set encryptmode=%3

echo Current Directory  : %currdir%
echo Input File         : %fileinp%
echo Output File        : %fileout%
echo Encrypt Mode       : %encryptmode%


if "%fileinp%" == "" set fileinp=uw_blink_2806x.txt
:: if "%fileinp%" == "" set fileinp=uw_blink.txt
if "%fileout%" == "" set fileout=uw_blink.btl
if "%encryptmode%" == "" set encryptmode=1

set encryptpass="nice_password123"

if "%encryptmode%" == "2" set encryptpass="nice_password123456789ABCDEFGHIJ"

echo %currdir%CS_1277_Encryptor -i %fileinp% -o %fileout% -p %encryptpass% -d -t %encryptmode% --iv "alabalanicalater"

%currdir%CS_1277_Encryptor -i %fileinp% -o %fileout% -p %encryptpass% -d -t %encryptmode% --iv "alabalanicalater"


:: encrypt uw_blink_2806x.txt uw_blink.btl 0
:: encrypt uw_blink_2806x.txt uw_blink.btl 1
:: encrypt uw_blink_2806x.txt uw_blink.btl 2
:: encrypt uw_blink.txt uw_blink.btl 0
:: encrypt uw_blink.txt uw_blink.btl 1

:: "..\..\..\10_PC_Application\CS_1277\CS_1277_Encryptor\CS_1277_Encryptor" -i "..\..\..\05_Projects\${ProjName}\${ConfigName}\${ProjName}.txt" -o "..\..\..\05_Projects\${ProjName}\${ConfigName}\${ProjName}.btl" -p "nice_password123456789ABCDEFGHIJ" -d -t 2 --iv "alabalanicalater"
:: ..\..\..\10_PC_Application\CS_1277\CS_1277_Encryptor\encrypt.bat ..\..\..\05_Projects\${ProjName}\${ConfigName}\${ProjName}.txt ..\..\..\05_Projects\${ProjName}\${ConfigName}\${ProjName}.btl 2

