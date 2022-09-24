@echo off
echo. In Github Origin Repository 
echo. "convesys" to "github" automatic transfer
SET /P _CommitString= Please enter Commit Message:
IF "%_CommitString%"=="" SET _CommitString=Merged Convesys
echo Commit Message: "%_CommitString%"
echo.
echo Process Submodules:
echo.
echo Process HexMonitor:
cd 02_Libraries\HexMonitor
git.exe fetch --all -v --progress
git.exe pull --progress -v --no-rebase "origin"
git.exe merge --allow-unrelated-histories -Xrenormalize remotes/convesys/master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git.exe push --progress "origin" master:master
cd ..\..
echo.
echo Process MathAndControl:
cd 02_Libraries\MathAndControl
git.exe fetch --all -v --progress
git.exe pull --progress -v --no-rebase "origin"
git.exe merge --allow-unrelated-histories -Xrenormalize remotes/convesys/master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git.exe push --progress "origin" master:master
cd ..\..
echo.
echo Process ModbusAddress:
cd 02_Libraries\ModbusAddress
git.exe fetch --all -v --progress
git.exe pull --progress -v --no-rebase "origin"
git.exe merge --allow-unrelated-histories -Xrenormalize remotes/convesys/master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git.exe push --progress "origin" master:master
cd ..\..
echo.
echo Process Peripheral:
cd 02_Libraries\Peripheral
git.exe fetch --all -v --progress
git.exe pull --progress -v --no-rebase "origin"
git.exe merge --allow-unrelated-histories -Xrenormalize remotes/convesys/master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git.exe push --progress "origin" master:master
cd ..\..
echo.
echo Process TestControl:
cd 02_Libraries\TestControl
git.exe fetch --all -v --progress
git.exe pull --progress -v --no-rebase "origin"
git.exe merge --allow-unrelated-histories -Xrenormalize remotes/convesys/master < ..\..\ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git.exe push --progress "origin" master:master
cd ..\..
echo.
echo Process Project:
git.exe fetch --all -v --progress
git.exe pull --progress -v --no-rebase "origin"
git.exe merge --no-commit --allow-unrelated-histories -Xrenormalize remotes/convesys/master < ConvesysToGithub.in
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