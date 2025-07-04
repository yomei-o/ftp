# FTP

FTP server and client following standards in ***RFC 959***. 

Based on GlenGGG' FTP.

https://github.com/GlenGGG/FTP

Support active and passive connection mode. 

Developed for Linux system, MacOS and Windows.

Has connectivity with Windows ftp client and WinSCP.

Compiled with Visual Stdio 2022, mingw, clang and gcc.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

What things you need to install the software and how to install them

```
You need to have g++, gcc, make and cmake in your PATH.
```

### Installing

Clone this repository.

```
git clone https://github.com/yomei-o/ftp.git
cd ftp
```

Run make to build client and server.

```
cd ftp-client
mkdir build
cd build
cmake ..
make

cd ../..
cd ftp-server
mkdir build
cd build
cmake ..
make
```
### Usage

Start server

```
sudo ./server [server command port] [server-root-path]
```

Start client

```
sudo ./client [server ip] [server command port]
```

#### client side commands:

| Commands                           | Descriptions                                    |
| ---------------------------------- | :---------------------------------------------- |
| user [username]                    | login as [username]                             |
| pass                               | enter password                                  |
| pwd                                | print current working directory                 |
| list or ls                         | list directory                                  |
| cwd or cd [directory]              | change directory                                |
| retr or get [file path]            | get file                                        |
| stor or put [file path]            | store file                                      |
| pasv                               | enter passive mode                              |
| port [ip1,ip2,ip3,ip4,port1,port2] | enter active mode, refer to RFC 959 for details |

