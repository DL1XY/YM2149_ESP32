/*
 * rest_common.h
 *
 *  Created on: 17.03.2020
 *      Author: arnew
 */

#ifndef MAIN_REST_COMMON_H_
#define MAIN_REST_COMMON_H_
#include <string.h>
#include <fcntl.h>
#include "esp_http_server.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "cJSON.h"


#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)
#define SCRATCH_BUFSIZE (10240)

typedef struct rest_server_context {
    char base_path[ESP_VFS_PATH_MAX + 1];
    char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;

#define CHECK_FILE_EXTENSION(filename, ext) (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)


esp_err_t rest_common_get_handler(httpd_req_t *req);
esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath);






#endif /* MAIN_REST_COMMON_H_ */
