@ECHO OFF

SET _JUMP_DIRS_EXE_PATH=%~dp0%jump_dirs.exe

SET _JUMP_DIRS_TMP_FILE_PATH=%~dp0%_jump_dirs_tmp_file

SET _JUMP_DIRS_EXECUTER_PATH=%~dp0%_jump_dirs_executer.cmd

IF NOT EXIST %_JUMP_DIRS_EXE_PATH% (
    ECHO ERROR: File %_JUMP_DIRS_EXE_PATH% not found!
    ECHO Check that all necessary files are present and re-run the setup file
    EXIT /B 1
)
IF NOT EXIST %_JUMP_DIRS_EXECUTER_PATH% (
    ECHO ERROR: File %_JUMP_DIRS_EXECUTER_PATH% not found!
    ECHO Check that all necessary files are present and re-run the setup file
    EXIT /B 2
)

IF "%JUMP_DIRS_CMD%"=="" SET JUMP_DIRS_CMD=z

IF "%JUMP_DIRS_DATAFILE%"=="" SET JUMP_DIRS_DATAFILE=%~dp0%_jump_dirs_data_file

IF "%JUMP_DIRS_MAX_SCORE%"=="" SET JUMP_DIRS_MAX_SCORE=5000

IF "%JUMP_DIRS_CODEPAGE%"=="" SET JUMP_DIRS_CODEPAGE=1252


CHCP %JUMP_DIRS_CODEPAGE% > nul 2> nul
IF ERRORLEVEL 1 (
    ECHO ERROR: Could not change code page to %JUMP_DIRS_CODEPAGE%!
    EXIT /B 3
)


DOSKEY %JUMP_DIRS_CMD%=(cmd /c "%_JUMP_DIRS_EXECUTER_PATH% %_JUMP_DIRS_EXE_PATH% %JUMP_DIRS_DATAFILE% %JUMP_DIRS_MAX_SCORE% $*") ^> %_JUMP_DIRS_TMP_FILE_PATH% ^&^& (@ECHO OFF ^& FOR /f "delims=" %%d in ('TYPE %_JUMP_DIRS_TMP_FILE_PATH%') do CD /d "%%~d" ^& @ECHO ON) ^|^| TYPE %_JUMP_DIRS_TMP_FILE_PATH% 2>nul

DOSKEY cd=(CALL %_JUMP_DIRS_EXECUTER_PATH% %_JUMP_DIRS_EXE_PATH% %JUMP_DIRS_DATAFILE% %JUMP_DIRS_MAX_SCORE% -cd $* ^& cd $*)
