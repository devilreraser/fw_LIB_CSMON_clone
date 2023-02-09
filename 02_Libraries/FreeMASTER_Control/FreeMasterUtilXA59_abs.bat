rem Put this Line to Execute After Build:    ${CS_LIB_CSMON_LIBRARIES}\FreeMASTER_Control\FreeMasterUtilXA59.bat TI 04_Output_Files ${ProjName} ${ConfigName} ${CS_LIB_CSMON_LIBRARIES}
set DeviceFamily=%1
set OutputPath="%2"
set ProjName=%3
set ConfigName=%4
set LibrariesPath="%5"

pushd .
cd %OutputPath%
set OutputPath=%CD%
popd

set FreeMASTER_OutputPath=%OutputPath%\%ConfigName%\FreeMASTER_Control
rmdir /S /Q %FreeMASTER_OutputPath%
mkdir %FreeMASTER_OutputPath%

pushd .
cd %FreeMASTER_OutputPath%
set FreeMASTER_OutputPath=%CD%
popd

pushd .
cd %LibrariesPath%
set LibrariesPath=%CD%
popd

set FreeMASTER_Path=%LibrariesPath%\FreeMASTER_Control

%FreeMASTER_Path%\FreeMasterUtilXA59.exe i:%OutputPath%\%ConfigName%\%ProjName%.map o: s:*.c s:*.h p:off rname:1 raddr:2 rsize:4 regex:%DeviceFamily% 

set FreeMASTER_DefaultPath=%OutputPath%\%ConfigName%

move /Y %FreeMASTER_DefaultPath%\*.var %FreeMASTER_OutputPath%
move /Y %FreeMASTER_DefaultPath%\*.adr %FreeMASTER_OutputPath%
move /Y %FreeMASTER_DefaultPath%\*.map %FreeMASTER_OutputPath%
