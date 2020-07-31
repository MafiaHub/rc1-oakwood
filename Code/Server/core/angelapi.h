asIScriptEngine *engine;

bool canRun = false;

std::map<std::string, asIScriptFunction*> commands;

void RegisterCommand(std::string cmdName, asIScriptFunction* cb)
{
    if (commands[cmdName])
    {
        oak_log("^F[^5ANGEL^F] Command ^A\"%s\" ^Fis already defined, replacing...^R\n", cmdName.c_str());
        commands[cmdName]->Release();
    }

    commands[cmdName] = cb;
}

int RandomNumber(int min, int max)
{
    return rand() % max + min;
}

#pragma region AngelScript Callbacks
void oak_angel_msg_callback(const asSMessageInfo* msg, void* param)
{
    const char* type = "^1Error";
    if (msg->type == asMSGTYPE_WARNING)
        type = "^BWarning";
    else if (msg->type == asMSGTYPE_INFORMATION)
        type = "^CInfo";
    oak_log("^F[^5ANGEL^F] ^A%s ^8(^A%d^7, ^A%d^8): %s^F: %s^R\n", msg->section, msg->row, msg->col, type, msg->message);
}

int oak_angel_include_callback(const char* include, const char* from, CScriptBuilder* builder, void* userParam)
{
    return builder->AddSectionFromFile(include);
}

void oak_angel_exception(asIScriptContext *ctx)
{
    asIScriptEngine* e = ctx->GetEngine();

    const asIScriptFunction* function = ctx->GetExceptionFunction();

    oak_log("^F[^9ERROR^F] ^1RUNTIME ERROR^F: %s^R\n", ctx->GetExceptionString());
    oak_log("^F\tFunction: ^A%s^R\n", function->GetDeclaration());
    oak_log("^F\tModule: ^A%s^R\n", function->GetModuleName());
    oak_log("^F\tSection: ^A%s^R\n", function->GetScriptSectionName());
    oak_log("^F\tLine: ^A%d^R\n", ctx->GetExceptionLineNumber());
}
#pragma endregion

#pragma region Functions wrapped for AngelScript usage
int oak_print(std::string msg)
{
    return oak_logn(msg.c_str(), msg.size());
}

CScriptArray* a_oak_player_list()
{
    std::vector<int> players;

    auto list = oak_player_list(0);

    for (int i = 0; i < GlobalConfig.players; i++)
    {
        players.push_back((int)list[i]);
    }

    asITypeInfo* t = engine->GetTypeInfoByDecl("array<int>");

    CScriptArray* arr;

    if (players.size() >= 1)
    {
        arr = CScriptArray::Create(t, players.size());

        for (asUINT i = 1; i < players.size(); i++)
        {
            arr->SetValue(i, &list[i]);
        }
    }
    else
    {
        arr = CScriptArray::Create(t, (asUINT)0);
    }

    return arr;
}

int a_oak_player_spawn(int player, float x, float y, float z, float heading)
{
    return oak_player_spawn(player, { x, y, z }, heading);
}

int a_oak_player_kick(int player, std::string reason)
{
    return oak_player_kick(player, reason.c_str(), reason.size());
}

int a_oak_player_playanim(int player, std::string anim)
{
    return oak_player_playanim(player, anim.c_str(), anim.size());
}

int a_oak_player_model_set(int player, std::string model)
{
    return oak_player_model_set(player, model.c_str(), model.size());
}

int a_oak_player_position_set(int player, float x, float y, float z)
{
    return oak_player_position_set(player, { x, y, z });
}

float a_oak_player_position_getx(int player)
{
    return oak_player_position_get(player).x;
}

float a_oak_player_position_gety(int player)
{
    return oak_player_position_get(player).y;
}

float a_oak_player_position_getz(int player)
{
    return oak_player_position_get(player).z;
}

int a_oak_player_direction_set(int player, float x, float y, float z)
{
    return oak_player_direction_set(player, { x, y, z });
}

float a_oak_player_direction_getx(int player)
{
    return oak_player_direction_get(player).x;
}

float a_oak_player_direction_gety(int player)
{
    return oak_player_direction_get(player).y;
}

float a_oak_player_direction_getz(int player)
{
    return oak_player_direction_get(player).z;
}

std::string a_oak_player_name_get(int player)
{
    return std::string(oak_player_name_get(player));
}

std::string a_oak_player_model_get(int player)
{
    return std::string(oak_player_model_get(player));
}

std::string a_oak_player_ip_get(int player)
{
    return std::string(oak_player_ip_get(player));
}

std::string a_oak_player_statecode_get(int player)
{
    return std::string(oak_player_statecode_get(player));
}

