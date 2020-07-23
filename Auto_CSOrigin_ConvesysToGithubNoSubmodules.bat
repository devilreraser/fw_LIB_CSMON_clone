@echo off
echo.
echo.CONVESYS ORIGIN Used!
echo.
echo."Convesys" to "Github" Automatic Transfer Without Submodules
echo.
SET /P _CommitString= Please enter Commit Message:
IF "%_CommitString%"=="" SET _CommitString=Merged Convesys
echo Commit Message: "%_CommitString%"

echo.
echo Process Project:
git lfs install
git lfs uninstall
rem pause
git.exe fetch --all -v --progress
rem pause
git.exe checkout -f -B master remotes/convesys/master --
rem pause
git pull --progress -v --no-rebase "convesys" master
rem pause
git lfs install --skip-smudge
rem git lfs install
rem pause 
git.exe checkout -f -B master remotes/github/master --
rem pause
git pull --progress -v --no-rebase "github" master
git lfs pull
git lfs install --force
rem pause
git merge --no-commit --allow-unrelated-histories -Xrenormalize remotes/convesys/master < ConvesysToGithub.in
echo Resolve Merge Conflicts In PROJECT With Convesys Master before Autoresolve
pause
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
echo Resolve Merge Conflicts In PROJECT With Convesys Master after Autoresolve
pause
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
git lfs uninstall
git.exe checkout -f -B master remotes/origin/master --
git pull --progress -v --no-rebase "origin" master

echo Completed!
echo Press Enter to Exit
pause >nul
exit
