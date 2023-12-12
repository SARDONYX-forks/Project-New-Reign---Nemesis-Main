# Project-New-Reign---Nemesis-Main

Nemesis is currently being written from scratch. So existing issues will likely
be not valid once it is done. Hence, I came to the conclusion of archiving this
repository for the time being

Thanks to all for supporting this project

## How to build

- Visual Studio
  - Feature: msvc v140 VS2015 C++ (Need to build Cpython 3.8.2)

- Python(For aqt command.This is Unofficial Qt installer)
  - version: 3.8~3.10
    - To avoid this error
      [cannot open file 'libffi-7.lib'](https://discuss.python.org/t/struggling-to-build-python-on-windows-10/11031)

1.Edit `CMakeLists.txt` when Local build

```cmake
option(UseLocalQt "Uses local copy of Qt instead of GitHub Action" ON) # OFF -> ON
set(Qt5_DIR "${PROJECT_SOURCE_DIR}/build/5.14.1/msvc2017/lib/cmake/Qt5" CACHE PATH "Qt local path")
```

2.Execute the following PowerShell command.

```powershell
New-Item -ItemType Directory ./build -ErrorAction SilentlyContinue
Set-Location ./build
pip install aqt
aqt install-qt windows desktop 5.14.2 win32_msvc2017
cmake -A Win32 ..
cmake --build . -j $Env:NUMBER_OF_PROCESSORS --config Release
```
