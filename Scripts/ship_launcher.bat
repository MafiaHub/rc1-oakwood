@echo off

set op=%CD%
set pr=%~dp0\..
cd %~dp0

pushd ..

echo Shipping Launcher files...

if not exist Shipping mkdir Shipping
cd Shipping

if exist Launcher rmdir /Q /S Launcher
mkdir Launcher

echo f|xcopy /e %pr%\Files\launcher\*.* Launcher\*.*

popd

cd %op%
