#pragma once

void init_chat_commands() {
    register_command("/q", [&](std::vector<std::string> args) {
        librg_network_stop(&network_context);
        exit(0);
    });

    register_command("/npc", [&](std::vector<std::string> args) {
        librg_send(&network_context, NETWORK_NPC_CREATE, data, {});
    });

    register_command("/shade", [&](std::vector<std::string> args) {
        effects::load(GlobalConfig.localpath + "files/Cinematic.fx");
        effects::is_enabled = !effects::is_enabled;
    });

    register_command("/savepos", [&](std::vector<std::string> args) {
        auto local_player = MafiaSDK::GetMission()->GetGame()->GetLocalPlayer();

        std::ofstream pos_file("positions.txt");
        auto pos = local_player->GetInterface()->humanObject.entity.position;
        auto dir = local_player->GetInterface()->humanObject.entity.rotation;
        zpl_vec3 position = EXPAND_VEC(pos);
        zpl_vec3 direction = EXPAND_VEC(dir);
        auto rot = DirToRotation180(direction);

        pos_file << position.x << " " << position.y << " " << position.z << ", " << rot << std::endl;
    });
}
