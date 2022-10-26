@echo off
set configname=%1
copy /Y ..\..\..\02_Libraries\ModbusAddress\01_Source\csmon.h* ..\..\..\04_Output_Files\%configname%