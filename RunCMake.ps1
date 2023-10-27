param 
(
    [Parameter(Mandatory = $True)] [ValidateSet('Win64vs2019', 'Uwp64vs2019', 'Win64vs2022', 'Uwp64vs2022', 'Prospero')][string] $Target,
    [switch]$NoUnityBuild,
    [switch]$NoSubmoduleUpdate,
    [string]$SolutionName
)

Set-Location $PSScriptRoot

if ($NoSubmoduleUpdate -eq $False) {
    $CURRENT_COMMIT = git log -n 1 --format=%H

    Write-Host "Current commit: $CURRENT_COMMIT"

    $UPDATE_SUBMODULES = $True
    $LAST_UPDATE_FILE = "$PSScriptRoot\Data\Content\AssetCache\LastSubmoduleUpdate.txt" 

    if (Test-Path $LAST_UPDATE_FILE -PathType Leaf -ErrorAction SilentlyContinue) {
        $LAST_COMMIT = Get-Content -Path $LAST_UPDATE_FILE

        if ($CURRENT_COMMIT -eq $LAST_COMMIT) {
            Write-Host "Submodules already up-to-date."
            $UPDATE_SUBMODULES = $False
        }
        else {
            Write-Host "Submodules were last updated at commit: $LAST_COMMIT"
        }
    }

    if ($UPDATE_SUBMODULES) {
        Write-Host "Updating submodules"

        git submodule init
        git submodule update

        Out-File ( New-Item -Path $LAST_UPDATE_FILE -Force) -InputObject $CURRENT_COMMIT
    }
}

$CMAKE_ARGS = @("-S", "$PSScriptRoot")

if ($NoUnityBuild) {
    $CMAKE_ARGS += "-DNS_ENABLE_FOLDER_UNITY_FILES:BOOL=OFF"
}
else {
    $CMAKE_ARGS += "-DNS_ENABLE_FOLDER_UNITY_FILES:BOOL=ON"
}

if ($SolutionName -ne "") {
    $CMAKE_ARGS += "-DNS_SOLUTION_NAME:STRING='${SolutionName}'"
}

$CMAKE_ARGS += "-G"

Write-Host ""

if ($Target -eq "Win64vs2019") {

    Write-Host "=== Generating Solution for Visual Studio 2019 x64 ==="

    $CMAKE_ARGS += "Visual Studio 16 2019"
    $CMAKE_ARGS += "-A"
    $CMAKE_ARGS += "x64"
    $CMAKE_ARGS += "-B"
    $CMAKE_ARGS += "$PSScriptRoot\Workspace\vs2019x64"
}
elseif($Target -eq "Prospero"){
    Write-Host "=== Generating Solution for Prospero ==="

    $CMAKE_ARGS += "Visual Studio 17 2022"
    
    $CMAKE_ARGS += "-DCMAKE_TOOLCHAIN_FILE=D:\ConsoleTools\Prospero\Tools\CMake\PS5.cmake"
    $CMAKE_ARGS += "-A Prospero"
    $CMAKE_ARGS += "-DNS_ENABLE_QT_SUPPORT:BOOL=OFF"
    $CMAKE_ARGS += "-DNS_ENABLE_FOLDER_UNITY_FILES:BOOL=OFF"
    $CMAKE_ARGS += "-DNS_3RDPARTY_DUKTAPE_SUPPORT=OFF"
    $CMAKE_ARGS += "-DNS_3RDPARTY_ZSTD_SUPPORT=ON"
    $CMAKE_ARGS += "-DNS_3RDPARTY_LUA_SUPPORT=OFF"
    $CMAKE_ARGS += "x64"
    $CMAKE_ARGS += "-B"
    $CMAKE_ARGS += "$PSScriptRoot\Workspace\Prospero"
}
elseif ($Target -eq "Uwp64vs2019") {

    Write-Host "=== Generating Solution for Visual Studio 2019 x64 UWP ==="
    
    $CMAKE_ARGS += "Visual Studio 16 2019"
    $CMAKE_ARGS += "-A"
    $CMAKE_ARGS += "x64"
    $CMAKE_ARGS += "-B"
    $CMAKE_ARGS += "$PSScriptRoot\Workspace\vs2019x64uwp"
    $CMAKE_ARGS += "-DCMAKE_TOOLCHAIN_FILE=$PSScriptRoot\Code\BuildSystem\CMake\toolchain-winstore.cmake"

    $CMAKE_ARGS += "-DNS_ENABLE_QT_SUPPORT:BOOL=OFF"
    $CMAKE_ARGS += "-DNS_BUILD_FILTER='UwpProjects'"
}
elseif ($Target -eq "Win64vs2022") {

    Write-Host "=== Generating Solution for Visual Studio 2022 x64 ==="

    $CMAKE_ARGS += "Visual Studio 17 2022"
    $CMAKE_ARGS += "-DNS_BUILD_FMOD:BOOL=OFF"
    $CMAKE_ARGS += "-A"
    $CMAKE_ARGS += "x64"
    $CMAKE_ARGS += "-B"
    $CMAKE_ARGS += "$PSScriptRoot\Workspace\vs2022x64"
}
elseif ($Target -eq "Uwp64vs2022") {

    Write-Host "=== Generating Solution for Visual Studio 2022 x64 UWP ==="

    $CMAKE_ARGS += "Visual Studio 17 2022"
    $CMAKE_ARGS += "-A"
    $CMAKE_ARGS += "x64"
    $CMAKE_ARGS += "-B"
    $CMAKE_ARGS += "$PSScriptRoot\Workspace\vs2022x64uwp"
    $CMAKE_ARGS += "-DCMAKE_TOOLCHAIN_FILE=$PSScriptRoot\Code\BuildSystem\CMake\toolchain-winstore.cmake"

    $CMAKE_ARGS += "-DNS_ENABLE_QT_SUPPORT:BOOL=OFF"
    $CMAKE_ARGS += "-DNS_BUILD_FILTER='UwpProjects'"
}
else {
    throw "Unknown target '$Target'."
}

Write-Host ""
Write-Host "Running cmake.exe $CMAKE_ARGS"
Write-Host ""
if (Get-Command "cmake.exe" -ErrorAction SilentlyContinue)
{
	Write-Host "Using cmake in path"
	&cmake.exe $CMAKE_ARGS
}
else
{
	Write-Host "Using precompiled cmake"
	&Data\Tools\Precompiled\cmake\bin\cmake.exe $CMAKE_ARGS
}

if (!$?) {
    throw "CMake failed with exit code '$LASTEXITCODE'."
}
