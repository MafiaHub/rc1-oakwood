/*
	Copyright 2017 D�vid Svitana

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

#ifndef _CVEHICLE_H_
#define _CVEHICLE_H_

namespace MafiaSDK
{
	struct C_Vehicle_Interface {
		PADDING(C_Vehicle_Interface, _pad0, 0x1B4);
		float engine_health;
		PADDING(C_Vehicle_Interface, _pad1, 0x34);
		float health;
		PADDING(C_Vehicle_Interface, _pad2, 0x130);
		Vector3D position;
		PADDING(C_Vehicle_Interface, _pad3, 0x104);
		byte horn;
		byte siren;
		byte sound_enabled;
		float hand_break;
		PADDING(C_Vehicle_Interface, _pad7, 0x6C);
		float speed_limit;
		PADDING(C_Vehicle_Interface, _pad8, 0x98);
		float accelerating;
		PADDING(C_Vehicle_Interface, _pad9, 0x4);
		float engine_rpm;
		PADDING(C_Vehicle_Interface, _pad10, 0x14);
		DWORD gear;
		PADDING(C_Vehicle_Interface, _pad11, 0x54);
		float break_val;
		PADDING(C_Vehicle_Interface, _pad12, 0x24);
		float clutch;
		PADDING(C_Vehicle_Interface, _pad13, 0x40);
		float wheel_angle;
		PADDING(C_Vehicle_Interface, _pad14, 0x604);
		byte engine_on;
		PADDING(C_Vehicle_Interface, _pad15, 0x3);
		float fuel;
		PADDING(C_Vehicle_Interface, _pad16, 0x8);
		Vector3D rotation;
		PADDING(C_Vehicle_Interface, _pad17, 0xC);
		Vector3D rotation_second;
		PADDING(C_Vehicle_Interface, _pad18, 0x1330);
		Vector3D speed;
	};

	namespace C_Vehicle_Enum
	{
		enum FunctionsAddresses
		{

		};
	};
	
	class C_Vehicle
	{
	public:
		C_Vehicle_Interface* GetInterface()
		{
			return reinterpret_cast<C_Vehicle_Interface*>(this);
		}
	};
};

#endif