std::string a_oak_player_statename_get(int player)
{
    return std::string(oak_player_statename_get(player));
}

int a_oak_camera_set(int player, float posx, float posy, float posz, float dirx, float diry, float dirz)
{
    return oak_camera_set(player, { posx, posy, posz }, { dirx, diry, dirz });
}

int a_oak_hud_announce(int player, std::string text, float duration)
{
    return oak_hud_announce(player, text.c_str(), text.size(), duration);
}

int a_oak_hud_message(int player, std::string text, int color)
{
    return oak_hud_message(player, text.c_str(), text.size(), color);
}

int a_oak_dialog_show(int player, std::string title, std::string text, std::string button1, std::string button2, oak_dialog id, int type)
{
    return oak_dialog_show(player, title.c_str(), title.size(), text.c_str(), text.size(), button1.c_str(), button1.size(), button2.c_str(), button2.size(), id, type);
}

int a_oak_chat_send(int player, std::string text)
{
    return oak_chat_send(player, text.c_str(), text.size());
}

int a_oak_chat_broadcast(std::string text)
{
    return oak_chat_broadcast(text.c_str(), text.size());
}

CScriptArray* a_oak_vehicle_list()
{
    auto list = oak_vehicle_list(0);

    asITypeInfo* t = engine->GetTypeInfoByDecl("array<int>");

    CScriptArray* arr;

    if (sizeof(list) / 4 >= 1)
    {
        arr = CScriptArray::Create(t, sizeof(list) / 4);

        for (asUINT i = 1; i < sizeof(list) / 4; i++)
        {
            arr->SetValue(i, &list[i]);
        }
    }
    else
    {
        arr = CScriptArray::Create(t, (asUINT)0);
    }

    return arr;
}

int a_oak_vehicle_spawn(std::string vehName, float x, float y, float z, float heading)
{
    return oak_vehicle_spawn(vehName.c_str(), vehName.size(), { x, y, z }, heading);
}

int a_oak_vehicle_position_set(int vehicle, float x, float y, float z)
{
    return oak_vehicle_position_set(vehicle, { x, y, z });
}

float a_oak_vehicle_position_getx(int vehicle)
{
    return oak_vehicle_position_get(vehicle).x;
}

float a_oak_vehicle_position_gety(int vehicle)
{
    return oak_vehicle_position_get(vehicle).y;
}

float a_oak_vehicle_position_getz(int vehicle)
{
    return oak_vehicle_position_get(vehicle).z;
}

int a_oak_vehicle_direction_set(int vehicle, float x, float y, float z)
{
    return oak_vehicle_direction_set(vehicle, { x, y, z });
}

float a_oak_vehicle_direction_getx(int vehicle)
{
    return oak_vehicle_direction_get(vehicle).x;
}

float a_oak_vehicle_direction_gety(int vehicle)
{
    return oak_vehicle_direction_get(vehicle).y;
}

float a_oak_vehicle_direction_getz(int vehicle)
{
    return oak_vehicle_direction_get(vehicle).z;
}

int a_oak_vehicle_velocity_set(int vehicle, float x, float y, float z)
{
    return oak_vehicle_velocity_set(vehicle, { x, y, z });
}

float a_oak_vehicle_velocity_getx(int vehicle)
{
    return oak_vehicle_velocity_get(vehicle).x;
}

float a_oak_vehicle_velocity_gety(int vehicle)
{
    return oak_vehicle_velocity_get(vehicle).y;
}

float a_oak_vehicle_velocity_getz(int vehicle)
{
    return oak_vehicle_velocity_get(vehicle).z;
}

int a_oak_play_audio_stream(int player, std::string url)
{
    return oak_play_audio_stream(player, url.c_str(), url.size());
}

int a_oak_create_explosion(int player, float x, float y, float z, float radius, float force)
{
    return oak_create_explosion(player, { x, y, z }, radius, force);
}
#pragma endregion

#pragma region AngelScript events
void oak_angel_event_server_tick() {
    if (!canRun) return;

    if (engine->GetModule("OakModule") == nullptr) return;

    asIScriptContext* ctx = engine->CreateContext();

    asIScriptFunction* func = engine->GetModule("OakModule")->GetFunctionByName("onServerTick");

    if (!func) return;

    ctx->Prepare(func);

    ctx->Execute();

    ctx->Release();
}

void oak_angel_event_server_tick_second() {
    if (engine->GetModule("OakModule") == nullptr) return;

    asIScriptContext* ctx = engine->CreateContext();

    asIScriptFunction* func = engine->GetModule("OakModule")->GetFunctionByName("onServerTickSecond");

    if (!func) return;

    ctx->Prepare(func);

    ctx->Execute();

    ctx->Release();
}

