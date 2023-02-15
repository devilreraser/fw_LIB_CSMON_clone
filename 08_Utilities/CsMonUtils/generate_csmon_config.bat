:GetBatchFilePath
set pathbatch=%~dp0
set paramPath=%1
set "paramPath=%paramPath:/=\%"
%pathbatch%CsMonFirmwareFilesGen\CsMonFirmwareFilesGen\CsMonFirmwareFilesGen %paramPath%