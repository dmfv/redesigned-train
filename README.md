# redesigned-train
Requerments:
boost-1.81.0 (put boost sources in root). Important: build header-only library
TODO: check custom location library support (find_package)

How to build in linux:
mkdir build &&  cd build
cmake ..
cmake --build .

It's possible to build for windows with using cygwin Unix Socket Emulation library
in additional to linux build use:
g++ client.cpp