void oak_angel_event_player_connect(int player) {
    if (engine->GetModule("OakModule") == nullptr) return;

    asIScriptContext* ctx = engine->CreateContext();

    asIScriptFunction* func = engine->GetModule("OakModule")->GetFunctionByName("onPlayerConnect");

    if (!func) return;

    ctx->Prepare(func);

    ctx->SetArgDWord(0, player);

    ctx->Execute();

    ctx->Release();
}

void oak_angel_event_player_disconnect(int player) {
    if (engine->GetModule("OakModule") == nullptr) return;

    asIScriptContext* ctx = engine->CreateContext();

    asIScriptFunction* func = engine->GetModule("OakModule")->GetFunctionByName("onPlayerDisconnect");

    if (!func) return;

    ctx->Prepare(func);

    ctx->SetArgDWord(0, player);

    ctx->Execute();

    ctx->Release();
}

void oak_angel_event_player_death(int player, int killer, int reason, int type, int part) {
    if (engine->GetModule("OakModule") == nullptr) return;

    asIScriptContext* ctx = engine->CreateContext();

    asIScriptFunction* func = engine->GetModule("OakModule")->GetFunctionByName("onPlayerDeath");

    oak_player_clear_inventory(player);

    if (!func) return;

    ctx->Prepare(func);

    switch (reason)
    {
    case 1:
        type = 10;
    case 2:
        type = 9;
    }

    ctx->SetArgDWord(0, player);
    ctx->SetArgDWord(1, killer);
    ctx->SetArgDWord(2, type);
    ctx->SetArgDWord(3, part);

    ctx->Execute();

    ctx->Release();
}

void oak_angel_event_player_hit(int player, int attacker, float damage) {
    if (engine->GetModule("OakModule") == nullptr) return;

    asIScriptContext* ctx = engine->CreateContext();

    asIScriptFunction* func = engine->GetModule("OakModule")->GetFunctionByName("onPlayerHit");

    if (!func) return;

    ctx->Prepare(func);

    ctx->SetArgDWord(0, player);
    ctx->SetArgDWord(1, attacker);
    ctx->SetArgFloat(2, damage);

    ctx->Execute();

    ctx->Release();
}

void oak_angel_event_player_key(int player, int key, bool is_pressed) {
    if (engine->GetModule("OakModule") == nullptr) return;

    asIScriptContext* ctx = engine->CreateContext();

    asIScriptFunction* func = engine->GetModule("OakModule")->GetFunctionByName("onPlayerKey");

    if (!func) return;

    ctx->Prepare(func);

    ctx->SetArgDWord(0, player);
    ctx->SetArgDWord(1, key);
    ctx->SetArgDWord(2, is_pressed ? 1 : 0);

    ctx->Execute();

    ctx->Release();
}

void oak_angel_event_player_chat(int player, const char* text) {
    if (engine->GetModule("OakModule") == nullptr) return;

    asIScriptContext* ctx = engine->CreateContext();

    if (text[0] == '/')
    {
        auto ar = SplitString(std::string(text), " ");

        asIScriptFunction* func = commands[ar[0].substr(1)];

        if (!func)
        {
            asIScriptFunction* func1 = engine->GetModule("OakModule")->GetFunctionByName("playerInvalidCommand");

            if (!func1) return;

            ctx->Prepare(func1);

            std::string cmdName = ar[0].substr(1);

            ctx->SetArgDWord(0, player);
            ctx->SetArgObject(1, &cmdName);
        }
        else
        {
            ctx->Prepare(func);

            asITypeInfo* t = engine->GetTypeInfoByDecl("array<string>");

            CScriptArray* arr;

            if (ar.size() >= 1)
            {
                arr = CScriptArray::Create(t, ar.size() - 1);

                for (asUINT i = 1; i < ar.size(); i++)
                {
                    arr->SetValue(i - 1, &ar[i]);
                }
            }
            else
            {
                arr = CScriptArray::Create(t, (asUINT)0);
            }
            
            ctx->SetArgDWord(0, player);
            ctx->SetArgObject(1, arr);
        }
    }
    else
    {
        asIScriptFunction* func = engine->GetModule("OakModule")->GetFunctionByName("onPlayerChat");

        if (!func) return;

        ctx->Prepare(func);

        auto well = std::string(text);

        ctx->SetArgDWord(0, player);
        ctx->SetArgObject(1, &well);
    }

    

    ctx->Execute();

    ctx->Release();
}

