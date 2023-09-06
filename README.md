# LBlocks
 Modularization tool using blocks with input and output

## Install
```
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=<Target Path>
cmake --install .
```

## Build test
```
cmake .. -DBUILD_TEST=1
cmake --build .
```

## Linux conio.h
[zoelabbb/conio.h](https://github.com/zoelabbb/conio.h.git) is used for the test of simple pd control for a mass system, on Linux. You can find it in /include/test/linux.