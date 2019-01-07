@echo off

set op=%CD%
cd %~dp0
tools\premake\win32\premake5 %* vs2017 --file=plugins/premake5.lua
cd %op%
timeout /t 3 /nobreak