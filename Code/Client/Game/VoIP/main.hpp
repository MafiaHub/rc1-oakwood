namespace voip {

    // TODO: sync thoose from server
    constexpr int bitrate 		= 64000;
    constexpr int sample_rate 	= 48000;
    constexpr int channel_count = 2;

	constexpr int frame_duration = 600;
	constexpr int frame_samples_per_channel = (sample_rate * frame_duration) / 10000LL;
	constexpr int frame_size = channel_count * frame_samples_per_channel * sizeof(int16_t);

    HRECORD chan = NULL;
    OpusEncoder *encoder = nullptr;

	// FIFO buffer for splitting mic data into even chunks
	int mic_buffer_size = 0;
	char* mic_buffer = nullptr;

	// Buffer to encode
	int16_t frame_to_encode[frame_size];
    int16_t frame_decoded[frame_size];
	uint8_t frame_to_send[frame_size];

    /*
    * Function wich decode encoded data and pushes it into streamed output
    */
    auto decode_and_push(voip_channel_t* player_channel, unsigned char* encoded, u32 encoded_size) {
        
        i32 decoded_samples = opus_decode(player_channel->decoder, encoded, encoded_size, frame_decoded, sizeof(frame_decoded), 0);
        if (decoded_samples < 0) {
            printf("[VOIP] Decode failed\n");
            return;
        }

	    i32 decoded_size = frame_samples_per_channel * channel_count * sizeof(int16_t);
	    BASS_StreamPutData(player_channel->playback_stream, frame_decoded, decoded_size);
    }

	/*
	* Function for encoding and sending user microphone data 
	* Data wich needs to be encoded needs be in even chunks ( frame_size )
	*/
	auto encode_and_send(int16_t* to_encode) {
		
		u32 encoded_lenght = opus_encode(encoder, to_encode, frame_samples_per_channel, (unsigned char*)frame_to_send, sizeof(frame_to_send));
		if (encoded_lenght < 0) {
			printf("[VOIP] Encode failed: %s\n", opus_strerror(encoded_lenght));
			return;
		}

        if(librg_is_connected(&network_context)) {
            librg_send(&network_context, NETWORK_SEND_VOIP_DATA, data, {
                librg_data_wu32(&data, encoded_lenght);
                librg_data_wptr(&data, (void *)frame_to_send, encoded_lenght);
            });
		}	
	}

    /*
    * BASS microphone callback
    */ 
    BOOL prepare_ecnode_buffer(const void *buffer, DWORD length) {
		
		// allocate buffering buffer 4x for FIFO
		if (!mic_buffer) mic_buffer = (char*)malloc(length * 4);

		mic_buffer_size += length;
		memcpy(mic_buffer + (mic_buffer_size - length), buffer, length);

		if (mic_buffer_size > frame_size) {

			memcpy(frame_to_encode, (char*)mic_buffer, frame_size);
			encode_and_send(frame_to_encode);

			int overflow = mic_buffer_size - frame_size;
			mic_buffer_size -= frame_size;
			memcpy((char*)mic_buffer + (mic_buffer_size - overflow), (char*)mic_buffer + frame_size, overflow);
		}
	
        return TRUE;
    }

    auto network_tick() {
        
        u32 offset = 0;
        u32 avalable = BASS_ChannelGetData(chan, NULL, BASS_DATA_AVAILABLE);
		void* real_data = malloc(avalable);
		BASS_ChannelGetData(chan, real_data, avalable);

		while (avalable >= frame_size) {
			prepare_ecnode_buffer((char*)real_data + offset, frame_size);
			avalable -= frame_size;
			offset += frame_size;
		}

		if (avalable > 0) {
			prepare_ecnode_buffer((char*)real_data + offset, avalable);
		}

		free(real_data);
    }

    /* 
    * Inits opus encoder for encoding user input
    * Inits bass libary and create recording stream to capture microphone buffer
    */
    auto init() {

        int error_code;
        encoder = opus_encoder_create(sample_rate, channel_count, OPUS_APPLICATION_VOIP, &error_code);
        assert(error_code == OPUS_OK);

        BASS_SetConfig(BASS_CONFIG_VISTA_TRUEPOS, 0);

        if (!BASS_Init(-1, sample_rate, NULL, NULL, NULL)) {
            MessageBoxA(NULL, "Unable to init bass audio !", "BASS_Init", MB_ICONERROR | MB_OK);
            return;
        }

        if (!BASS_RecordInit(-1)) {
            MessageBoxA(NULL, "Unable to init record  !", "BASS_RecordInit", MB_ICONERROR | MB_OK);
            return;
        }

        if (!(chan = BASS_RecordStart(sample_rate, channel_count, MAKELONG(0, 50), NULL, 0))) {
		    MessageBoxA(NULL, "Unable to start recording  !", "BASS_RecordStart", MB_ICONERROR | MB_OK);
		    return;
        }
    }

	/* 
	* Function wich creates new channel for remote ped
	*/
    auto create_remote() {

        int error_code;
		voip_channel_t* new_voip = new voip_channel_t();

        /* Create a new decoder state. */
        new_voip->decoder = opus_decoder_create(sample_rate, channel_count, &error_code);
        assert(error_code == OPUS_OK);

        /* Create playback stream for one instance*/
        new_voip->playback_stream = BASS_StreamCreate(sample_rate, channel_count, BASS_STREAM_AUTOFREE, STREAMPROC_PUSH, NULL);
        BASS_ChannelPlay(new_voip->playback_stream, FALSE);
	    BASS_ChannelSetAttribute(new_voip->playback_stream, BASS_ATTRIB_VOL, 3.0);
        
        return new_voip;
    }
}