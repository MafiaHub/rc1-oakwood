#define OAK_MASTERLIST_HOST "http://master.oakwood-mp.net/push"
#define OAK_MASTERLIST_PUSH_TIME 30.0f
#define OAK_MASTERLIST_PUSH_TIMEOUT 5.0f
#define OAK_MASTERLIST_POLL_TIME 1.0f

const char *oak__masterlist_response(http_t *res) {
    zpl_local_persist char buf[512] = { 0 };
    zpl_local_persist zpl_json_object root = { 0 };
    u8 err = false;

    zpl_json_parse(&root, res->response_size, (char *)res->response_data, zpl_heap(), false, &err);

    if (err) {
        zpl_json_free(&root);
        return "Unknown response";
    }

    zpl_json_object *msg = zpl_json_find(&root, "message", false);

    if (!msg) {
        zpl_json_free(&root);
        return "Invalid response";
    }

    sprintf(buf, "%s", msg->string);

    return (const char *)buf;
}

http_t *oak__masterlist_form_request() {
    zpl_global char buf[512] = { 0 };

    oak_endp_payload_info(buf);

    return http_post(OAK_MASTERLIST_HOST, buf, zpl_strlen(buf), NULL);
}

void oak__masterlist_push() {
    zpl_local_persist http_t *req = nullptr;
    zpl_local_persist f64 last_push_time = -OAK_MASTERLIST_PUSH_TIME;
    zpl_local_persist b32 was_push_successful = false;
    zpl_local_persist f32 push_time = 0.0f;

    if (zpl_time_now() - last_push_time > OAK_MASTERLIST_PUSH_TIME) {
        last_push_time = zpl_time_now();
        req = oak__masterlist_form_request();
        push_time = zpl_time_now();
    }

    if (!req) return;

    auto status = http_process(req);

    if (status == HTTP_STATUS_PENDING && (zpl_time_now() - push_time > OAK_MASTERLIST_PUSH_TIMEOUT)) {
        // Force the server to re-send the request if it takes too long for the existing request to be processed.
        last_push_time = -OAK_MASTERLIST_PUSH_TIME;
    }

    switch (status) {
        case HTTP_STATUS_FAILED:
            zpl_printf("[ERROR] Could not push update to the masterlist! \n\t%s: \"%s\" (%d)\n",
                req->reason_phrase, oak__masterlist_response(req), req->status_code);

            if (req->status_code == 502) {
                mod_log("Masterlist is down! Please, contact developers!");
            }

            http_release(req);
            was_push_successful = false;
            req = nullptr;
            break;

        case HTTP_STATUS_COMPLETED:
            http_release(req);

            if (!was_push_successful)
                oak_log("Successfully registered to the masterlist!");

            was_push_successful = true;
            req = nullptr;
            break;

        case HTTP_STATUS_PENDING:
            break;
    }
}

void oak_masterlist_update() {
    zpl_local_persist f64 last_update = 0.0f;

    if (GlobalConfig.visible && zpl_time_now() - last_update > OAK_MASTERLIST_POLL_TIME) {
        last_update = zpl_time_now();

        oak__masterlist_push();
    }
}
