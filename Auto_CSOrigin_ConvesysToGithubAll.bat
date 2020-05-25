@echo off
echo.
echo.CONVESYS ORIGIN Used!
echo.
echo."Convesys" to "Github" Automatic Transfer Whole Project With Submodules
echo.
SET /P _CommitString= Please enter Commit Message:
IF "%_CommitString%"=="" SET _CommitString=Merged Convesys
echo Commit Message: "%_CommitString%"
echo.
echo Process Submodules:

echo.
echo Process HexMonitor:
cd 02_Libraries\HexMonitor
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/github/master --
git merge --allow-unrelated-histories -s recursive -Xtheirs remotes/convesys/master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git push github master
git.exe checkout -f -B master origin/master --
cd ..\..

echo.
echo Process MathAndControl:
cd 02_Libraries\MathAndControl
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/github/master --
git merge --allow-unrelated-histories -s recursive -Xtheirs remotes/convesys/master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git push github master
git.exe checkout -f -B master origin/master --
cd ..\..

echo.
echo Process ModbusAddress:
cd 02_Libraries\ModbusAddress
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/github/master --
git merge --allow-unrelated-histories -s recursive -Xtheirs remotes/convesys/master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git push github master
git.exe checkout -f -B master origin/master --
cd ..\..

echo.
echo Process Peripheral:
cd 02_Libraries\Peripheral
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/github/master --
git merge --allow-unrelated-histories -s recursive -Xtheirs remotes/convesys/master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git push github master
git.exe checkout -f -B master origin/master --
cd ..\..

echo.
echo Process TestControl:
cd 02_Libraries\TestControl
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/github/master --
git merge --allow-unrelated-histories -s recursive -Xtheirs remotes/convesys/master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git push github master
git.exe checkout -f -B master origin/master --
cd ..\..

echo.
echo Process Project:
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/github/master --
git merge --no-commit --allow-unrelated-histories -s recursive -Xtheirs remotes/convesys/master < ConvesysToGithub.in
git reset HEAD .gitattributes
git checkout remotes/github/master -- .gitattributes
git reset HEAD .gitmodules
git checkout remotes/github/master -- .gitmodules
git commit -a -m "%_CommitString%"
git push github master
git.exe checkout -f -B master origin/master --

echo Completed!
echo Press Enter to Exit
pause >nul
exit
