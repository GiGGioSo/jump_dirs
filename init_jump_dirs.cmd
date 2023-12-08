@echo off

SETLOCAL

SET _JUMP_DIRS_EXE_PATH=%~dp0%jump_dirs.exe

SET _TMP_FILE_PATH=%~dp0%tmp_file.txt

SET _GET_FULLPATH_AND_EXECUTE=%~dp0%convert_path_and_execute.bat


IF NOT EXIST %_JUMP_DIRS_EXE_PATH% (
    ECHO ERROR: File %_JUMP_DIRS_EXE_PATH% not found!
    ECHO Check that all necessary files are present and re-run the setup file
    EXIT /B 1
)
IF NOT EXIST %_GET_FULLPATH_AND_EXECUTE% (
    ECHO ERROR: File %_GET_FULLPATH_AND_EXECUTE% not found!
    ECHO Check that all necessary files are present and re-run the setup file
    EXIT /B 2
)

IF "%JUMP_DIRS_DATAFILE%"=="" SET JUMP_DIRS_DATAFILE=%~dp0%_jump_dirs_data

DOSKEY z=(%_JUMP_DIRS_EXE_PATH% %JUMP_DIRS_DATAFILE% $*) ^> %_TMP_FILE_PATH% ^&^& (@ECHO OFF ^& FOR /f "delims=" %%d in ('TYPE %_TMP_FILE_PATH%') do CD /d "%%~d" ^& @ECHO ON) ^|^| TYPE %_TMP_FILE_PATH% 2>nul

DOSKEY cd=IF "$1" == "/D" (%_GET_FULLPATH_AND_EXECUTE% %_JUMP_DIRS_EXE_PATH% %JUMP_DIRS_DATAFILE% $2 ^& cd $*) ELSE (%_GET_FULLPATH_AND_EXECUTE% %_JUMP_DIRS_EXE_PATH% %JUMP_DIRS_DATAFILE% $1 ^& cd $*)

ENDLOCAL

