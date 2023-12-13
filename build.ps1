#! NOTE: VS2022 gives me a syntax error when I try to compile 5.14.2. Therefore, use the one for VS2019.
# If GitHub CI definition, then ignore.
if (!$env:Qt5_DIR) {
  $env:Qt5_DIR = Join-Path -Path $(Get-Location) -ChildPath "build/5.15.2/msvc2019/lib/cmake"
  if (!$(Test-Path $env:Qt5_DIR)) {
    Write-Host "Not found aqt(Qt install command). Try to install with Python3..." -ForegroundColor Cyan
    if (Get-Command rye -ErrorAction SilentlyContinue) { rye install apt }
    elseif (Get-Command python3 -ErrorAction SilentlyContinue) { pip install aqt }
    else { throw "Couldn't found Python3" }
    aqt install-qt windows desktop 5.15.2 win32_msvc2019 -O ./build
  }
}

# -S: source path
# -B: build path
# -A Win32: Need IA-32(32bit) mode for `hkxcmd`
cmake -S . -B ./build -A Win32 -D CMAKE_BUILD_TYPE=Release
cmake --build ./build -j $Env:NUMBER_OF_PROCESSORS --config Release
