@echo off
set BranchString=master
set RemoteString=origin
set RemotePath=https://github.com/devilreraser/
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
set RetRootPath=..\..

git.exe checkout -f -B %BranchString% remotes/%RemoteString%/%BranchString% --
echo. Checkout '%RemoteString%/%BranchString%' path '%RemotePath%%ProjectName%'
cd %Module1Path%
git.exe checkout -f -B %BranchString% remotes/%RemoteString%/%BranchString% --
echo. Checkout '%RemoteString%/%BranchString%' path '%RemotePath%%Module1Name%'
cd %RetRootPath%
cd %Module2Path%
git.exe checkout -f -B %BranchString% remotes/%RemoteString%/%BranchString% --
echo. Checkout '%RemoteString%/%BranchString%' path '%RemotePath%%Module2Name%'
cd %RetRootPath%
cd %Module3Path%
git.exe checkout -f -B %BranchString% remotes/%RemoteString%/%BranchString% --
echo. Checkout '%RemoteString%/%BranchString%' path '%RemotePath%%Module3Name%'
cd %RetRootPath%
cd %Module4Path%
git.exe checkout -f -B %BranchString% remotes/%RemoteString%/%BranchString% --
echo. Checkout '%RemoteString%/%BranchString%' path '%RemotePath%%Module4Name%'
cd %RetRootPath%
cd %Module5Path%
git.exe checkout -f -B %BranchString% remotes/%RemoteString%/%BranchString% --
echo. Checkout '%RemoteString%/%BranchString%' path '%RemotePath%%Module5Name%'
cd %RetRootPath%
pause