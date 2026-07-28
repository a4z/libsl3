# DLL Smoke Test

Run from the repository root in PowerShell:

```powershell
.\testdll.ps1
```

Defaults:

- MSVC version: `22`
- configuration: `Release`
- dependencies: `TOOLCHAIN_INCLUDES=toolchain/fetch-dependencies`
- SQLite: internal
- build dirs are deleted before configure

Useful variants:

```powershell
.\testdll.ps1 -KeepBuild
.\testdll.ps1 -Config Debug
.\testdll.ps1 -Config RelWithDebInfo
.\testdll.ps1 -Msvc 26
```

The script builds the shared DLL, runs the in-tree CTest suite against it,
installs the package, builds `tests/pkg-consumer` against the install, and runs
that consumer test with the installed DLL on `PATH`. The consumer build
directory includes both compiler and config, for example
`build\Windows\pkg-consumer-msvc22-Release`.
