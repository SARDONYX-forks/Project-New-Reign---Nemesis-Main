param (
    [string]$QtDir = "./packages/qt/6.10.2/msvc2022_64"
)

$ErrorActionPreference = "Stop"

# ---- paths -------------------------------------------------

$RootDir = Resolve-Path .
$AppDir = Join-Path $RootDir "Nemesis_App"
$BuildDir = Join-Path $AppDir "build"

$QmlDir = Join-Path $AppDir "qml"
$ResourcesDir = Join-Path $AppDir "resources"
$ShadersDir = Join-Path $AppDir "shaders"
$QsbDir = Join-Path $BuildDir "shaders_qsb"

$OutputQrc = Join-Path $BuildDir "auto_resources.qrc"

$QsbExe = Join-Path $QtDir "bin/qsb.exe"

# ---- mkdir -------------------------------------------------

New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null
New-Item -ItemType Directory -Force -Path $QsbDir | Out-Null

# ---- 1. compile shaders -----------------------------------

$ShaderExts = @("*.frag", "*.vert")

foreach ($ext in $ShaderExts) {
    Get-ChildItem -Path $ShadersDir -Recurse -Filter $ext | ForEach-Object {

        $out = Join-Path $QsbDir ($_.Name + ".qsb")

        if (!(Test-Path $out) -or ($_.LastWriteTime -gt (Get-Item $out).LastWriteTime)) {
            & $QsbExe `
                --qt6 `
                --glsl 450 `
                --hlsl 50 `
                --msl 12 `
                -o $out `
                $_.FullName
        }
    }
}

# ---- 2. generate QRC --------------------------------------

$qrc = @()
$qrc += "<RCC>"

function Write-Block {
    param (
        [string]$Prefix,
        [array]$Files,
        [string]$BaseDir
    )

    if ($Files.Count -eq 0) { return @() }

    $lines = @()
    $lines += "  <qresource prefix=`"$Prefix`">"

    foreach ($f in $Files) {
        $rel = Resolve-Path $f | ForEach-Object {
            $_.Path.Substring($BaseDir.Length + 1).Replace("\", "/")
        }
        $lines += "    <file alias=`"$rel`">$($f.Replace('\','/'))</file>"
    }

    $lines += "  </qresource>"
    return $lines
}

# QML
$qmlFiles = Get-ChildItem $QmlDir -Recurse -Filter *.qml | ForEach-Object { $_.FullName }
$qrc += Write-Block "/qml" $qmlFiles $QmlDir

# Images
$imgFiles = Get-ChildItem $ResourcesDir -Recurse -Filter *.png | ForEach-Object { $_.FullName }
$qrc += Write-Block "/resources" $imgFiles $ResourcesDir

# Shaders (.qsb)
$qsbFiles = Get-ChildItem $QsbDir -Filter *.qsb | ForEach-Object { $_.FullName }
$qrc += Write-Block "/shaders" $qsbFiles $QsbDir

$qrc += "</RCC>"

Set-Content -Encoding UTF8 $OutputQrc ($qrc -join "`n")

Write-Host "Generated: $OutputQrc"
