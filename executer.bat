@ECHO OFF

IF "%3"=="-cd" (
    %1 %2 -a %~dpfn4
    IF NOT ERRORLEVEL 1 ( EXIT /B 0 ) ELSE ( EXIT /B 1 )
)

IF "%3"=="-x" (
    %1 %2 -x %~dpfn4
    IF NOT ERRORLEVEL 1 ( EXIT /B 0 ) ELSE ( EXIT /B 1 )
)

%1 %2 %3 %4 %5 %6 %7 %8
IF NOT ERRORLEVEL 1 ( EXIT /B 0 ) ELSE ( EXIT /B 1 )
