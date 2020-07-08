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
git.exe checkout -f -B master remotes/convesys/master --
git pull --progress -v --no-rebase "convesys"
git.exe checkout -f -B master remotes/github/master --
git pull --progress -v --no-rebase "github"
git merge --allow-unrelated-histories -Xrenormalize remotes/convesys/master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git push github master
git.exe checkout -f -B master origin/master --
git pull --progress -v --no-rebase "origin"
cd ..\..

echo.
echo Process MathAndControl:
cd 02_Libraries\MathAndControl
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/convesys/master --
git pull --progress -v --no-rebase "convesys"
git.exe checkout -f -B master remotes/github/master --
git pull --progress -v --no-rebase "github"
git merge --allow-unrelated-histories -Xrenormalize remotes/convesys/master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git push github master
git.exe checkout -f -B master origin/master --
git pull --progress -v --no-rebase "origin"
cd ..\..

echo.
echo Process ModbusAddress:
cd 02_Libraries\ModbusAddress
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/convesys/master --
git pull --progress -v --no-rebase "convesys"
git.exe checkout -f -B master remotes/github/master --
git pull --progress -v --no-rebase "github"
git merge --allow-unrelated-histories -Xrenormalize remotes/convesys/master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git push github master
git.exe checkout -f -B master origin/master --
git pull --progress -v --no-rebase "origin"
cd ..\..

echo.
echo Process Peripheral:
cd 02_Libraries\Peripheral
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/convesys/master --
git pull --progress -v --no-rebase "convesys"
git.exe checkout -f -B master remotes/github/master --
git pull --progress -v --no-rebase "github"
git merge --allow-unrelated-histories -Xrenormalize remotes/convesys/master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git push github master
git.exe checkout -f -B master origin/master --
git pull --progress -v --no-rebase "origin"
cd ..\..

echo.
echo Process TestControl:
cd 02_Libraries\TestControl
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/convesys/master --
git pull --progress -v --no-rebase "convesys"
git.exe checkout -f -B master remotes/github/master --
git pull --progress -v --no-rebase "github"
git merge --allow-unrelated-histories -Xrenormalize remotes/convesys/master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git push github master
git.exe checkout -f -B master origin/master --
git pull --progress -v --no-rebase "origin"
cd ..\..

echo.
echo Process Project:
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/convesys/master --
git pull --progress -v --no-rebase "convesys"
git.exe checkout -f -B master remotes/github/master --
git pull --progress -v --no-rebase "github"
git merge --no-commit --allow-unrelated-histories -Xrenormalize remotes/convesys/master < ConvesysToGithub.in
IF %ERRORLEVEL% == 0 goto CommitProject
IF %ERRORLEVEL% NEQ 0 Echo Error = %ERRORLEVEL%
echo Auto Resolve Merge Conflicts In PROJECT With Convesys Master:
pause >nul
FOR /F %%F IN ('git diff --name-only --diff-filter=U') DO echo "%%F"
@echo on
pause >nul
FOR /F %%F IN ('git diff --name-only --diff-filter=U') DO git checkout convesys/master -- "%%F"
git status
pause >nul
@echo off
:CommitProject
git reset HEAD .gitattributes
git checkout remotes/github/master -- .gitattributes
git reset HEAD .gitmodules
git checkout remotes/github/master -- .gitmodules
git commit -a -m "%_CommitString%"
git push github master
git.exe checkout -f -B master origin/master --
git pull --progress -v --no-rebase "origin"

echo Completed!
echo Press Enter to Exit
pause >nul
exit
