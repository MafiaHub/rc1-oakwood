#pragma once

#define MASTERLIST_HOST "http://oakmaster.madaraszd.net/push"
#define MASTERLIST_PUSH_TIME 30.0f
#define MASTERLIST_PUSH_TIMEOUT 5.0f
#define MASTERLIST_POLL_TIME 1.0f
#define MASTERLIST_FORMAT R"FOO({
	"host": "%s",
	"name": "%s",
	"players": %d,
	"maxPlayers": %d,
	"pass": %s,
	"port": "%d"
})FOO"

http_t* masterlist_form_request() {
	zpl_local_persist char buf[512] = { 0 };

	snprintf(buf, 512, MASTERLIST_FORMAT,
		GlobalConfig.host.c_str(),
		GlobalConfig.name.c_str(),
		(int)GlobalConfig.players,
		(int)GlobalConfig.max_players,
		"false",
		(int)GlobalConfig.port
	);

	return http_post(MASTERLIST_HOST, buf, zpl_strlen(buf), nullptr);
}

std::string masterlist_response(http_t *res) {
	zpl_local_persist char buf[512] = { 0 };
	zpl_local_persist zpl_json_object root = { 0 };
	u8 err = false;

	zpl_json_parse(&root, res->response_size, (char *)res->response_data, zpl_heap(), false, &err);

	if (err) {
		zpl_json_free(&root);
		return "Unknown response";
	}

	zpl_json_object *msg;
	zpl_json_find(&root, "message", false, &msg);

	if (!msg) {
		zpl_json_free(&root);
		return "Invalid response";
	}

	return msg->string;
}

void masterlist_push() {
	zpl_local_persist http_t *req = nullptr;
	zpl_local_persist f64 last_push_time = -MASTERLIST_PUSH_TIME;
	zpl_local_persist b32 was_push_successful = false;
	zpl_local_persist f32 push_time = 0.0f;

	if (zpl_time_now() - last_push_time > MASTERLIST_PUSH_TIME) {
		last_push_time = zpl_time_now();
		req = masterlist_form_request();
		push_time = zpl_time_now();
	}

	if (!req) return;

	auto status = http_process(req);

	if (status == HTTP_STATUS_PENDING && (zpl_time_now() - push_time > MASTERLIST_PUSH_TIMEOUT)) {
		// Force the server to re-send the request if it takes too long for the existing request to be processed.
		last_push_time = -MASTERLIST_PUSH_TIME;
	}

	switch (status) {
	case HTTP_STATUS_FAILED:
		zpl_printf("[ERROR] Could not push update to the masterlist! \n\t%s: \"%s\" (%d)\n", 
			req->reason_phrase, masterlist_response(req).c_str(), req->status_code);

		if (req->status_code == 502) {
			zpl_printf("Masterlist is down! Please, contact developers!\n");
		}

		http_release(req);
		was_push_successful = false;
		req = nullptr;
		break;

	case HTTP_STATUS_COMPLETED:
		http_release(req);

		if (!was_push_successful)
			zpl_printf("Successfully registered to the masterlist!\n");

		was_push_successful = true;
		req = nullptr;
		break;
	}
}

void masterlist_update() {
	zpl_local_persist f64 last_masterlist_update = 0.0f;

	if (GlobalConfig.visible && zpl_time_now() - last_masterlist_update > MASTERLIST_POLL_TIME) {
		last_masterlist_update = zpl_time_now();

		masterlist_push();
	}
}