@echo off

SETLOCAL

SET _JUMP_DIRS_EXE_PATH=%~dp0%jump_dirs.exe

IF NOT EXIST %_JUMP_DIRS_EXE_PATH% (
    ECHO ERROR: File %_JUMP_DIRS_EXE_PATH% not found!
    ECHO Check that all necessary files are present and re-run the setup file
    EXIT /B 1
)

ECHO jump_dirs correctly loaded!

DOSKEY cd=ECHO CIAO ^& cd $*

ENDLOCAL
