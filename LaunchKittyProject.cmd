@echo off
setlocal

set "KITTY_UE_EDITOR=C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"
set "KITTY_PROJECT=%~dp0KittyProject.uproject"

if not exist "%KITTY_UE_EDITOR%" (
    echo Unreal Engine 5.8 was not found:
    echo %KITTY_UE_EDITOR%
    pause
    exit /b 1
)

if not exist "%KITTY_PROJECT%" (
    echo KittyProject.uproject was not found:
    echo %KITTY_PROJECT%
    pause
    exit /b 1
)

start "" "%KITTY_UE_EDITOR%" "%KITTY_PROJECT%" -NoLiveCoding

endlocal
