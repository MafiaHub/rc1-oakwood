@echo off
setlocal enabledelayedexpansion

if "%VS_SET%"=="" ( 
    echo Registering msdev variables...
    call ..\Scripts\vs17.bat
    set VS_SET="1"
) else (
    echo Msdev variables found!
)

msbuild /t:Rebuild /p:Configuration=Debug