void oak_angel_event_console(const char* text) {
    if (engine->GetModule("OakModule") == nullptr) return;

    std::string t = std::string(text);

    if (t == "") return;

    asIScriptContext* ctx = engine->CreateContext();

    asIScriptFunction* func = engine->GetModule("OakModule")->GetFunctionByName("onConsoleInput");

    if (!func) return;

    ctx->Prepare(func);

    ctx->SetArgObject(0, &t);

    ctx->Execute();

    ctx->Release();
}

void oak_angel_event_vehicle_destroy(int vehicle) {
    if (engine->GetModule("OakModule") == nullptr) return;

    asIScriptContext* ctx = engine->CreateContext();

    asIScriptFunction* func = engine->GetModule("OakModule")->GetFunctionByName("onVehicleDestroy");

    if (!func) return;

    ctx->Prepare(func);

    ctx->SetArgDWord(0, vehicle);

    ctx->Execute();

    ctx->Release();
}

void oak_angel_event_vehicle_player_use(int vehicle, int player, bool success, int seat_id, int entrance) {
    if (engine->GetModule("OakModule") == nullptr) return;

    asIScriptContext* ctx = engine->CreateContext();

    asIScriptFunction* func = engine->GetModule("OakModule")->GetFunctionByName("onVehicleUse");

    if (!func) return;

    ctx->Prepare(func);

    ctx->SetArgDWord(0, vehicle);
    ctx->SetArgDWord(1, player);
    ctx->SetArgDWord(2, success ? 1 : 0);
    ctx->SetArgDWord(3, seat_id);
    ctx->SetArgDWord(4, entrance);

    ctx->Execute();

    ctx->Release();
}

void oak_angel_event_dialog_done(int player, int dialogId, int dialogSel, const char* dialogText) {
    if (engine->GetModule("OakModule") == nullptr) return;

    asIScriptContext* ctx = engine->CreateContext();

    asIScriptFunction* func = engine->GetModule("OakModule")->GetFunctionByName("onDialogDone");

    if (!func) return;

    ctx->Prepare(func);

    auto well = std::string(dialogText);

    ctx->SetArgDWord(0, player);
    ctx->SetArgDWord(1, dialogId);
    ctx->SetArgDWord(2, dialogSel);
    ctx->SetArgObject(3, &well);

    ctx->Execute();

    ctx->Release();
}

void oak_angel_event_vehicle_exploded(int vehicle) {
    if (engine->GetModule("OakModule") == nullptr) return;

    asIScriptContext* ctx = engine->CreateContext();

    asIScriptFunction* func = engine->GetModule("OakModule")->GetFunctionByName("onVehicleExplode");

    if (!func) return;

    ctx->Prepare(func);

    ctx->SetArgDWord(0, vehicle);

    ctx->Execute();

    ctx->Release();
}
#pragma endregion

#pragma region AngelScript functions
void oak_angel_register_enums()
{
    int r = engine->RegisterEnum("VisibilityType"); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("VisibilityType", "Name", 0); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("VisibilityType", "Icon", 1); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("VisibilityType", "Radar", 2); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("VisibilityType", "Model", 3); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("VisibilityType", "Collision", 4); ZPL_ASSERT(r >= 0);

    r = engine->RegisterEnum("VehicleSeat"); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("VehicleSeat", "FrontLeft", 0); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("VehicleSeat", "FrontRight", 1); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("VehicleSeat", "RearLeft", 2); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("VehicleSeat", "RearRight", 3); ZPL_ASSERT(r >= 0);

    r = engine->RegisterEnum("DeathType"); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("DeathType", "Unknown", 1); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("DeathType", "Explosion", 2); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("DeathType", "Fire", 3); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("DeathType", "CarCrash", 5); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("DeathType", "FallDamage", 6); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("DeathType", "KilledByCar", 8); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("DeathType", "Drowned", 9); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("DeathType", "OutOfWorld", 10); ZPL_ASSERT(r >= 0);

    r = engine->RegisterEnum("PlayerPart"); ZPL_ASSERT(r >= 0); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("PlayerPart", "Unknown", 0); ZPL_ASSERT(r >= 0); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("PlayerPart", "RightHand", 1); ZPL_ASSERT(r >= 0); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("PlayerPart", "LeftHand", 2); ZPL_ASSERT(r >= 0); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("PlayerPart", "RightLeg", 3); ZPL_ASSERT(r >= 0); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("PlayerPart", "LeftLeg", 4); ZPL_ASSERT(r >= 0); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("PlayerPart", "Torso", 5); ZPL_ASSERT(r >= 0); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("PlayerPart", "Head", 5); ZPL_ASSERT(r >= 0); ZPL_ASSERT(r >= 0);

    r = engine->RegisterEnum("DialogType"); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("DialogType", "MsgBox", 0); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("DialogType", "Input", 1); ZPL_ASSERT(r >= 0);
    r = engine->RegisterEnumValue("DialogType", "Password", 2); ZPL_ASSERT(r >= 0);
}

