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
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/github/master --
git merge --no-commit --allow-unrelated-histories -Xrenormalize remotes/convesys/master < ConvesysToGithub.in
IF %ERRORLEVEL% == 0 goto CommitProject
IF %ERRORLEVEL% NEQ 0 Echo Error = %ERRORLEVEL%
echo Please Resolve Merge Conflicts In PROJECT And Press Enter To Continue
pause >nul
:CommitProject
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
