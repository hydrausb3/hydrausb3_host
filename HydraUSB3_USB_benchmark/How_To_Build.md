### How To Build

GitHub Workflows / CI Build is available on [Build_HydraUSB3_USB_benchmark.yml](https://github.com/hydrausb3/hydrausb3_host/blob/main/.github/workflows/Build_HydraUSB3_USB_benchmark.yml)

###  GNU/Linux Ubuntu 20.04 LTS
#### Prerequisites
`sudo apt-get install gcc pkg-config libusb-1.0-0-dev`
#### Automatic build (using Makefile) 
`make`

### Windows MSYS2/mingw64
#### Prerequisites
- install MSYS2/mingw64 from https://www.msys2.org
- Start mingw64 console
- `pacman -Syu`
- `pacman -S mingw-w64-x86_64-make mingw-w64-x86_64-pkgconf mingw-w64-x86_64-gcc mingw-w64-x86_64-libusb`
#### Automatic build (using Makefile)
`ming32-make.exe`
- Optional copy libusb-1.0.dll to same directory as the executable(if it is not in your path)
  - `cp /mingw64/bin/libusb-1.0.dll ./`
