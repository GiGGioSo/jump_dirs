@echo off

SETLOCAL

SET _JUMP_DIRS_EXE_PATH=%~dp0%jump_dirs.exe

SET _TMP_FILE_PATH=%~dp0%_jump_dirs_tmp_file

SET _EXECUTER=%~dp0%executer.cmd

IF NOT EXIST %_JUMP_DIRS_EXE_PATH% (
    ECHO ERROR: File %_JUMP_DIRS_EXE_PATH% not found!
    ECHO Check that all necessary files are present and re-run the setup file
    EXIT /B 1
)
IF NOT EXIST %_EXECUTER% (
    ECHO ERROR: File %_EXECUTER% not found!
    ECHO Check that all necessary files are present and re-run the setup file
    EXIT /B 2
)

IF "%JUMP_DIRS_DATAFILE%"=="" SET JUMP_DIRS_DATAFILE=%~dp0%_jump_dirs_data_file

IF "%JUMP_DIRS_CMD%"=="" SET JUMP_DIRS_CMD=z

DOSKEY %JUMP_DIRS_CMD%=(cmd /c "%_EXECUTER% %_JUMP_DIRS_EXE_PATH% %JUMP_DIRS_DATAFILE% $*") ^> %_TMP_FILE_PATH% ^&^& (@ECHO OFF ^& FOR /f "delims=" %%d in ('TYPE %_TMP_FILE_PATH%') do CD /d "%%~d" ^& @ECHO ON) ^|^| TYPE %_TMP_FILE_PATH% 2>nul

DOSKEY cd=IF "$1" == "/D" (cmd /c "%_EXECUTER% %_JUMP_DIRS_EXE_PATH% %JUMP_DIRS_DATAFILE% -cd $2" ^& cd $*) ELSE (cmd /c "%_EXECUTER% %_JUMP_DIRS_EXE_PATH% %JUMP_DIRS_DATAFILE% -cd $1" ^& cd $*)

ENDLOCAL

