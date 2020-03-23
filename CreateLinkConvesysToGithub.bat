@echo off
set Origin1Path=https://github.com/devilreraser/
set Origin2Path=//192.168.1.102/repo_git/
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

git remote add convesys %Origin2Path%%ProjectName%
git remote set-url --push convesys %Origin1Path%%ProjectName%

echo.
echo "Info: Delete Submodule Folder if Create Submodule Fails!!!"
echo.
git submodule add  --force -- "%Origin1Path%%Module1Name%" "%Module1Path%"
cd %Module1Path%
git remote add convesys %Origin2Path%%Module1Name%
git remote set-url --push convesys %Origin1Path%%Module1Name%
cd ..\..

echo.
echo "Info: Delete Submodule Folder if Create Submodule Fails!!!"
echo.
git submodule add  --force -- "%Origin1Path%%Module2Name%" "%Module2Path%"
cd %Module2Path%
git remote add convesys %Origin2Path%%Module2Name%
git remote set-url --push convesys %Origin1Path%%Module2Name%
cd ..\..

echo.
echo "Info: Delete Submodule Folder if Create Submodule Fails!!!"
echo.
git submodule add  --force -- "%Origin1Path%%Module3Name%" "%Module3Path%"
cd %Module3Path%
git remote add convesys %Origin2Path%%Module3Name%
git remote set-url --push convesys %Origin1Path%%Module3Name%
cd ..\..

echo.
echo "Info: Delete Submodule Folder if Create Submodule Fails!!!"
echo.
git submodule add  --force -- "%Origin1Path%%Module4Name%" "%Module4Path%"
cd %Module4Path%
git remote add convesys %Origin2Path%%Module4Name%
git remote set-url --push convesys %Origin1Path%%Module4Name%
cd ..\..

echo.
echo "Info: Delete Submodule Folder if Create Submodule Fails!!!"
echo.
git submodule add  --force -- "%Origin1Path%%Module5Name%" "%Module5Path%"
cd %Module5Path%
git remote add convesys %Origin2Path%%Module5Name%
git remote set-url --push convesys %Origin1Path%%Module5Name%
cd ..\..

pause