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

#ifndef _CCAR_H_
#define _CCAR_H_



namespace MafiaSDK
{
	struct C_Car_Interface
	{
		C_Entity_Interface		 entity;
		PADDING(C_Car_Interface, _pad0, 0x4);
		C_Vehicle_Interface	vehicle_interface;
	};

	namespace C_Car_Enum
	{
		enum FunctionsAddresses
		{
			Engine = 0x004E1CE0,
			SetEngineOn = 0x004CB5B0,
			SetGear = 0x004CB070,
			GearSnd = 0x004ED810,
			GetSeatProperty = 0x0041DC30,
			GetOwner = 0x0041DEC0,
			Update = 0x0041FAC0,
			PrepareDropOutWheel = 0x00426DD0,
			PrepareDropOut = 0x00426EC0,
			CarExplosion = 0x00421D60
		};
	};

	class C_Car : public C_Actor
	{
	public:
		C_Car_Interface * GetInterface()
		{
			return reinterpret_cast<C_Car_Interface*>(this);
		}

		void Update(float dt)
		{
			unsigned long functionAddress = C_Car_Enum::FunctionsAddresses::Update;

			__asm
			{
				push dt
				mov ecx, this
				call functionAddress
			}
		}

		void AI(float dt)
		{
			__asm
			{
				mov ecx, this
				mov eax, dword ptr ds : [ecx]
				push dt
				call dword ptr ds : [eax + 0x34]
			}
		}

		void GetSeatProperty(int seat_idx, bool & unk1, bool & unk2, bool & unk3, bool & unk4)
		{
			unsigned long functionAddress = C_Car_Enum::FunctionsAddresses::GetSeatProperty;

			__asm
			{
				push unk4
				push unk3
				push unk2
				push unk1
				push seat_idx
				mov ecx, this
				call functionAddress
			}
		}

		/* --- C_Vehicle Function here :) --- */
		void SetEngineOn(BOOL arg1, BOOL arg2)
		{
			unsigned long functionAddress = C_Car_Enum::FunctionsAddresses::SetEngineOn;
			__asm
			{
				mov ecx, this
				add ecx, 0x70

				push arg2
				push arg1
				call functionAddress
			}
		}

		void SetGear(int gear)
		{
			unsigned long functionAddress = C_Car_Enum::FunctionsAddresses::SetGear;
			__asm
			{
				mov ecx, this
				add ecx, 0x70
				push gear
				call functionAddress
			}
		}

		void GearSnd()
		{
			unsigned long functionAddress = C_Car_Enum::FunctionsAddresses::GearSnd;
			__asm
			{
				mov ecx, this
				add ecx, 0x70
				call functionAddress
			}
		}


		void Engine(float arg1, float arg2, float arg3)
		{
			unsigned long functionAddress = C_Car_Enum::FunctionsAddresses::Engine;

			__asm
			{
				mov ecx, this
				add ecx, 0x70

				push arg3
				push arg2
				push arg1
				call functionAddress
			}
		}

		/* Just our custom function why not*/
		void UpdatePos(const Vector3D & pos, const Vector3D & rot)
		{
			//Get state of actor and if actor is in sleeping mode or non active 
			//We make the actor as an active one
			int vehicle_state = *(int*)((DWORD)this + 0x1C);
			if (vehicle_state)
			{
				this->SetActState(0);
			}

			//this->GetInterface()->position = pos;
			//this->GetInterface()->rotation = rot;
		}

		unsigned long GetOwner(int seat_idx)
		{
			unsigned long functionAddress = C_Car_Enum::FunctionsAddresses::GetOwner;

			__asm
			{
				push seat_idx
				mov ecx, this
				call functionAddress
			}
		}

		void Prepare_Drop_Out_Wheel(unsigned int wheel_idx, const Vector3D & speed, const Vector3D & unk)
		{
			unsigned long functionAddress = C_Car_Enum::FunctionsAddresses::PrepareDropOutWheel;

			__asm {
				push unk
				push speed
				mov edx, wheel_idx
				mov ecx, this
				call functionAddress
			}
		}

		void Prepare_Drop_Out(unsigned int comp_idx, const Vector3D & speed, const Vector3D & unk)
		{
			unsigned long functionAddress = C_Car_Enum::FunctionsAddresses::PrepareDropOut;

			__asm {
				push unk
				push speed
				mov edx, comp_idx
				mov ecx, this
				call functionAddress
			}
		}

		void CarExplosion(unsigned int tick_time)
		{
			unsigned long functionAddress = C_Car_Enum::FunctionsAddresses::CarExplosion;

			__asm {
				push tick_time
				mov ecx, this
				call functionAddress
			}
		}
	};
};

#endif