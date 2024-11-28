set vkckbkConfig=cmake . --preset debug
set vkckbkBuild=cmake --build --preset debug

IF "%1" == "-build-all" (
    set vkckbkConfig=%vkckbkConfig% -DTARGET_SUBDIR=ALL
) ELSE (
    set vkckbkConfig=%vkckbkConfig% -DTARGET_SUBDIR=%1
)

IF "%2" == "-log" (
    set vkckbkConfig=%vkckbkConfig% > config.log
    set vkckbkBuild=%vkckbkBuild% > build.log
) ELSE IF "%2" == "-trace-log" (
    set vkckbkConfig=%vkckbkConfig% --trace > config.log
    set vkckbkBuild=%vkckbkBuild% --trace > build.log
)

%vkckbkConfig%
%vkckbkBuild%