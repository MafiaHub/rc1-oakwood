inline auto entitycreate(librg_event *evnt) {
    auto new_door = new mafia_door();
    auto name_len = librg_data_ru32(evnt->data);
    librg_data_rptr(evnt->data, new_door->name, name_len);
    new_door->angle = librg_data_rf32(evnt->data);
    new_door->open_side = librg_data_ru8(evnt->data);
    new_door->name[name_len] = '\0';

    auto found = (MafiaSDK::C_Door *)MafiaSDK::GetMission()->FindActorByName(new_door->name);
    
    if(found) {
        
        //NOTE(DavoSK): If doors are not opening set open angle
        auto door_int = found->GetInterface();

        if (door_int->current_state != 2 && door_int->current_state != 3) {
            //NOTE(DavoSK): We need to set direction byte & calculate percentage of open
            door_int->open_direction = new_door->open_side;
            auto set_angle = new_door->angle / door_int->open_percent_koef;
            found->SetOpenAngle(set_angle);
        }

        new_door->door = found;
    }

    evnt->entity->user_data = new_door;
}

inline auto clientstreamer_update(librg_event *evnt) {
    auto door = (mafia_door*)evnt->entity->user_data;
    
    if (!door || !door->door) {
        librg_event_reject(evnt);
        return;
    }

    auto door_int = door->door->GetInterface();
    librg_data_wf32(evnt->data, door_int->open_angle);
    librg_data_wu8(evnt->data, door_int->open_direction);
}

inline auto entityremove(librg_event *evnt) {
    auto door = (mafia_door *)evnt->entity->user_data;
    
    if(door) {
        delete door;
        evnt->entity->user_data = nullptr;
    }
}
