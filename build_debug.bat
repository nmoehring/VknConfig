
IF "%1" == "" (
    cmake . --preset debug -DTARGET_SUBDIR=ALL
    cmake --build --preset debug
) ELSE (
    cmake . --preset debug -DTARGET_SUBDIR=%1
    cmake --build --preset debug 
)