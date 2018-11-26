#ifndef _MAFIA_SDK_
#define _MAFIA_SDK_

#include <Windows.h>
#include <DSound.h>
#include <d3d9.h>
#include <functional>

#define byte BYTE
#include "Utils/Helpers.hpp"
#include "Utils/MemoryPatcher.h"

#include "Engine/Math.hpp"
#include "Engine/ITexture.hpp"
#include "Engine/I3D_Frame.hpp"
#include "Engine/I3D_Projector.hpp"
#include "Engine/I3D_Light.hpp"
#include "Engine/ISND_Source.hpp"
#include "Engine/I3D_Sound.hpp"
#include "Engine/I3D_Sector.hpp"
#include "Engine/IGraph.hpp"
#include "Engine/I3D_Driver.hpp"
#include "Engine/I3D_Mesh_Object.hpp"
#include "Engine/I3D_Mesh_Level.hpp"
//#include "Engine/DirectHook.hpp"

namespace MafiaSDK {
	class C_Actor;
}

#include "Game/ai_action_manager.hpp"
#include "Game/ai_follow_manager.hpp"

#include "Game/C_Entity.hpp"
#include "Game/C_Actor.hpp"
#include "Game/C_Vehicle.hpp"
#include "Game/C_Car.hpp"
#include "Game/C_Human.hpp"
#include "Game/C_Player.hpp"
#include "Game/C_Camera.hpp"
#include "Game/C_Indicators.hpp"
#include "Game/GM_Menu.hpp"
#include "Game/C_Blood.hpp"
#include "Game/C_BloodManager.hpp"
#include "Game/C_Game.hpp"
#include "Game/C_Bottle.hpp"
#include "Game/C_Program.hpp"
#include "Game/C_Detector.hpp"
#include "Game/C_Mission.hpp"
#include "Game/C_DropInWeapon.hpp"

#endif