void oak_angel_register_functions()
{
    #pragma region Player Functions
    int r = engine->RegisterGlobalFunction("array<int>@ playerGetList()", asFUNCTION(a_oak_player_list), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerSpawn(int playerid, float x, float y, float z, float heading)", asFUNCTION(a_oak_player_spawn), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerDespawn(int playerid)", asFUNCTION(oak_player_despawn), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerInvalid(int playerid)", asFUNCTION(oak_player_invalid), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerKick(int playerid, string reason)", asFUNCTION(a_oak_player_kick), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerKill(int playerid)", asFUNCTION(oak_player_kill), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerPlayAnim(int playerid, string animName)", asFUNCTION(a_oak_player_playanim), asCALL_CDECL); ZPL_ASSERT(r >= 0);

    r = engine->RegisterGlobalFunction("int playerSetModel(int playerid, string modelName)", asFUNCTION(a_oak_player_model_set), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerSetHealth(int playerid, float health)", asFUNCTION(oak_player_health_set), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerSetPosition(int playerid, float x, float y, float z)", asFUNCTION(a_oak_player_position_set), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerSetDirection(int playerid, float x, float y, float z)", asFUNCTION(a_oak_player_direction_set), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerSetHeading(int playerid, float heading)", asFUNCTION(oak_player_heading_set), asCALL_CDECL); ZPL_ASSERT(r >= 0);

    r = engine->RegisterGlobalFunction("int playerGiveWeapon(int playerid, int weapId, int ammo1, int ammo2)", asFUNCTION(oak_player_give_weapon), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerRemoveWeapon(int playerid, int weapId)", asFUNCTION(oak_player_remove_weapon), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerClearInventory(int playerid)", asFUNCTION(oak_player_clear_inventory), asCALL_CDECL); ZPL_ASSERT(r >= 0);

    r = engine->RegisterGlobalFunction("string playerGetName(int playerid)", asFUNCTION(a_oak_player_name_get), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("string playerGetModel(int playerid)", asFUNCTION(a_oak_player_model_get), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float playerGetHealth(int playerid)", asFUNCTION(oak_player_health_get), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float playerGetHeading(int playerid)", asFUNCTION(oak_player_heading_get), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float playerGetPositionX(int playerid)", asFUNCTION(a_oak_player_position_getx), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float playerGetPositionY(int playerid)", asFUNCTION(a_oak_player_position_gety), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float playerGetPositionZ(int playerid)", asFUNCTION(a_oak_player_position_getz), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float playerGetDirectionX(int playerid)", asFUNCTION(a_oak_player_direction_getx), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float playerGetDirectionY(int playerid)", asFUNCTION(a_oak_player_direction_gety), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float playerGetDirectionZ(int playerid)", asFUNCTION(a_oak_player_direction_getz), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("string playerGetIP(int playerid)", asFUNCTION(a_oak_player_ip_get), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("string playerGetStateCode(int playerid)", asFUNCTION(a_oak_player_statecode_get), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("string playerGetStateName(int playerid)", asFUNCTION(a_oak_player_statename_get), asCALL_CDECL); ZPL_ASSERT(r >= 0);

    r = engine->RegisterGlobalFunction("int playerEnableInput(int playerid, int enable)", asFUNCTION(oak_player_input_enable), asCALL_CDECL); ZPL_ASSERT(r >= 0);

    r = engine->RegisterGlobalFunction("int playerGetVisibility(int playerid, VisibilityType type)", asFUNCTION(oak_player_visibility_get), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerSetVisibility(int playerid, VisibilityType type, int state)", asFUNCTION(oak_player_visibility_set), asCALL_CDECL); ZPL_ASSERT(r >= 0);

    r = engine->RegisterGlobalFunction("int playerPlayAudioStream(int playerid, string url)", asFUNCTION(a_oak_play_audio_stream), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerPauseAudioStream(int playerid)", asFUNCTION(oak_pause_audio_stream), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerSetStreamVolume(int playerid, float vol)", asFUNCTION(oak_set_stream_volume), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerStopAudioStream(int playerid)", asFUNCTION(oak_stop_audio_stream), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    #pragma endregion

    #pragma region Camera Functions
    r = engine->RegisterGlobalFunction("int cameraSet(int playerid, float posx, float posy, float posz, float dirx, float diry, float dirz)", asFUNCTION(a_oak_camera_set), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int cameraUnlock(int playerid)", asFUNCTION(oak_camera_unlock), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int cameraTargetPlayer(int playerid, int targetid)", asFUNCTION(oak_camera_target_player), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int cameraTargetVehicle(int playerid, int targetid)", asFUNCTION(oak_camera_target_vehicle), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int cameraClearTarget(int playerid)", asFUNCTION(oak_camera_target_unset), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    #pragma endregion

    #pragma region HUD Functions
    r = engine->RegisterGlobalFunction("int playerHudFade(int playerid, int fadeout, int duration, int color)", asFUNCTION(oak_hud_fadeout), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerHudCountdown(int playerid, int number)", asFUNCTION(oak_hud_countdown), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerHudAnnounce(int playerid, string message, float length)", asFUNCTION(a_oak_hud_announce), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerHudMessage(int playerid, string message, int color)", asFUNCTION(a_oak_hud_message), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerShowDialog(int playerid, string title, string message, string button1, string button2, int dialogId, DialogType dialogType)", asFUNCTION(a_oak_dialog_show), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    #pragma endregion

    #pragma region Chat Functions
    r = engine->RegisterGlobalFunction("int playerSendMessage(int playerid, string message)", asFUNCTION(a_oak_chat_send), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int playerClearChat(int playerid)", asFUNCTION(oak_chat_clear), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int allSendMessage(string message)", asFUNCTION(a_oak_chat_broadcast), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    #pragma endregion

    #pragma region Vehicle Functions
    r = engine->RegisterGlobalFunction("array<int>@ vehicleGetList()", asFUNCTION(a_oak_vehicle_list), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int vehicleSpawn(string vehicleName, float x, float y, float z, float heading)", asFUNCTION(a_oak_vehicle_spawn), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int vehicleDespawn(int vehId)", asFUNCTION(oak_vehicle_despawn), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int vehicleInvalid(int vehId)", asFUNCTION(oak_vehicle_invalid), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int vehicleRepair(int vehId)", asFUNCTION(oak_vehicle_repair), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int vehicleExplode(int vehId)", asFUNCTION(oak_vehicle_explode), asCALL_CDECL); ZPL_ASSERT(r >= 0);

    r = engine->RegisterGlobalFunction("int vehicleSetPosition(int vehId, float x, float y, float z)", asFUNCTION(a_oak_vehicle_position_set), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int vehicleSetDirection(int vehId, float x, float y, float z)", asFUNCTION(a_oak_vehicle_direction_set), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int vehicleSetSpeed(int vehId, float speed)", asFUNCTION(oak_vehicle_speed_set), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int vehicleSetVelocity(int vehId, float x, float y, float z)", asFUNCTION(a_oak_vehicle_velocity_set), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int vehicleSetHeading(int vehId, float heading)", asFUNCTION(oak_vehicle_heading_set), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int vehicleSetFuel(int vehId, float fuel)", asFUNCTION(oak_vehicle_fuel_set), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int vehicleSetTransparency(int vehId, float transparency)", asFUNCTION(oak_vehicle_transparency_set), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int vehicleSetLock(int vehId, int state)", asFUNCTION(oak_vehicle_lock_set), asCALL_CDECL); ZPL_ASSERT(r >= 0);

    r = engine->RegisterGlobalFunction("float vehicleGetPositionX(int vehicleid)", asFUNCTION(a_oak_vehicle_position_getx), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float vehicleGetPositionY(int vehicleid)", asFUNCTION(a_oak_vehicle_position_gety), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float vehicleGetPositionZ(int vehicleid)", asFUNCTION(a_oak_vehicle_position_getz), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float vehicleGetDirectionX(int vehicleid)", asFUNCTION(a_oak_vehicle_direction_getx), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float vehicleGetDirectionY(int vehicleid)", asFUNCTION(a_oak_vehicle_direction_gety), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float vehicleGetDirectionZ(int vehicleid)", asFUNCTION(a_oak_vehicle_direction_getz), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float vehicleGetSpeed(int vehId)", asFUNCTION(oak_vehicle_speed_get), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float vehicleGetVelocityX(int vehicleid)", asFUNCTION(a_oak_vehicle_velocity_getx), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float vehicleGetVelocityY(int vehicleid)", asFUNCTION(a_oak_vehicle_velocity_gety), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float vehicleGetVelocityZ(int vehicleid)", asFUNCTION(a_oak_vehicle_velocity_getz), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float vehicleGetHeading(int vehId)", asFUNCTION(oak_vehicle_heading_get), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float vehicleGetFuel(int vehId)", asFUNCTION(oak_vehicle_fuel_get), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float vehicleGetTransparency(int vehId)", asFUNCTION(oak_vehicle_transparency_get), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int vehicleGetLock(int vehId)", asFUNCTION(oak_vehicle_lock_get), asCALL_CDECL); ZPL_ASSERT(r >= 0);

    r = engine->RegisterGlobalFunction("float vehicleSetVisibility(int vehId, VisibilityType type, int state)", asFUNCTION(oak_vehicle_visibility_set), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float vehicleGetVisibility(int vehId, VisibilityType type)", asFUNCTION(oak_vehicle_visibility_get), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    #pragma endregion

    #pragma region Vehicle/Player manipulation Functions
    r = engine->RegisterGlobalFunction("int vehiclePlayerPut(int vehId, int playerId, VehicleSeat seat)", asFUNCTION(oak_vehicle_player_put), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int vehiclePlayerRemove(int vehId, int playerId)", asFUNCTION(oak_vehicle_player_remove), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int vehiclePlayerInside(int playerId)", asFUNCTION(oak_vehicle_player_inside), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int vehiclePlayerGetSeat(int vehId, int playerId)", asFUNCTION(oak_vehicle_player_seat_get), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int vehiclePlayerAtSeat(int vehId, VehicleSeat seat)", asFUNCTION(oak_vehicle_player_at_seat), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    #pragma endregion

    #pragma region Misc Functions
    r = engine->RegisterGlobalFunction("void print(string message)", asFUNCTION(oak_print), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int setKillbox(float level)", asFUNCTION(oak_killbox_set), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("float getKillbox()", asFUNCTION(oak_killbox_get), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int createExplosion(int sender, float x, float y, float z, float radius, float force)", asFUNCTION(a_oak_create_explosion), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalFunction("int serverReload()", asFUNCTION(oak_server_reload), asCALL_CDECL); ZPL_ASSERT(r >= 0);
    #pragma endregion
}

int upd = 0;

void oak_angel_tick()
{
    oak_angel_event_server_tick();

    if (upd >= oak__console_computed_fps)
    {
        oak_angel_event_server_tick_second();
        upd = 0;
    }

    upd++;
}

void oak_angel_init()
{
    oak_log("^F[^5INFO^F] Initializing AngelScript^R\n");

    engine = asCreateScriptEngine();

    int r = engine->SetMessageCallback(asFUNCTION(oak_angel_msg_callback), 0, asCALL_CDECL); ZPL_ASSERT(r >= 0);

    RegisterStdString(engine);
    RegisterScriptAny(engine);
    RegisterScriptArray(engine, true);
    RegisterScriptDateTime(engine);
    RegisterScriptFile(engine);
    RegisterScriptFileSystem(engine);
    RegisterStdStringUtils(engine);

    r = engine->RegisterFuncdef("void OakCmdCallback(int playerid, array<string> args)"); ZPL_ASSERT(r >= 0);

    r = engine->RegisterGlobalFunction("void registerCommand(string cmdName, OakCmdCallback @cb)", asFUNCTION(RegisterCommand), asCALL_CDECL); ZPL_ASSERT(r >= 0);

    r = engine->RegisterGlobalFunction("int random(int min, int max)", asFUNCTION(RandomNumber), asCALL_CDECL); ZPL_ASSERT(r >= 0);

    r = engine->RegisterGlobalProperty("int maxPlayers", &GlobalConfig.max_players); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalProperty("int connectedPlayers", &GlobalConfig.players); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalProperty("string serverName", &GlobalConfig.name); ZPL_ASSERT(r >= 0);

    oak_angel_register_enums();
    oak_angel_register_functions();

    CScriptBuilder builder;

    builder.SetIncludeCallback(oak_angel_include_callback, NULL);

    r = builder.StartNewModule(engine, "OakModule");
    if (r < 0)
    {
        oak_log("^F[^9ERROR^F] Cannot start new script module.\n");
        return;
    }
    r = builder.AddSectionFromFile(GlobalConfig.script_file.c_str());
    if (r < 0)
    {
        oak_log("^F[^9ERROR^F] Failed to add script file.\n");
        return;
    }
    r = builder.BuildModule();
    if (r < 0)
    {
        oak_log("^F[^9ERROR^F] Failed to build script module, please fix all the issues and run again.\n");
        return;
    }

    asIScriptModule* mod = engine->GetModule("OakModule");
    asIScriptFunction* func = mod->GetFunctionByDecl("void main()");
    if (func == 0)
    {
        // The function couldn't be found. Instruct the script writer
        // to include the expected function in the script.
        oak_log("^F[^9ERROR^F] The script must have the function ^A'void main()'^F. Please add it and try again.^R\n");
        return;
    }

    // Create our context, prepare it, and then execute
    asIScriptContext* ctx = engine->CreateContext();
    ctx->Prepare(func);
    r = ctx->Execute();
    if (r != asEXECUTION_FINISHED)
    {
        // The execution didn't complete as expected. Determine what happened.
        if (r == asEXECUTION_EXCEPTION)
        {
            // An exception occurred, let the script writer know what happened so it can be corrected.
            oak_log("^F[^9ERROR^F] An exception ^A'%s'^F occurred. Please correct the code and try again.^R\n", ctx->GetExceptionString());
        }
    }

    ctx->Release();

    canRun = true;
}

void oak_angel_reload()
{
    oak_log("^F[^5INFO^F] Reloading AngelScript^R\n");

    canRun = false;

    for (std::map<std::string, asIScriptFunction*>::iterator it = commands.begin(); it != commands.end(); ++it)
    {
        it->second->Release();
    }

    commands.clear();

    engine->ShutDownAndRelease();

    engine = asCreateScriptEngine();

    int r = engine->SetMessageCallback(asFUNCTION(oak_angel_msg_callback), 0, asCALL_CDECL); ZPL_ASSERT(r >= 0);

    RegisterStdString(engine);
    RegisterScriptAny(engine);
    RegisterScriptArray(engine, true);
    RegisterScriptDateTime(engine);
    RegisterScriptFile(engine);
    RegisterScriptFileSystem(engine);
    RegisterStdStringUtils(engine);

    r = engine->RegisterFuncdef("void OakCmdCallback(int playerid, array<string> args)"); ZPL_ASSERT(r >= 0);

    r = engine->RegisterGlobalFunction("void registerCommand(string cmdName, OakCmdCallback @cb)", asFUNCTION(RegisterCommand), asCALL_CDECL); ZPL_ASSERT(r >= 0);

    r = engine->RegisterGlobalFunction("int random(int min, int max)", asFUNCTION(RandomNumber), asCALL_CDECL); ZPL_ASSERT(r >= 0);

    r = engine->RegisterGlobalProperty("int maxPlayers", &GlobalConfig.max_players); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalProperty("int connectedPlayers", &GlobalConfig.players); ZPL_ASSERT(r >= 0);
    r = engine->RegisterGlobalProperty("string serverName", &GlobalConfig.name); ZPL_ASSERT(r >= 0);

    oak_angel_register_enums();
    oak_angel_register_functions();

    CScriptBuilder builder;

    builder.SetIncludeCallback(oak_angel_include_callback, NULL);

    r = builder.StartNewModule(engine, "OakModule");
    if (r < 0)
    {
        oak_log("^F[^9ERROR^F] Cannot start new script module.\n");
        return;
    }
    r = builder.AddSectionFromFile(GlobalConfig.script_file.c_str());
    if (r < 0)
    {
        oak_log("^F[^9ERROR^F] Failed to add script file.\n");
        return;
    }
    r = builder.BuildModule();
    if (r < 0)
    {
        oak_log("^F[^9ERROR^F] Failed to build script module, please fix all the issues and run again.\n");
        return;
    }

    asIScriptModule* mod = engine->GetModule("OakModule");
    asIScriptFunction* func = mod->GetFunctionByDecl("void main()");
    if (func == 0)
    {
        // The function couldn't be found. Instruct the script writer
        // to include the expected function in the script.
        oak_log("^F[^9ERROR^F] The script must have the function ^A'void main()'^F. Please add it and try again.^R\n");
        return;
    }

    // Create our context, prepare it, and then execute
    asIScriptContext* ctx = engine->CreateContext();
    ctx->Prepare(func);
    r = ctx->Execute();
    if (r != asEXECUTION_FINISHED)
    {
        // The execution didn't complete as expected. Determine what happened.
        if (r == asEXECUTION_EXCEPTION)
        {
            // An exception occurred, let the script writer know what happened so it can be corrected.
            oak_log("^F[^9ERROR^F] An exception ^A'%s'^F occurred. Please correct the code and try again.^R\n", ctx->GetExceptionString());
        }
    }

    ctx->Release();

    canRun = true;
}

void oak_angel_stop()
{
    oak_log("^F[^5INFO^F] Shutting down AngelScript^R\n");

    canRun = false;

    for (std::map<std::string, asIScriptFunction*>::iterator it = commands.begin(); it != commands.end(); ++it)
    {
        it->second->Release();
    }
    
    commands.clear();

    engine->ShutDownAndRelease();
}
#pragma endregion
