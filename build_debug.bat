set configCommand=cmake . --preset debug
set buildCommand=cmake --build --preset debug

IF "%1" == "-build-all" (
    set configCommand=%configCommand% -DTARGET_SUBDIR=ALL
) ELSE (
    set configCommand=%configCommand% -DTARGET_SUBDIR=%1
)

IF "%2" == "-log" (
    set configCommand=%configCommand% > config.log
    set buildCommand=%buildCommand% > build.log
) ELSE IF "%2" == "-trace-log" (
    set configCommand=%configCommand% --trace > config.log
    set buildCommand=%buildCommand% --trace > build.log
)

%configCommand%
%buildCommand%