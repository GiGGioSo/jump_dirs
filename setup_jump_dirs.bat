@ECHO off
SETLOCAL

ECHO Starting the setup of jump_dirs...

SET INIT_FILE_PATH=%~dp0%init_jump_dirs.cmd

IF EXIST %INIT_FILE_PATH% (
    reg add "HKCU\Software\Microsoft\Command Processor" /v AutoRun /t REG_EXPAND_SZ /d "%INIT_FILE_PATH%" /f
) ELSE (
    ECHO ERROR: Could not find the file %INIT_FILE_PATH%!
    EXIT /B 1
)

ECHO jump_dirs was setup successfully!

ECHO Restart the terminal for the changes to take effect

ENDLOCAL
