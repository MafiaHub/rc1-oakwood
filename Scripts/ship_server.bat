@echo off

set op=%CD%
set pr=%~dp0\..
set build=Debug
if not [%1] == [] then set build=%1
cd %~dp0

pushd ..

echo Shipping Server files...

if not exist Shipping mkdir Shipping
cd Shipping

if exist Server rmdir /Q /S Server
mkdir Server
echo f|xcopy %pr%\Bin\%build%\OakwoodServer.exe Server\Server.exe

mkdir Server\config
echo f|xcopy %pr%\Files\config\server.json Server\config\server.json

mkdir Server\plugins
echo f|xcopy %pr%\Bin\%build%\plugins\*.* Server\plugins\

mkdir Server\static
echo f|xcopy /S /Q %pr%\Files\static Server\static\

mkdir Server\modding
pushd Server\modding
echo f|xcopy %pr%\Scripts\plugin_generate.bat plugin_generate.bat
mkdir plugins
echo f|xcopy /E %pr%\Code\Plugins plugins\

mkdir shared
echo f|xcopy /E %pr%\Code\Shared shared\

mkdir vendors
mkdir vendors\librg
echo f|xcopy /E %pr%\Vendors\librg vendors\librg\

mkdir tools
mkdir tools\premake
echo f|xcopy /E %pr%\Tools\premake\bin tools\premake\

popd
popd

cd %op%