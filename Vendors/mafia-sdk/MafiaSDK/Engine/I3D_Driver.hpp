/*
Copyright 2018 Dávid Svitana

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef _I3D_Driver_H_
#define _I3D_Driver_H_

namespace MafiaSDK
{
    constexpr unsigned long I3D_Driver_Const = 0x101C5298;
	namespace I3D_Driver_Enum
	{
		enum FunctionsAddresses
		{
			Construct = 0x100201F0,
			Destruct = 0x10020760,

		};
	
        enum FrameType
        {
            LIGHT = 2,
            CAMERA, 
            SOUND,
            SECTOR,
            DUMMY,
            TARGET,
            USER,
            MODEL,
            JOINT,
            VOLUME,
            OCCLUDER,
            SCENE,
            AREA,
            SHADOW,
            LANDSACPE,
            EMITOR
        };
    };

	class I3D_Driver
	{
	public:
        template<typename T>
		T* CreateFrame(I3D_Driver_Enum::FrameType frame_type)
		{
			__asm
			{
				mov edi, this
				mov eax, dword ptr ds : [edi]
				push frame_type
				push edi
				call dword ptr ds : [eax + 0x50]
			}
		}
	};

    auto I3DGetDriver() 
    {
        return *(I3D_Driver**)(I3D_Driver_Const);
    } 
};

#endif
