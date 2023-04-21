rem Put this Line to Execute After Build:    ..\..\..\02_Libraries\FreeMASTER_Control\FreeMasterUtilXA59.bat TI 04_Output_Files ${ProjName} ${ConfigName}
set DeviceFamily=%1
set OutputPath=%2
set ProjName=%3
set ConfigName=%4

::del ..\..\..\02_Libraries\FreeMASTER_Control\*.map
..\..\..\02_Libraries\FreeMASTER_Control\FreeMasterUtilXA59.exe d:..\..\..\ i:.\%OutputPath%\%ConfigName%\%ProjName%.map o: s:*.c s:*.h p:off rname:1 raddr:2 rsize:4 regex:%DeviceFamily% 
copy /Y ..\..\..\%OutputPath%\%ConfigName%\*.var  ..\..\..\02_Libraries\FreeMASTER_Control
copy /Y ..\..\..\%OutputPath%\%ConfigName%\*.adr  ..\..\..\02_Libraries\FreeMASTER_Control
copy /Y ..\..\..\%OutputPath%\%ConfigName%\*.map ..\..\..\02_Libraries\FreeMASTER_Control