@echo off
rem start "" cmd /c "echo Hello world!&echo(&pause"
echo.
echo Process Submodules:
echo.
echo HexMonitor:
echo.
cd 02_Libraries\HexMonitor
git.exe fetch -v --progress "origin"
git.exe checkout -f -B master origin/master --
git.exe pull --progress -v --no-rebase "origin"
cd ..\..
echo.
echo MathAndControl:
echo.
cd 02_Libraries\MathAndControl
git.exe fetch -v --progress "origin"
git.exe checkout -f -B master origin/master --
git.exe pull --progress -v --no-rebase "origin"
cd ..\..
echo.
echo ModbusAddress:
echo.
cd 02_Libraries\ModbusAddress
git.exe fetch -v --progress "origin"
git.exe checkout -f -B master origin/master --
git.exe pull --progress -v --no-rebase "origin"
cd ..\..
echo.
echo Peripheral:
echo.
cd 02_Libraries\Peripheral
git.exe fetch -v --progress "origin"
git.exe checkout -f -B master origin/master --
git.exe pull --progress -v --no-rebase "origin"
cd ..\..
echo.
echo TestControl:
echo.
cd 02_Libraries\TestControl
git.exe fetch -v --progress "origin"
git.exe checkout -f -B master origin/master --
git.exe pull --progress -v --no-rebase "origin"
cd ..\..
echo.
echo Process Project:
echo.
git.exe fetch -v --progress "origin"
git.exe checkout -f -B master origin/master --
git.exe pull --progress -v --no-rebase "origin"
echo.
echo Completed!
echo Press Enter to Exit
pause >nul
exit