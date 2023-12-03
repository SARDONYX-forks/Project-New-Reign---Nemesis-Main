# Project-New-Reign---Nemesis-Main

Nemesis is currently being written from scratch. So existing issues will likely
be not valid once it is done. Hence, I came to the conclusion of archiving this
repository for the time being

Thanks to all for supporting this project

## How to build

- Visual Studio
  - Feature: msvc v140 VS2015 C++ (Need to build Cpython 3.8.2)

- Python(For aqt command.This is Unofficial Qt installer)

```powershell
mkdir -p ./build;cd ./build;
pip install aqt
aqt install-qt windows desktop 5.14.2 win32_msvc2017
cmake -DCMAKE_BUILD_TYPE=Release;cmake -A Win32 ..
```
