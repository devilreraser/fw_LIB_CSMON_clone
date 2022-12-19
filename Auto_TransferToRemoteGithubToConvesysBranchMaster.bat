@echo off

set branch=%1
if /I "%branch%"=="" set branch=master
set src_remote=%2
if /I "%src_remote%"=="" set src_remote=github
set dst_remote=%3
if /I "%dst_remote%"=="" set dst_remote=convesys
set org_remote=%4
if /I "%org_remote%"=="" set org_remote=origin

echo.
echo."%src_remote%" to "%dst_remote%" Automatic Transfer Whole Project With Submodules
echo.
SET /P _CommitString= Please enter Commit Message:
IF "%_CommitString%"=="" SET _CommitString=Merged %src_remote% to %dst_remote%
echo Commit Message: "%_CommitString%"
echo.
echo Process Submodules:

echo.
echo Process HexMonitor:
cd 02_Libraries\HexMonitor
git.exe fetch --all -v --progress
git.exe checkout -f -B %branch% remotes/%src_remote%/%branch% --
git pull --progress -v --no-rebase "%src_remote%" %branch%
git.exe checkout -f -B %branch% remotes/%dst_remote%/%branch% --
git pull --progress -v --no-rebase "%dst_remote%" %branch%
git merge --allow-unrelated-histories -Xrenormalize remotes/%src_remote%/%branch% < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git push %dst_remote% %branch%
git.exe checkout -f -B %branch% remotes/%org_remote%/%branch% --
git pull --progress -v --no-rebase "%org_remote%" %branch%
cd ..\..

echo.
echo Process MathAndControl:
cd 02_Libraries\MathAndControl
git.exe fetch --all -v --progress
git.exe checkout -f -B %branch% remotes/%src_remote%/%branch% --
git pull --progress -v --no-rebase "%src_remote%" %branch%
git.exe checkout -f -B %branch% remotes/%dst_remote%/%branch% --
git pull --progress -v --no-rebase "%dst_remote%" %branch%
git merge --allow-unrelated-histories -Xrenormalize remotes/%src_remote%/%branch% < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git push %dst_remote% %branch%
git.exe checkout -f -B %branch% remotes/%org_remote%/%branch% --
git pull --progress -v --no-rebase "%org_remote%" %branch%
cd ..\..

echo.
echo Process ModbusAddress:
cd 02_Libraries\ModbusAddress
git.exe fetch --all -v --progress
git.exe checkout -f -B %branch% remotes/%src_remote%/%branch% --
git pull --progress -v --no-rebase "%src_remote%" %branch%
git.exe checkout -f -B %branch% remotes/%dst_remote%/%branch% --
git pull --progress -v --no-rebase "%dst_remote%" %branch%
git merge --allow-unrelated-histories -Xrenormalize remotes/%src_remote%/%branch% < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git push %dst_remote% %branch%
git.exe checkout -f -B %branch% remotes/%org_remote%/%branch% --
git pull --progress -v --no-rebase "%org_remote%" %branch%
cd ..\..

echo.
echo Process Peripheral:
cd 02_Libraries\Peripheral
git.exe fetch --all -v --progress
git.exe checkout -f -B %branch% remotes/%src_remote%/%branch% --
git pull --progress -v --no-rebase "%src_remote%" %branch%
git.exe checkout -f -B %branch% remotes/%dst_remote%/%branch% --
git pull --progress -v --no-rebase "%dst_remote%" %branch%
git merge --allow-unrelated-histories -Xrenormalize remotes/%src_remote%/%branch% < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git push %dst_remote% %branch%
git.exe checkout -f -B %branch% remotes/%org_remote%/%branch% --
git pull --progress -v --no-rebase "%org_remote%" %branch%
cd ..\..

echo.
echo Process TestControl:
cd 02_Libraries\TestControl
git.exe fetch --all -v --progress
git.exe checkout -f -B %branch% remotes/%src_remote%/%branch% --
git pull --progress -v --no-rebase "%src_remote%" %branch%
git.exe checkout -f -B %branch% remotes/%dst_remote%/%branch% --
git pull --progress -v --no-rebase "%dst_remote%" %branch%
git merge --allow-unrelated-histories -Xrenormalize remotes/%src_remote%/%branch% < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git push %dst_remote% %branch%
git.exe checkout -f -B %branch% remotes/%org_remote%/%branch% --
git pull --progress -v --no-rebase "%org_remote%" %branch%
cd ..\..

echo.
echo Process Project:
::git lfs install
::git lfs uninstall
rem pause
git.exe fetch --all -v --progress
rem pause
git.exe checkout -f -B %branch% remotes/%src_remote%/%branch% --
rem pause
git pull --progress -v --no-rebase "%src_remote%" %branch%
rem pause
::git lfs install --skip-smudge
rem git lfs install
rem pause 
git.exe checkout -f -B %branch% remotes/%dst_remote%/%branch% --
rem pause
git pull --progress -v --no-rebase "%dst_remote%" %branch%
::git lfs pull
::git lfs install --force
rem pause
git merge --no-commit --allow-unrelated-histories -Xrenormalize remotes/%src_remote%/%branch% < ConvesysToGithub.in
IF %ERRORLEVEL% == 0 goto CommitProject
IF %ERRORLEVEL% NEQ 0 Echo Error = %ERRORLEVEL%
echo Printing Conflicts In PROJECT:
rem pause
FOR /F %%F IN ('git diff --name-only --diff-filter=U') DO echo "%%F"
echo Resolve Merge Conflicts In PROJECT With %src_remote% %branch% before Autoresolve
pause
rem @echo on
echo Auto Resolve Merge Conflicts In PROJECT With %src_remote% %branch%:
pause
FOR /F %%F IN ('git diff --name-only --diff-filter=U') DO git checkout %src_remote%/%branch% -- "%%F"
git status
pause
rem @echo off
echo Resolve Merge Conflicts In PROJECT With %src_remote% %branch% after Autoresolve
pause
:CommitProject
SET /P _CommitAck= Please enter "y" for Commit to %dst_remote%. Any other symbol skips commit:
IF "%_CommitAck%"=="y" goto Commit_dst_remote
goto EndCommit
:Commit_dst_remote
echo Before fix gitattributes and gitmodules for GithubToConvesys commit
::pause
git reset HEAD .gitattributes
git checkout remotes/%dst_remote%/%branch% -- .gitattributes
git reset HEAD .gitmodules
git checkout remotes/%dst_remote%/%branch% -- .gitmodules
echo After fix gitattributes and gitmodules for GithubToConvesys commit
::pause
call fix_convesys_modules.bat
call fix_convesys_attributes.bat
echo After forced fix gitattributes and gitmodules for GithubToConvesys commit
::pause

git commit -a -m "%_CommitString%"
rem pause
git push %dst_remote% %branch%
rem pause
:EndCommit
::git lfs uninstall
git.exe checkout -f -B %branch% remotes/%org_remote%/%branch% --
git pull --progress -v --no-rebase "%org_remote%" %branch%

echo Completed!
echo Press Enter to Exit
pause >nul
exit
