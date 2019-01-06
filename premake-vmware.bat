@echo off
pushd \\vmware-host\Shared Folders\mafia\Oakwood
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
popd