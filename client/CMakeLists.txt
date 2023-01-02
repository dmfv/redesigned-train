cmake_minimum_required(VERSION 3.2)
set(PROJECT_NAME client)
project(${PROJECT_NAME})

set(SOURCES client.cpp)

# add here Cygwin Unix emulation library if needed
# include_directories("C:/Programms/cygwin64/usr/include")
# include_directories("C:/Programms/cygwin64/usr/include/sys")
# if("WIN32" OR "WIN64")
#   # set stuff for windows
# endif()

# set(COMPILER_FLAGS -Wall)
set(COMPILER_FEATURES cxx_std_17)

add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_options(${PROJECT_NAME}  PRIVATE ${COMPILER_FLAGS})
target_compile_features(${PROJECT_NAME} PRIVATE ${COMPILER_FEATURES})