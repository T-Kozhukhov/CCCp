cmake_minimum_required(VERSION 3.1)
project(CCCp) #project name
set(CMAKE_BUILD_TYPE Release) #force release build

#allow us to use multithreading in the code
set(CMAKE_CXX_STANDARD 11) #use C++11, required to use threads
find_package(Threads) #get libraries required for multithreading... WHAT THE HELL GCC THIS IS IN THE STANDARD?!?!?!

#getting number of compute threads from user
if(${COMPUTE_THREAD_COUNT} STREQUAL "") #check if threads were defined properly
	
endif(${COMPUTE_THREAD_COUNT} STREQUAL "")
add_definitions(-D_COMPUTE_THREAD_COUNT=${COMPUTE_THREAD_COUNT}) #set the core count, should be system dependent
message("Set to compile with ${COMPUTE_THREAD_COUNT} compute threads!") #print message

#normal stuff
include_directories(include) #add headers
file(GLOB SOURCES "src/*.cpp") #add source files

#GSL
find_package(GSL REQUIRED) #add GSL libraries

#VTK
find_package(VTK REQUIRED) #add VTK libs
include(${VTK_USE_FILE})

#generate executable
add_executable(CCCp ${SOURCES})

#link libraries
target_link_libraries(CCCp GSL::gsl GSL::gslcblas) #link GSL libs
target_link_libraries(CCCp ${VTK_LIBRARIES}) #link VTK libs
target_link_libraries (CCCp ${CMAKE_THREAD_LIBS_INIT}) #link thread related libs
