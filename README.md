
# N64 Controller Test
![Release](https://github.com/Ryzee119/n64_controller_test/workflows/Release/badge.svg) ![badge](https://img.shields.io/github/license/Ryzee119/n64_controller_test)
 
A simple homebrew N64 rom made to perform some basic N64 controller tests.  
**WARNING: This has the potential the corrupt your saved games. Use with caution.**

## Download
You can download a precompiled binary from the [release section](https://github.com/Ryzee119/n64_controller_test/releases).
## Build
This was developed using the opensource N64 toolchain [libdragon](https://github.com/DragonMinded/libdragon).  
I built using a docker container. The build process is something like this:
```
apt-get install npm docker.io
git clone https://github.com/Ryzee119/n64_controller_test.git
cd n64_controller_test
npm install -g libdragon
libdragon download
libdragon start
libdragon make
```
This should produce a `.z64` rom file.

## Images
![page 1](https://github.com/Ryzee119/n64_controller_test/blob/master/images/page1.jpg?raw=true)

![page 2](https://github.com/Ryzee119/n64_controller_test/blob/master/images/page2.jpg?raw=true)

![page 3](https://github.com/Ryzee119/n64_controller_test/blob/master/images/page3.jpg?raw=true)
