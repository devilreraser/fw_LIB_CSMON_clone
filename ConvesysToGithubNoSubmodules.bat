@echo off
echo.
SET /P _CommitString=Please enter Commit Message:
IF "%_CommitString%"=="" SET _CommitString=Merged Convesys
echo Commit Message: "%_CommitString%"
echo.
echo Process Project:
git.exe fetch --all -v --progress
git.exe pull --progress -v --no-rebase "origin"
git.exe merge --no-commit --no-ff --allow-unrelated-histories remotes/convesys/master < ConvesysToGithub.in
git reset HEAD .gitattributes
git checkout -- .gitattributes
git reset HEAD .gitmodules
git checkout -- .gitmodules
git commit -a -m "%_CommitString%"
git.exe push --progress "origin" master:master
echo Completed!
echo Press Enter to Exit
pause >nul
exit