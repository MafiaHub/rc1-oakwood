inline auto clientstreamer_update(librg_event *evnt) {
    auto door = (mafia_door*)evnt->entity->user_data;
    door->angle = librg_data_rf32(evnt->data);
    door->open_side = librg_data_ru8(evnt->data);
}

inline auto entitycreate(librg_event *evnt) {
    auto door = (mafia_door *)evnt->entity->user_data;
    auto name_len = strlen(door->name);
    librg_data_wu32(evnt->data, name_len);
    librg_data_wptr(evnt->data, door->name, name_len);
    librg_data_wf32(evnt->data, door->angle);
    librg_data_wu8(evnt->data, door->open_side);
}
