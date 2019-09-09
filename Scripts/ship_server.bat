@echo off

set op=%CD%
set pr=%~dp0\..
set build=Production
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

mkdir Server\static
echo f|xcopy /S /Q %pr%\Files\static Server\static\

mkdir Server\resources
echo f|xcopy /s /q %pr%\Resources Server\resources

popd

cd %op%
