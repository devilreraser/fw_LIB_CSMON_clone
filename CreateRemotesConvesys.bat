@echo off
set RemoteString=convesys
set RemotePath=//192.168.1.102/repo_git/
set ProjectName=fw_LIB_CSMON.git
set Module1Name=fw_LIB_HexMonitor.git
set Module1Path=02_Libraries/HexMonitor
set Module2Name=fw_LIB_MathAndControl.git
set Module2Path=02_Libraries/MathAndControl
set Module3Name=uw_LIB_ModbusAddress.git
set Module3Path=02_Libraries/ModbusAddress
set Module4Name=fw_LIB_Peripheral.git
set Module4Path=02_Libraries/Peripheral
set Module5Name=fw_LIB_TestControl.git
set Module5Path=02_Libraries/TestControl

git remote remove %RemoteString% >nul 2>nul
if %errorlevel%==0 echo. Old Remote With Same Name Deleted!
echo. Added '%RemoteString%' path '%RemotePath%%ProjectName%'
git remote add %RemoteString% %RemotePath%%ProjectName%
cd %Module1Path%
git remote remove %RemoteString% >nul 2>nul
if %errorlevel%==0 echo. Old Remote With Same Name Deleted!
echo. Added '%RemoteString%' path '%RemotePath%%Module1Name%'
git remote add %RemoteString% %RemotePath%%Module1Name%
cd ..\..
cd %Module2Path%
git remote remove %RemoteString% >nul 2>nul
if %errorlevel%==0 echo. Old Remote With Same Name Deleted!
echo. Added '%RemoteString%' path '%RemotePath%%Module2Name%'
git remote add %RemoteString% %RemotePath%%Module2Name%
cd ..\..
cd %Module3Path%
git remote remove %RemoteString% >nul 2>nul
if %errorlevel%==0 echo. Old Remote With Same Name Deleted!
echo. Added '%RemoteString%' path '%RemotePath%%Module3Name%'
git remote add %RemoteString% %RemotePath%%Module3Name%
cd ..\..
cd %Module4Path%
git remote remove %RemoteString% >nul 2>nul
if %errorlevel%==0 echo. Old Remote With Same Name Deleted!
echo. Added '%RemoteString%' path '%RemotePath%%Module4Name%'
git remote add %RemoteString% %RemotePath%%Module4Name%
cd ..\..
cd %Module5Path%
git remote remove %RemoteString% >nul 2>nul
if %errorlevel%==0 echo. Old Remote With Same Name Deleted!
echo. Added '%RemoteString%' path '%RemotePath%%Module5Name%'
git remote add %RemoteString% %RemotePath%%Module5Name%
cd ..\..
pause