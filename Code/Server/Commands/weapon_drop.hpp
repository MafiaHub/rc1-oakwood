#pragma once

librg_entity_t *spawn_weapon_drop(zpl_vec3 position, char *model, inventory_item item) {
    mafia_weapon_drop *drop = new mafia_weapon_drop();

    drop->weapon = item;
    strncpy(drop->model, model, strlen(model));

    auto new_weapon_entity			= librg_entity_create(&network_context, TYPE_WEAPONDROP);
    new_weapon_entity->position		= position;
    new_weapon_entity->position.y	+= 0.7f;
    new_weapon_entity->user_data	= drop;

    return new_weapon_entity;
}