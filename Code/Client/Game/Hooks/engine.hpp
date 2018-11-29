#pragma once
namespace hooks
{
    namespace engine
    {

        /*
        * We wait till engine is initialize then we hook and init our mod
        */
        auto OnGameInited() {
            init_config();
            mod_init_game();
            mod_init_networking();
        }
        
        __declspec(naked) void GameInited() {
            __asm {
                pushad
                    call OnGameInited
                popad

                retn
            }
        }

        auto init() {
            MemoryPatcher::InstallJmpHook(0x005BFE39, (DWORD)&GameInited);
        }
    }
};
