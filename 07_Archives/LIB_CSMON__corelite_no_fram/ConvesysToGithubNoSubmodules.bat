@echo off
echo.
SET /P _CommitString=Please enter Commit Message:
IF "%_CommitString%"=="" SET _CommitString=Merged Convesys
echo Commit Message: "%_CommitString%"
echo.
echo Process Project:
git.exe fetch --all -v --progress
git.exe checkout -f -B master origin/master --
git pull
git.exe checkout -f -B master remotes/github/master --
git pull
git.exe checkout -f -B master remotes/convesys/master --
git pull
git checkout remotes/github/master
git merge --no-commit --allow-unrelated-histories -s recursive -Xtheirs master < ConvesysToGithub.in
git reset HEAD .gitattributes
git checkout remotes/github/master -- .gitattributes
git reset HEAD .gitmodules
git checkout remotes/github/master -- .gitmodules
git commit -a -m "%_CommitString%"
git switch -C master
git push github master
git.exe fetch --all -v --progress
git.exe checkout -f -B master origin/master --
git pull
echo Completed!
echo Press Enter to Exit
pause >nul
exit