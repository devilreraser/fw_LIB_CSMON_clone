:GetBatchFilePath
set pathbatch=%~dp0
set paramPath=%1
set "paramPath=%paramPath:/=\%"
start /w %pathbatch%CsMonFirmwareFilesGen\CsMonFirmwareFilesGen\CsMonFirmwareFilesGen %paramPath%
