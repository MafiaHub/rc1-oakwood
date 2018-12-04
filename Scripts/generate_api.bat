@echo off

set op=%CD%
cd %~dp0

pushd ..

echo Generating API...

echo vtable...
Tools\oakgen generate_vtable Code\Server\natives.hpp > Code\Shared\mod_api_vtable.generated.hpp

echo types...
Tools\oakgen generate_types Code\Server\natives.hpp > Code\Shared\mod_api_types.generated.hpp

echo assign...
Tools\oakgen generate_assign Code\Server\natives.hpp > Code\Server\natives.generated.hpp

echo API generated.
echo.

popd

node generate_lua_bindings.js

cd %op%