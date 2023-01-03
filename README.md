# redesigned-train
---
### Requirements:

general
* main language is C++ (up to 17 standard)
* building tools GNU Make or Cmake
* Boost and glibc are only available.
* build and work correctly on Linux

server side
* server can work with multiple clients
* process input data 
* easy to update new ways to process data

client side:
* no any specific restrictions

full info located in requirements.pdf


---

### Dependencies
boost-1.81.0 (put boost sources in root). Important: build header-only library
TODO: check custom location library support (find_package)
---

### How to build in linux:
```bash 
mkdir build &&  cd build
```
```bash 
cmake ..
```
```bash 
cmake --build .
```
---

### It's possible to build for windows with using cygwin Unix Socket Emulation library
In additional to linux build use:
```bash 
g++ client.cpp
```
---
### Run server:
```bash 
server.exe 32101
```
you can run without argument in this case server will run on 15001 porn
### Run client to localhost 32101
```bash 
client.exe 127.0.0.1 32101
```
