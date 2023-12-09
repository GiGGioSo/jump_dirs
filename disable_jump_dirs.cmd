@ECHO off
SETLOCAL

ECHO Disabling jump_dirs...

REG DELETE "HKCU\Software\Microsoft\Command Processor" /v AutoRun /f

ECHO Successfully disabled jump_dirs!
ECHO Restart the terminal for the changes to take effect

ENDLOCAL
