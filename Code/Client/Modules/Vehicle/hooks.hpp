//----------------------------------------------
//C_car::Prepare_DropOut_Wheel
//----------------------------------------------
typedef bool(__fastcall* C_car_Prepare_DropOut_Wheel_t)(void* _this, int idx, const S_vector& speed, const S_vector* unk);
C_car_Prepare_DropOut_Wheel_t car_prepare_dropout_wheel_original = nullptr;
bool __fastcall C_car_Prepare_DropOut_Wheel(void* _this, int idx, const S_vector& speed, const S_vector* unk) {

	auto vehicle_ent = modules::player::get_vehicle_from_base((void*)_this);
	if (!vehicle_ent) return false;

	zpl_vec3 send_speed = EXPAND_VEC(speed);
	zpl_vec3 send_unk = { 0.0f, 0.0f, 0.0f };

	if (unk) send_unk = EXPAND_VEC((*unk));

	librg_send(&network_context, NETWORK_VEHICLE_WHEEL_DROPOUT, data, {
		librg_data_wu32(&data, vehicle_ent->id);
		librg_data_wu32(&data, idx);
		librg_data_wptr(&data, (void*)&send_speed, sizeof(zpl_vec3));
		librg_data_wptr(&data, (void*)&send_unk, sizeof(zpl_vec3));
	});

	return false;
}

//----------------------------------------------
//C_car::Prepare_DropOut
//----------------------------------------------
typedef bool(__fastcall* C_car_Prepare_DropOut_t)(void* _this, int idx, const S_vector& speed, const S_vector* unk);
C_car_Prepare_DropOut_Wheel_t car_prepare_dropout_original = nullptr;
bool __fastcall C_car_Prepare_DropOut(void* _this, int idx, const S_vector& speed, const S_vector* unk) {

	auto vehicle_ent = modules::player::get_vehicle_from_base((void*)_this);
	if (!vehicle_ent) return false;

	auto car = (MafiaSDK::C_Car*)(_this);
	auto component = car->GetCarComponent(idx);

	if (!car || !component) return false;
	auto check = *((unsigned __int16 *)component + 6);
	if (check == 2 ||
		check == 4 ||
		check == 5 ||
		check == 6 ||
		check == 7 ||
		check == 8) {
		zpl_vec3 send_speed = EXPAND_VEC(speed);
		zpl_vec3 send_unk = { 0.0f, 0.0f, 0.0f };

		if (unk) send_unk = EXPAND_VEC((*unk));

		librg_send(&network_context, NETWORK_VEHICLE_COMPONENT_DROPOUT, data, {
			librg_data_wu32(&data, vehicle_ent->id);
			librg_data_wu32(&data, idx);
			librg_data_wptr(&data, (void*)&send_speed, sizeof(zpl_vec3));
			librg_data_wptr(&data, (void*)&send_unk, sizeof(zpl_vec3));
		});

		return true;
	}

	return false;
}


//----------------------------------------------
//C_car::CarExplosion
//----------------------------------------------
typedef bool(__thiscall* C_car_CarExplosion_t)(void* _this, unsigned int tick);
C_car_CarExplosion_t c_car_carexplosion_original = nullptr;

bool __fastcall C_car_CarExplosion(void* _this, DWORD edx, unsigned int tick) {
	
	auto vehicle_ent = modules::player::get_vehicle_from_base((void*)_this);
	if (!vehicle_ent) return false;

	auto vehicle = (mafia_vehicle*)vehicle_ent->user_data;

	//NOTE(DavoSK): prevents spamming network when vehicle is not yet exploded
	if (vehicle && !vehicle->wants_explode) {
		librg_send(&network_context, NETWORK_VEHICLE_EXPLODE, data, {
			librg_data_wu32(&data, vehicle_ent->id);
		});
		vehicle->wants_explode = true;
	}

	return false;
}

