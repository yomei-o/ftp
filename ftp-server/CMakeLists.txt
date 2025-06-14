cmake_minimum_required(VERSION 3.13)
project(ftp_server_cmake CXX C)

#mingw
#cmake -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -G "MinGW Makefiles" ..
#


add_executable(ftp_server 
	src/BS_thread_pool.cpp src/filesystem_.cpp
	src/dprintf_.c src/listen_for_client.c src/manage_client.c src/nicaddr.c
	src/request.c src/server.c src/server_command.c src/socket.c src/usleep_.cpp
	)

target_compile_features(ftp_server PRIVATE cxx_std_17)

set(CMAKE_CXX_FLAGS "-fpermissive")
set(CMAKE_C_FLAGS "-Wimplicit-function-declaration")

if(WIN32)
target_link_libraries (ftp_server PRIVATE ws2_32 iphlpapi)
endif()

