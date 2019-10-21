cmake_minimum_required(VERSION 3.1)
project(CCCmt) #project name
set(CMAKE_BUILD_TYPE Release) #force release build

#multithread release stuff
set (CMAKE_CXX_STANDARD 11) #use C++11, required to use threads
add_definitions(-DCORECOUNT=4) #set the core count, should be system dependent
find_package(Threads) #get libraries required for multithreading... WHAT THE HELL GCC THIS IS IN THE STANDARD?!?!?!

#normal stuff
include_directories(include) #add headers
file(GLOB SOURCES "src/*.cpp") #add source files

#GSL
find_package(GSL REQUIRED) #add GSL libraries

#VTK
find_package(VTK REQUIRED) #add VTK libs
include(${VTK_USE_FILE})

#generate executable
add_executable(CCCmt ${SOURCES})

#link libraries
target_link_libraries(CCCmt GSL::gsl GSL::gslcblas) #link GSL libs
target_link_libraries(CCCmt ${VTK_LIBRARIES}) #link VTK libs
target_link_libraries (CCCmt ${CMAKE_THREAD_LIBS_INIT}) #link thread related libs
