#/bin/bash 2>nul || goto :windows

unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     machine=linux;;
    Darwin*)    machine=macos;;
    MINGW*)     machine=mingw;;
    *)          machine="unknown"
esac

if [[ $1 == "clean" ]]
then
    # Invoke premake5's clean action
    echo "Performing clean action..."
    Tools/premake/bin/{$machine}/premake5 clean --file=Code/premake5.lua
else
    Tools/premake/bin/${machine}/premake5 gmake2 "${@}" --file=Code/premake5.lua
fi

exit

:windows
@echo off

if "%1" == "clean" (
    :: Invoke premake5's clean action
    echo Performing clean action...
    Tools\premake\bin\win32\premake5 clean --file=Code/premake5.lua
) else (
    :: Invoke premake5 with specified args and VS2017 action
    Tools\premake\bin\win32\premake5 %* vs2017 --file=Code/premake5.lua
)

:: Pause for 5 seconds and auto-close the command window
:end
timeout /t 3 /nobreak
