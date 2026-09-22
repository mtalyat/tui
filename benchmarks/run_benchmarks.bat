@echo off

:: Get path to executable (release version for speed)
set "TUI_EXE=%~dp0..\interpreter\windows\tui\x64\Release\tui.exe"

:: Run benchmarks
pushd %~dp0
%TUI_EXE% "%~dp0benchmarks.tui"
popd