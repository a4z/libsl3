<#
.SYNOPSIS
Build, test, install, and consume libsl3 as a Windows shared DLL.

.DESCRIPTION
Run from the repository root with .\testdll.ps1. By default the script uses
MSVC 2022, the Release configuration, fetched dependencies, and internal
SQLite. Build directories are deleted before configuration.

The script builds the shared DLL, runs the in-tree CTest suite against it,
installs the package, builds tests/pkg-consumer against the install, and runs
that consumer test with the installed DLL on PATH.

.EXAMPLE
.\testdll.ps1 -KeepBuild

.EXAMPLE
.\testdll.ps1 -Config Debug

.EXAMPLE
.\testdll.ps1 -Config RelWithDebInfo -Msvc 26
#>

param(
    [Alias("MsvcVersion")]
    [ValidateSet("22", "26")]
    [string]$Msvc = "22",

    [Alias("Configuration")]
    [ValidateSet("Debug", "Release", "RelWithDebInfo", "ReleaseDebug")]
    [string]$Config = "Release",

    [switch]$KeepBuild
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Invoke-Native {
    param(
        [string]$FilePath,
        [string[]]$Arguments
    )

    Write-Host ""
    Write-Host "> $FilePath $($Arguments -join ' ')"
    & $FilePath @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "$FilePath failed with exit code $LASTEXITCODE"
    }
}

function Remove-BuildDirectory {
    param(
        [string]$Path,
        [string]$BuildRoot
    )

    $fullPath = [System.IO.Path]::GetFullPath($Path)
    $fullBuildRoot = [System.IO.Path]::GetFullPath($BuildRoot)
    if (-not $fullPath.StartsWith($fullBuildRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to remove path outside build root: $fullPath"
    }

    if (Test-Path -LiteralPath $fullPath) {
        Write-Host ""
        Write-Host "> Remove-Item -Recurse -Force $fullPath"
        Remove-Item -LiteralPath $fullPath -Recurse -Force
    }
}

$RepoRoot = $PSScriptRoot
$ConfigurePreset = "msvc$Msvc"
$CmakeConfig = if ($Config -eq "ReleaseDebug") {
    "RelWithDebInfo"
} else {
    $Config
}

$BuildRoot = Join-Path $RepoRoot "build\Windows"
$BuildDir = Join-Path $BuildRoot $ConfigurePreset
$InstallPrefix = Join-Path $BuildDir "inst"
$ConsumerBuildDir = Join-Path $BuildRoot "pkg-consumer-$ConfigurePreset-$CmakeConfig"
$BuiltDllDir = Join-Path $BuildDir $CmakeConfig
$InstalledDllDir = Join-Path $InstallPrefix "bin"

$ConsumerGenerator = switch ($ConfigurePreset) {
    "msvc22" { "Visual Studio 17 2022" }
    "msvc26" { "Visual Studio 18 2026" }
    default { "" }
}

$oldPath = $env:PATH
try {
    if (-not $KeepBuild) {
        Remove-BuildDirectory -Path $BuildDir -BuildRoot $BuildRoot
        Remove-BuildDirectory -Path $ConsumerBuildDir -BuildRoot $BuildRoot
    }

    Invoke-Native cmake @(
        "--preset", $ConfigurePreset,
        "-DTOOLCHAIN_INCLUDES=toolchain/fetch-dependencies",
        "-DBUILD_SHARED_LIBS=ON",
        "-Dsl3_BUILD_TESTING=ON",
        "-Dsl3_USE_INTERNAL_SQLITE3=ON"
    )
    Invoke-Native cmake @(
        "--build", $BuildDir,
        "--config", $CmakeConfig,
        "--parallel"
    )

    $env:PATH = "$BuiltDllDir;$env:PATH"
    Invoke-Native ctest @(
        "--test-dir", $BuildDir,
        "-C", $CmakeConfig,
        "--output-on-failure"
    )

    Invoke-Native cmake @(
        "--install", $BuildDir,
        "--config", $CmakeConfig,
        "--prefix", $InstallPrefix
    )

    $consumerArgs = @(
        "-S", (Join-Path $RepoRoot "tests\pkg-consumer"),
        "-B", $ConsumerBuildDir,
        "-DCMAKE_PREFIX_PATH=$InstallPrefix"
    )
    if ($ConsumerGenerator -ne "") {
        $consumerArgs += @("-G", $ConsumerGenerator)
        if ($ConsumerGenerator.StartsWith("Visual Studio")) {
            $consumerArgs += @("-A", "x64")
        }
    }

    $env:PATH = "$InstalledDllDir;$env:PATH"
    Invoke-Native cmake $consumerArgs
    Invoke-Native cmake @(
        "--build", $ConsumerBuildDir,
        "--config", $CmakeConfig,
        "--parallel"
    )
    Invoke-Native ctest @(
        "--test-dir", $ConsumerBuildDir,
        "-C", $CmakeConfig,
        "--output-on-failure"
    )
} finally {
    $env:PATH = $oldPath
}
