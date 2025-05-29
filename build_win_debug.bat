@echo off
echo -- Config and Build Script running --
set configCommand=cmake . --preset winDebug
set buildCommand=cmake --build --preset winDebugBuild

IF "%1" == "-log" (
    echo -- Logging enabled --
    set "configCommand=%configCommand% > logs\config.log"
    set "buildCommand=%buildCommand% > logs\build.log"
) ELSE IF "%1" == "-trace-log" (
    echo -- Trace logging enabled --
    set "configCommand=%configCommand% --trace > config.log"
    set "buildCommand=%buildCommand% --trace > build.log"
)

echo -- Configuring --
%configCommand%
echo -- Building --
%buildCommand%