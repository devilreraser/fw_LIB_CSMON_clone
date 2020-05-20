@echo on
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
git.exe checkout -f -B master remotes/convesys/master --
git pull
git checkout remotes/github/master
git merge --allow-unrelated-histories -s recursive -Xtheirs master < ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git switch -C master
git push github master
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/convesys/master --
cd ..\..
echo.
echo Process MathAndControl:
cd 02_Libraries\MathAndControl
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/convesys/master --
git pull
git checkout remotes/github/master
git merge --allow-unrelated-histories -s recursive -Xtheirs master < ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git switch -C master
git push github master
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/convesys/master --
cd ..\..
echo.
echo Process ModbusAddress:
cd 02_Libraries\ModbusAddress
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/convesys/master --
git pull
git checkout remotes/github/master
git merge --allow-unrelated-histories -s recursive -Xtheirs master < ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git switch -C master
git push github master
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/convesys/master --
cd ..\..
echo.
echo Process Peripheral:
cd 02_Libraries\Peripheral
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/convesys/master --
git pull
git checkout remotes/github/master
git merge --allow-unrelated-histories -s recursive -Xtheirs master < ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git switch -C master
git push github master
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/convesys/master --
cd ..\..
echo.
echo Process TestControl:
cd 02_Libraries\TestControl
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/convesys/master --
git pull
git checkout remotes/github/master
git merge --allow-unrelated-histories -s recursive -Xtheirs master < ConvesysToGithub.in
git commit -a -m "%_CommitString%"
git switch -C master
git push github master
git.exe fetch --all -v --progress
git.exe checkout -f -B master remotes/convesys/master --
cd ..\..
echo.
echo Process Project:
git.exe fetch --all -v --progress
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
git.exe checkout -f -B master remotes/convesys/master --
echo Completed!
echo Press Enter to Exit
pause >nul
exit