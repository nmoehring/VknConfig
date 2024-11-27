
IF "%1" == "" (
    cmake . --preset debug
    cmake --build --preset debug
) ELSE (
    cmake . --preset debug -DTARGET_CHAPTER=%1
    cmake --build --preset debug -DTARGET_CHAPTER=%1
)