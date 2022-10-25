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
git pull --progress -v --no-rebase "convesys" master
git.exe checkout -f -B master remotes/github/master --
git pull --progress -v --no-rebase "github" master
git merge --allow-unrelated-histories -Xrenormalize remotes/convesys/master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git push github master
git.exe checkout -f -B master remotes/origin/master --
git pull --progress -v --no-rebase "origin" master
cd ..\..

echo.
echo Process MathAndControl:
cd 02_Libraries\MathAndControl
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/convesys/master --
git pull --progress -v --no-rebase "convesys" master
git.exe checkout -f -B master remotes/github/master --
git pull --progress -v --no-rebase "github" master
git merge --allow-unrelated-histories -Xrenormalize remotes/convesys/master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git push github master
git.exe checkout -f -B master remotes/origin/master --
git pull --progress -v --no-rebase "origin" master
cd ..\..

echo.
echo Process ModbusAddress:
cd 02_Libraries\ModbusAddress
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/convesys/master --
git pull --progress -v --no-rebase "convesys" master
git.exe checkout -f -B master remotes/github/master --
git pull --progress -v --no-rebase "github" master
git merge --allow-unrelated-histories -Xrenormalize remotes/convesys/master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git push github master
git.exe checkout -f -B master remotes/origin/master --
git pull --progress -v --no-rebase "origin" master
cd ..\..

echo.
echo Process Peripheral:
cd 02_Libraries\Peripheral
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/convesys/master --
git pull --progress -v --no-rebase "convesys" master
git.exe checkout -f -B master remotes/github/master --
git pull --progress -v --no-rebase "github" master
git merge --allow-unrelated-histories -Xrenormalize remotes/convesys/master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git push github master
git.exe checkout -f -B master remotes/origin/master --
git pull --progress -v --no-rebase "origin" master
cd ..\..

echo.
echo Process TestControl:
cd 02_Libraries\TestControl
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/convesys/master --
git pull --progress -v --no-rebase "convesys" master
git.exe checkout -f -B master remotes/github/master --
git pull --progress -v --no-rebase "github" master
git merge --allow-unrelated-histories -Xrenormalize remotes/convesys/master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git push github master
git.exe checkout -f -B master remotes/origin/master --
git pull --progress -v --no-rebase "origin" master
cd ..\..

echo.
echo Process Project:
::git lfs install
::git lfs uninstall
rem pause
git.exe fetch --all -v --progress
rem pause
git.exe checkout -f -B master remotes/convesys/master --
rem pause
git pull --progress -v --no-rebase "convesys" master
rem pause
::git lfs install --skip-smudge
rem git lfs install
rem pause 
git.exe checkout -f -B master remotes/github/master --
rem pause
git pull --progress -v --no-rebase "github" master
::git lfs pull
::git lfs install --force
rem pause
git merge --no-commit --allow-unrelated-histories -Xrenormalize remotes/convesys/master < ConvesysToGithub.in
IF %ERRORLEVEL% == 0 goto CommitProject
IF %ERRORLEVEL% NEQ 0 Echo Error = %ERRORLEVEL%
echo Printing Conflicts In PROJECT:
rem pause
FOR /F %%F IN ('git diff --name-only --diff-filter=U') DO echo "%%F"
echo Resolve Merge Conflicts In PROJECT With Convesys Master before Autoresolve
pause
rem @echo on
echo Auto Resolve Merge Conflicts In PROJECT With Convesys Master:
pause
FOR /F %%F IN ('git diff --name-only --diff-filter=U') DO git checkout convesys/master -- "%%F"
git status
pause
rem @echo off
echo Resolve Merge Conflicts In PROJECT With Convesys Master after Autoresolve
pause
:CommitProject
SET /P _CommitAck= Please enter "y" for Commit to GitHub. Any other symbol skips commit:
IF "%_CommitAck%"=="y" goto CommitGitHub
goto EndCommit
:CommitGitHub
rem pause
git reset HEAD .gitattributes
git checkout remotes/github/master -- .gitattributes
git reset HEAD .gitmodules
git checkout remotes/github/master -- .gitmodules
rem pause
git commit -a -m "%_CommitString%"
rem pause
git push github master
rem pause
:EndCommit
::git lfs uninstall
git.exe checkout -f -B master remotes/origin/master --
git pull --progress -v --no-rebase "origin" master

echo Completed!
echo Press Enter to Exit
pause >nul
exit
