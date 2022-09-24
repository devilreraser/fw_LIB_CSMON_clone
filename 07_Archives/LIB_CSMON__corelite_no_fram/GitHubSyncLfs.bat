@echo off
echo.
echo Process Project Sync LFS Data Github Only:
echo.
echo Press Enter to Continue... or Just Close The Console if LFS Not Supported
pause >nul
echo.
git lfs install
git lfs status
git lfs pull
git lfs status
echo.
echo Completed!
echo Press Enter to Exit
pause >nul
exit