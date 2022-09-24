rem Put this Line to Execute After Build for XC8  Compiler:    ..\..\02_Libraries\FreeMASTER_Control\FreeMasterUtilXA59.bat PIC_XC8 dist\${ConfName}\${IMAGE_TYPE}
rem Put this Line to Execute After Build for XC16 Compiler:    ..\..\02_Libraries\FreeMASTER_Control\FreeMasterUtilXA59.bat PIC dist\${ConfName}\${IMAGE_TYPE}
set DeviceFamily=%1
set ExecutablePath=%2
set ProjectName=%3
set OutputType=%4
rem ..\..\02_Libraries\FreeMASTER_Control\FreeMasterUtilXA59.exe d:.\ i:.\%ExecutablePath%\*.map o: s:..\..\*.c s:..\..\*.h p:off regex:%DeviceFamily% s:..\..\..\fw_LIB_Freemaster\02_Libraries\FreeMASTER_Sources_LIB\src_common\*.c s:..\..\..\fw_LIB_Freemaster\02_Libraries\FreeMASTER_Sources_LIB\src_common\*.h  s:..\..\..\fw_LIB_Freemaster\02_Libraries\FreeMASTER_Sources_LIB\PIC\*.c s:..\..\..\fw_LIB_Freemaster\02_Libraries\FreeMASTER_Sources_LIB\PIC\*.h s:..\..\..\fw_LIB_MathAndControl\01_Source\*.c s:..\..\..\fw_LIB_MathAndControl\01_Source\*.h s:..\..\..\fw_LIB_HexMonitor\01_Source\*.c s:..\..\..\fw_LIB_HexMonitor\01_Source\*.h s:..\..\..\fw_LIB_Freemaster\01_Source\*.c s:..\..\..\fw_LIB_Freemaster\01_Source\*.h  s:..\..\..\fw_LIB_Freemaster\01_Source\PIC\*.c s:..\..\..\fw_LIB_Freemaster\01_Source\PIC\*.h s:..\..\..\fw_LIB_MathAndControl\01_Source\*.c s:..\..\..\fw_LIB_MathAndControl\01_Source\*.h s:..\..\..\fw_LIB_HexMonitor\01_Source\*.c s:..\..\..\fw_LIB_HexMonitor\01_Source\*.h
..\..\02_Libraries\FreeMASTER_Control\FreeMasterUtilXA59.exe d:.\ i:.\%ExecutablePath%\*.map o: s:..\..\*.c s:..\..\*.h p:off regex:%DeviceFamily%
copy /Y .\%ExecutablePath%\*.var ..\..\02_Libraries\FreeMASTER_Control\HexMon.var
copy /Y .\%ExecutablePath%\*.adr ..\..\02_Libraries\FreeMASTER_Control\NexMon.adr
copy /Y .\%ExecutablePath%\%ProjectName%*.%OutputType% ..\..\02_Libraries\FreeMASTER_Control\HexMon.%OutputType%
rem pause