//----------------------------------------------
//C_Vehicle::Deform((S_vector const &,S_vector const &,float,float,uint,S_vector const *))
//----------------------------------------------
typedef bool(__thiscall* C_Vehicle_Deform_t)(void* _this, const S_vector& unk1, const S_vector& unk2, float unk3, float unk4, unsigned int unk5, S_vector* unk6);
C_Vehicle_Deform_t c_vehicle_deform_original = nullptr;
bool __fastcall C_Vehicle_Deform(void* _this,
	DWORD edx,
	const S_vector & pos,
	const S_vector & rot,
	float unk1,
	float unk2,
	unsigned int unk3,
	S_vector* unk4) {

	if (!_this) return false;

	//NOTE(DavoSK): Get car from C_Vehicle, be carefull if is something else RIP, CRASH, CRY
	MafiaSDK::C_Car* current_car = reinterpret_cast<MafiaSDK::C_Car*>((char*)_this - 0x70);
	if (!current_car) return false;

	auto vehicle_ent = modules::player::get_vehicle_from_base(current_car);
	if (!vehicle_ent) return false;

	//GET current vertices from car
	struct mesh_data {
		std::vector<MafiaSDK::I3D_vertex_mesh> vertices;
	};

	auto get_mesh_data = [](MafiaSDK::C_Car* car) {
		std::vector<mesh_data> car_before;
		car->EnumerateVehicleMeshes([&](MafiaSDK::I3D_mesh_object* mesh) {
			auto lod = mesh->GetLOD(0);
			if (lod) {
				auto vertices = lod->LockVertices(0);
				if (vertices) {
					mesh_data new_mesh;
					MafiaSDK::I3D_stats_mesh mesh_info;
					lod->GetStats(mesh_info);

					for (int i = 0; i < mesh_info.vertex_count; i++) {
						new_mesh.vertices.push_back(vertices[i]);
					}

					lod->UnlockVertices();
					car_before.push_back(new_mesh);
				}
			}
		});

		return car_before;
	};

	auto data_before = get_mesh_data(current_car);
	auto result      = c_vehicle_deform_original(_this, pos, rot, unk1, unk2, unk3, unk4);
	auto data_after  = get_mesh_data(current_car);

	// NOTE(DavoSK): Now after deformating vehicle we need to compare 
	// each vertex data to get delta 
	std::vector<mafia_vehicle_deform> deform_deltas;

	for (u32 i = 0; i < data_before.size(); i++) {
		auto before_mesh = data_before.at(i);
		auto after_mesh = data_after.at(i);

		for (u32 j = 0; j < before_mesh.vertices.size(); j++) {
			auto before_vertex = before_mesh.vertices.at(j);
			auto after_vertex = after_mesh.vertices.at(j);

			// NOTE(DavoSK): Compare vertex
			if (before_vertex.n.x != after_vertex.n.x ||
				before_vertex.n.y != after_vertex.n.y ||
				before_vertex.n.z != after_vertex.n.z ||

				before_vertex.p.x != after_vertex.p.x ||
				before_vertex.p.y != after_vertex.p.y ||
				before_vertex.p.z != after_vertex.p.z) {
				
				deform_deltas.push_back({
					i,
					j,
					EXPAND_VEC(after_vertex.n),
					EXPAND_VEC(after_vertex.p)
				});
			}
		}
	}

	if (deform_deltas.size()) {
		librg_send(&network_context, NETWORK_VEHICLE_DEFORM_DELTA, data, {
			librg_data_wu32(&data, vehicle_ent->id);
			librg_data_wu32(&data, deform_deltas.size());
			librg_data_wptr(&data, deform_deltas.data(), sizeof(mafia_vehicle_deform) * deform_deltas.size());
		});
	}

	return result;
}

inline auto init() {
	
    /*car_prepare_dropout_wheel_original = reinterpret_cast<C_car_Prepare_DropOut_Wheel_t>(
        DetourFunction((PBYTE)0x00426DD0, (PBYTE)&C_car_Prepare_DropOut_Wheel)
    );

    car_prepare_dropout_original = reinterpret_cast<C_car_Prepare_DropOut_t>(
        DetourFunction((PBYTE)0x00426EC0, (PBYTE)&C_car_Prepare_DropOut)
    );
	
    //Deform
    c_vehicle_deform_original = reinterpret_cast<C_Vehicle_Deform_t>(
        DetourFunction((PBYTE)0x004D5610, (PBYTE)&C_Vehicle_Deform)
    );*/

    c_car_carexplosion_original = reinterpret_cast<C_car_CarExplosion_t>(
        DetourFunction((PBYTE)0x00421D60, (PBYTE)&C_car_CarExplosion)
    );
}