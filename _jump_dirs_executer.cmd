@ECHO OFF

IF "%~4"=="-cd" (
    IF "%~5"=="/d" (
        %1 %2 %3 -a "%~dpfn6\"
        IF NOT ERRORLEVEL 1 ( EXIT /B 0 ) ELSE ( EXIT /B 1 )
    )
    %1 %2 %3 -a "%~dpfn5\"
    IF NOT ERRORLEVEL 1 ( EXIT /B 0 ) ELSE ( EXIT /B 1 )
)

IF "%~4"=="-x" (
    %1 %2 %3 -x "%~dpfn5\"
    IF NOT ERRORLEVEL 1 ( EXIT /B 0 ) ELSE ( EXIT /B 1 )
)

IF "%~4"=="-c" (
    %1 %2 %3 %CD% %5 %6 %7 %8 %9
    IF NOT ERRORLEVEL 1 ( EXIT /B 0 ) ELSE ( EXIT /B 1 )
)

%*
IF NOT ERRORLEVEL 1 ( EXIT /B 0 ) ELSE ( EXIT /B 1 )
