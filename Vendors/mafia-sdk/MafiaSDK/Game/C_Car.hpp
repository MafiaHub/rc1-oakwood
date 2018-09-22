/*
	Copyright 2017 Dávid Svitana

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
		PADDING(C_Car_Interface, _pad0, 0x1B8);
		float					engineHealth;
		PADDING(C_Car_Interface, _pad1, 0x168);
		Vector3D				position;
		PADDING(C_Car_Interface, _pad2, 0xF4);
		float					gasState;
		PADDING(C_Car_Interface, _pad3, 0x88);
		bool					hornState;
		bool					sirenState;
		PADDING(C_Car_Interface, _pad4, 0x12A);
		int						speedGear;
		PADDING(C_Car_Interface, _pad5, 0xC4);
		float					wheelRotation;
		PADDING(C_Car_Interface, _pad6, 0x598);
		Vector3D				rotation;
		PADDING(C_Car_Interface, _pad7, 0x1A56);
	};

	namespace C_Car_Enum
	{
		enum FunctionsAddresses
		{
			Engine = 0x004E1CE0,
			GetSeatProperty = 0x0041DC30,
			GetOwner = 0x0041DEC0
		};
	};

	class C_Car : public C_Actor
	{
	public:
		C_Car_Interface* GetInterface()
		{
			return reinterpret_cast<C_Car_Interface*>(this);
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

			this->GetInterface()->position = pos;
			this->GetInterface()->rotation = rot;
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
	};
};

#endif