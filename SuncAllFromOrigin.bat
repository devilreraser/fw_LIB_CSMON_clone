@echo off
rem start "" cmd /c "echo Hello world!&echo(&pause"
echo.
echo Process Submodules:
echo.
echo HexMonitor:
echo.
cd 02_Libraries\HexMonitor
git fetch --all
git pull
cd ..\..
echo.
echo MathAndControl:
echo.
cd 02_Libraries\MathAndControl
git fetch --all
git pull
cd ..\..
echo.
echo ModbusAddress:
echo.
cd 02_Libraries\ModbusAddress
git fetch --all
git pull
cd ..\..
echo.
echo Peripheral:
echo.
cd 02_Libraries\Peripheral
git fetch --all
git pull
cd ..\..
echo.
echo Process Project:
echo.
git fetch --all
git pull
echo.
echo Completed!
echo Press Enter to Exit
pause >nul
exit