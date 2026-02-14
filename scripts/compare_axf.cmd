@echo off
setlocal

if "%~2"=="" (
  echo Usage: compare_axf.cmd old.axf new.axf [output_dir]
  exit /b 1
)

set OLD=%~1
set NEW=%~2
set OUT=%~3

if "%OUT%"=="" (
  powershell -ExecutionPolicy Bypass -File "%~dp0compare_axf.ps1" -OldAxf "%OLD%" -NewAxf "%NEW%"
) else (
  powershell -ExecutionPolicy Bypass -File "%~dp0compare_axf.ps1" -OldAxf "%OLD%" -NewAxf "%NEW%" -OutputDir "%OUT%"
)

endlocal
