#define OAK_LOGGER_FILENAME "server.log"

zpl_file oak__debug_log = {0};
zpl_mutex logger_mutex = {0};

/**
 * Init logger
 */
int oak_log_init() {
    zpl_file_remove(OAK_LOGGER_FILENAME);
    zpl_file_create(&oak__debug_log, OAK_LOGGER_FILENAME);
    zpl_mutex_init(&logger_mutex);

    return 0;
}

/**
 * Free logger and opened files
 */
int oak_log_free() {
    zpl_file_close(&oak__debug_log);
    zpl_mutex_destroy(&logger_mutex);

    return 0;
}

/**
 * Do actual logging
 * @param fmt   string with pattern
 * @param 0..N  values to be put in the string
 */
int oak_log(const char *fmt, ...) {
    va_list ap;
    char message[2048] = { 0 };
    va_start(ap, fmt);
    zpl_snprintf_va(message, 2048, fmt, ap);
    va_end(ap);

    zpl_mutex_lock(&logger_mutex);
    printf("%s", message);
    zpl_file_write(&oak__debug_log, message, zpl_strlen(message));
    zpl_mutex_unlock(&logger_mutex);

    return 0;
}

/**
 * Single argument size limited string print
 * @param  str
 * @param  length
 * @return
 */
int oak_logn(const char *str, int length) {
    return oak_log("%.*s", length, str);
}
