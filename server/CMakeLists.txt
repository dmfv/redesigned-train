cmake_minimum_required(VERSION 3.2)
set(PROJECT_NAME server)
project(${PROJECT_NAME})

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

# set(BOOST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/boost-1.81.0")
set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/boost-1.81.0")
set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}/include")
set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/stage/lib")

include_directories(include)
set(SOURCES server.cpp 
            # sources/led_manipulator.cpp 
            # sources/led_commands.cpp
   )

# set(COMPILER_FLAGS -Wall)
set(COMPILER_FEATURES cxx_std_17)

find_package(Boost 1.80.0 REQUIRED)

include_directories(${BOOST_ROOT})
add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_options(${PROJECT_NAME}  PRIVATE ${COMPILER_FLAGS})
target_compile_features(${PROJECT_NAME} PRIVATE ${COMPILER_FEATURES})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})