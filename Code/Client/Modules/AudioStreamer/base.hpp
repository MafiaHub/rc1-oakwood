namespace audiostream
{
    bool isPlaying = false, isMuted = false, canPlay = false;
    HSTREAM stream;
    float currentVolume = 1;
    std::string playingUrl;

    void mute()
    {
        if (!stream || !canPlay) return;

        BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, 0);

        *(float*)ADDR_MUSIC_SLIDER = GlobalConfig.mus_volume;
        MafiaSDK::GetMission()->GetGame()->UpdateMusicVolume();

        isMuted = true;
    }

    void unmute()
    {
        if (!stream || !canPlay) return;

        BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, currentVolume);

        *(float*)ADDR_MUSIC_SLIDER = 0;
        MafiaSDK::GetMission()->GetGame()->UpdateMusicVolume();

        isMuted = false;
    }

    void set_volume(float volume)
    {
        if (!stream || !canPlay) return;

        currentVolume = volume;
        if (!isMuted)
            BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, volume);
    }

    float get_volume()
    {
        return currentVolume;
    }

    bool create(const std::string& url)
    {
        if (!canPlay) return false;

        playingUrl = url;

        if (stream) {
            BASS_ChannelStop(stream);
            BASS_StreamFree(stream);
        }

        stream = BASS_StreamCreateURL(url.c_str(), NULL, NULL, nullptr, nullptr);

        if (!stream) 
        { 
            playingUrl = "";
            return false;
        }

        set_volume(GlobalConfig.mus_volume);

        return true;
    }

    void pause()
    {
        if (!stream || !canPlay) return;

        *(float*)ADDR_MUSIC_SLIDER = GlobalConfig.mus_volume;
        MafiaSDK::GetMission()->GetGame()->UpdateMusicVolume();

        BASS_ChannelPause(stream);
        isPlaying = false;
    }

    void play()
    {
        if (!stream || !canPlay) return;

        BASS_ChannelPlay(stream, false);

        *(float*)ADDR_MUSIC_SLIDER = 0;
        MafiaSDK::GetMission()->GetGame()->UpdateMusicVolume();

        isPlaying = true;
    }

    void stop()
    {
        if (!stream || !canPlay) return;

        BASS_ChannelStop(stream);
        BASS_StreamFree(stream);

        *(float*)ADDR_MUSIC_SLIDER = GlobalConfig.mus_volume;
        MafiaSDK::GetMission()->GetGame()->UpdateMusicVolume();

        stream = NULL;

        isPlaying = false;
    }

    bool is_playing()
    {
        return isPlaying;
    }

    void init()
    {
        librg_network_add(&network_context, NETWORK_PLAY_STREAM, [](librg_message* msg) {
            u16 len = librg_data_ru16(msg->data);
            zpl_string url = zpl_string_make_reserve(zpl_heap(), len);
            librg_data_rptr(msg->data, url, len);

            if (stream && !isPlaying)
            {
                play();
            }
            else
            {
                if (!create(std::string(url)))
                {
                    chat::add_message("{ff0000}ERROR: {ffffff}Cannot create audio stream!");
                    chat::add_message("{00ff00}URL: {ffffff}" + std::string(url));
                }
                else
                {
                    play();
                }
            }
            });

        librg_network_add(&network_context, NETWORK_PAUSE_STREAM, [](librg_message* msg) {
            pause();
        });

        librg_network_add(&network_context, NETWORK_SET_STREAM_VOLUME, [](librg_message* msg) {
            f32 vol = librg_data_rf32(msg->data);
            set_volume(vol);
        });

        librg_network_add(&network_context, NETWORK_STOP_STREAM, [](librg_message* msg) {
            stop();
        });

        if (!BASS_Init(-1, 44100, BASS_DEVICE_STEREO, nullptr, nullptr))
        {
            printf("[error] Cannot initialize BASS audio device!\n");
            canPlay = false;
        }
        else
        {
            canPlay = true;
        }
    }
}
