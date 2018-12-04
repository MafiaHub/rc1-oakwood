@echo off

pushd Build
msbuild Oakwood.sln /p:Configuration=Release /maxcpucount:8
popd