cmake_minimum_required(VERSION 3.13)
project(ftp_server_cmake CXX C)

#mingw
#cmake -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -G "MinGW Makefiles" ..
#


add_executable(ftp_client
	src/BS_thread_pool.cpp src/mutex_.cpp
	src/client_commands.c src/connect_to_server.c src/socket.c src/usleep_.cpp
	src/client.c src/nicaddr.c
	)

target_compile_features(ftp_client PRIVATE cxx_std_17)

set(CMAKE_CXX_FLAGS "-fpermissive")
set(CMAKE_C_FLAGS "-Wimplicit-function-declaration")

if(WIN32)
target_link_libraries (ftp_client PRIVATE ws2_32 iphlpapi)
endif()

