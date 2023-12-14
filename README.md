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

Execute the following PowerShell command.

```powershell
./build.ps1
```

# Known Errors

- [cannot open file 'libffi-7.lib'](https://discuss.python.org/t/struggling-to-build-python-on-windows-10/11031)
- Veridic template parameters overload error(in `include/debugmsg.h`)
