@echo off
set currdir=%~dp0
set projname=%1
set confname=%2
set encryptmode=%3

set fileinp="..\..\..\04_Output_Files\%confname%\%projname%.txt"
set fileout="..\..\..\04_Output_Files\%confname%\%projname%.btl"
:: set fileinp="%projname%.txt"
:: set fileout="%projname%.btl"

echo Current Directory  : %currdir%
echo Input File         : %fileinp%
echo Output File        : %fileout%
echo Encrypt Mode       : %encryptmode%

cd %currdir%

set path=%path%;%currdir%

::echo %path%

if "%fileinp%" == "" set fileinp=uw_blink_2806x.txt
:: if "%fileinp%" == "" set fileinp=uw_blink.txt
if "%fileout%" == "" set fileout=uw_blink.btl
if "%encryptmode%" == "" set encryptmode=1

set encryptpass="nice_password123"

if "%encryptmode%" == "2" set encryptpass="nice_password123456789ABCDEFGHIJ"

::echo CS_1277_Encryptor -i %fileinp% -o %fileout% -p %encryptpass% -d -t %encryptmode% --iv "alabalanicalater"
::dir
::start "123" /d "C:\repo_git\fw_TMS320_F28xx_Bootloader\10_PC_Application\CS_1277\CS_1277_Encryptor\" CS_1277_Encryptor -i %fileinp% -o %fileout% -p %encryptpass% -d -t %encryptmode% --iv "alabalanicalater" > log.txt
::CS_1277_Encryptor --input %fileinp% --output %fileout% --pass %encryptpass% --delete --type %encryptmode% --iv "alabalanicalater" --silent
CS_1277_Encryptor -i %fileinp% -o %fileout% -p %encryptpass% -d -t %encryptmode% --iv "alabalanicalater" --silent
::cmd
::start "CS_1277_Encryptor.exe --help"



:: encrypt uw_blink_2806x.txt uw_blink.btl 0
:: encrypt uw_blink_2806x.txt uw_blink.btl 1
:: encrypt uw_blink_2806x.txt uw_blink.btl 2
:: encrypt uw_blink.txt uw_blink.btl 0
:: encrypt uw_blink.txt uw_blink.btl 1

:: "..\..\..\10_PC_Application\CS_1277\CS_1277_Encryptor\CS_1277_Encryptor" -i "..\..\..\05_Projects\${ProjName}\${ConfigName}\${ProjName}.txt" -o "..\..\..\05_Projects\${ProjName}\${ConfigName}\${ProjName}.btl" -p "nice_password123456789ABCDEFGHIJ" -d -t 2 --iv "alabalanicalater"
:: ..\..\..\10_PC_Application\CS_1277\CS_1277_Encryptor\encrypt.bat ..\..\..\05_Projects\${ProjName}\${ConfigName}\${ProjName}.txt ..\..\..\05_Projects\${ProjName}\${ConfigName}\${ProjName}.btl 2
:: ..\..\..\10_PC_Application\CS_1277\CS_1277_Encryptor\encrypt_ccs.bat ${ProjName} ${ConfigName} 2

