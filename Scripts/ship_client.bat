@echo off

set op=%CD%
set pr=%~dp0\..
set build=Release
if not [%1] == [] then set build=%1
cd %~dp0

pushd ..

echo Shipping Client files...

if not exist Shipping mkdir Shipping
cd Shipping

if exist Client rmdir /Q /S Client
mkdir Client
echo f|xcopy %pr%\Bin\%build%\Oakwood.exe Client\Oakwood.exe
echo f|xcopy %pr%\Bin\%build%\OakwoodClient.dll Client\OakwoodClient.dll

mkdir Client\config
echo f|xcopy %pr%\Files\config\Client.json Client\config\Client.json

mkdir Client\bin
echo f|xcopy %pr%\Files\bin\*.* Client\bin\*.*

echo f|xcopy /e %pr%\Files\launcher\*.* Client\*.*

mkdir Client\files
echo f|xcopy %pr%\Bin\%build%\files\*.* Client\files\

popd

cd %op%