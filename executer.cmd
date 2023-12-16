@ECHO OFF

IF "%3"=="-cd" (
    IF "%4"=="/d" (
        %1 %2 -a "%~dpfn5"
        IF NOT ERRORLEVEL 1 ( EXIT /B 0 ) ELSE ( EXIT /B 1 )
    )
    IF "%4"=="/d" (
        %1 %2 -a "%~dpfn5"
        IF NOT ERRORLEVEL 1 ( EXIT /B 0 ) ELSE ( EXIT /B 1 )
    )
    %1 %2 -a "%~dpfn4"
    IF NOT ERRORLEVEL 1 ( EXIT /B 0 ) ELSE ( EXIT /B 1 )
)

IF "%3"=="-x" (
    %1 %2 -x "%~dpfn4"
    IF NOT ERRORLEVEL 1 ( EXIT /B 0 ) ELSE ( EXIT /B 1 )
)

%*
IF NOT ERRORLEVEL 1 ( EXIT /B 0 ) ELSE ( EXIT /B 1 )
