@echo off
echo. "convesys" to "github" automatic transfer
SET /P _CommitString= Please enter Commit Message:
IF "%_CommitString%"=="" SET _CommitString=Merged Convesys
echo Commit Message: "%_CommitString%"
echo.
echo Process Submodules:
git lfs uninstall
echo.
echo Process HexMonitor:
cd 02_Libraries\HexMonitor
git.exe fetch --all -v --progress
git checkout -B master --track convesys/master
git checkout remotes/github/master
git merge --allow-unrelated-histories master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git switch -C master
git push github master
git.exe fetch --all -v --progress
git checkout -B master --track origin/master
cd ..\..
echo.
echo Process MathAndControl:
cd 02_Libraries\MathAndControl
git.exe fetch --all -v --progress
git checkout -B master --track convesys/master
git checkout remotes/github/master
git merge --allow-unrelated-histories master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git switch -C master
git push github master
git.exe fetch --all -v --progress
git checkout -B master --track origin/master
cd ..\..
echo.
echo Process ModbusAddress:
cd 02_Libraries\ModbusAddress
git.exe fetch --all -v --progress
git checkout -B master --track convesys/master
git checkout remotes/github/master
git merge --allow-unrelated-histories master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git switch -C master
git push github master
git.exe fetch --all -v --progress
git checkout -B master --track origin/master
cd ..\..
echo.
echo Process Peripheral:
cd 02_Libraries\Peripheral
git.exe fetch --all -v --progress
git checkout -B master --track convesys/master
git checkout remotes/github/master
git merge --allow-unrelated-histories master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git switch -C master
git push github master
git.exe fetch --all -v --progress
git checkout -B master --track origin/master
cd ..\..
echo.
echo Process TestControl:
cd 02_Libraries\TestControl
git.exe fetch --all -v --progress
git checkout -B master --track convesys/master
git checkout remotes/github/master
git merge --allow-unrelated-histories master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git switch -C master
git push github master
git.exe fetch --all -v --progress
git checkout -B master --track origin/master
cd ..\..
echo.
echo Process Project:
git.exe fetch --all -v --progress
git checkout -B master --track convesys/master
git lfs install
git checkout remotes/github/master
git pull github master
git merge --no-commit --allow-unrelated-histories master < ConvesysToGithub.in
git reset HEAD .gitattributes
git checkout remotes/github/master -- .gitattributes
git reset HEAD .gitmodules
git checkout remotes/github/master -- .gitmodules
git commit -a -m "%_CommitString%"
git switch -C master
git push github master
git.exe fetch --all -v --progress
git lfs uninstall
git checkout -B master --track origin/master
echo Completed!
echo Press Enter to Exit
pause >nul
exit