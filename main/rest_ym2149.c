/* HTTP Restful API Server

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "rest_common.h"
#include "ym2149.h"

static const char *YM2149_REST_TAG = "YM2149-REST";

#define REST_CHECK(a, str, goto_tag, ...)                                          \
do                                                                                 \
{                                                                                  \
	if (!(a))                                                                      \
	{                                                                              \
		ESP_LOGE(YM2149_REST_TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
		goto goto_tag;                                                             \
	}                                                                              \
} while (0)
/* Simple handler for getting system handler */
static esp_err_t system_info_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    cJSON_AddStringToObject(root, "version", IDF_VER);
    cJSON_AddNumberToObject(root, "cores", chip_info.cores);
    const char *sys_info = cJSON_Print(root);
    httpd_resp_sendstr(req, sys_info);
    free((void *)sys_info);
    cJSON_Delete(root);
    return ESP_OK;
}

/* Simple handler for getting temperature data */
static esp_err_t temperature_data_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "raw", esp_random() % 20);
    const char *sys_info = cJSON_Print(root);
    httpd_resp_sendstr(req, sys_info);
    free((void *)sys_info);
    cJSON_Delete(root);
    return ESP_OK;
}


// ########### YM2149 ###############
static esp_err_t YM2149_setChannelFreqFineHandler(httpd_req_t *req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
    int received = 0;
    if (total_len >= SCRATCH_BUFSIZE) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    cJSON *root = cJSON_Parse(buf);
    uint8_t channel = (uint8_t)cJSON_GetObjectItem(root, "channel")->valueint;
    uint8_t value = (uint8_t)cJSON_GetObjectItem(root, "value")->valueint;
    YM2149_setChannelFreqFine(&channel, &value);

    ESP_LOGI(YM2149_REST_TAG, "setChannelFreqFineHandler channel:%d value:%d", channel, value);
    cJSON_Delete(root);
    httpd_resp_sendstr(req, "Post control value successfully");
    return ESP_OK;
}

static esp_err_t YM2149_setChannelFreqRoughHandler(httpd_req_t *req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
    int received = 0;
    if (total_len >= SCRATCH_BUFSIZE) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    cJSON *root = cJSON_Parse(buf);
    uint8_t channel = (uint8_t)cJSON_GetObjectItem(root, "channel")->valueint;
    uint8_t value = (uint8_t)cJSON_GetObjectItem(root, "value")->valueint;
    YM2149_setChannelFreqRough (&channel, &value);

    ESP_LOGI(YM2149_REST_TAG, "setChannelFreqRoughHandler channel:%d value:%d", channel, value);
    cJSON_Delete(root);
    httpd_resp_sendstr(req, "Post control value successfully");
    return ESP_OK;
}

static esp_err_t YM2149_setNoiseFreqHandler(httpd_req_t *req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
    int received = 0;
    if (total_len >= SCRATCH_BUFSIZE) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    cJSON *root = cJSON_Parse(buf);

    uint8_t value = (uint8_t)cJSON_GetObjectItem(root, "value")->valueint;
    YM2149_setNoiseFreq(&value);

    ESP_LOGI(YM2149_REST_TAG, "YM2149_setNoiseFreqHandler value:%d", value);
    cJSON_Delete(root);
    httpd_resp_sendstr(req, "Post control value successfully");
    return ESP_OK;
}


static esp_err_t YM2149_setChannelNoiseHandler(httpd_req_t *req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
    int received = 0;
    if (total_len >= SCRATCH_BUFSIZE) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    cJSON *root = cJSON_Parse(buf);
    uint8_t channel = (uint8_t)cJSON_GetObjectItem(root, "channel")->valueint;
    bool value = (bool)cJSON_GetObjectItem(root, "value")->valueint;
    YM2149_setChannelNoise (&channel, &value);

    ESP_LOGI(YM2149_REST_TAG, "setChannelNoiseHandler channel:%d value:%d", channel, value);
    cJSON_Delete(root);
    httpd_resp_sendstr(req, "Post control value successfully");
    return ESP_OK;
}

static esp_err_t YM2149_setChannelToneHandler(httpd_req_t *req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
    int received = 0;
    if (total_len >= SCRATCH_BUFSIZE) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    cJSON *root = cJSON_Parse(buf);
    uint8_t channel = (uint8_t)cJSON_GetObjectItem(root, "channel")->valueint;
    bool value = (bool)cJSON_GetObjectItem(root, "value")->valueint;
    YM2149_setChannelTone (&channel, &value);

    ESP_LOGI(YM2149_REST_TAG, "YM2149_setChannelToneHandler channel:%d value:%d", channel, value);
    cJSON_Delete(root);
    httpd_resp_sendstr(req, "Post control value successfully");
    return ESP_OK;
}

static esp_err_t YM2149_setChannelLevelModeHandler(httpd_req_t *req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
    int received = 0;
    if (total_len >= SCRATCH_BUFSIZE) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    cJSON *root = cJSON_Parse(buf);
    uint8_t channel = (uint8_t)cJSON_GetObjectItem(root, "channel")->valueint;
    bool value = (bool)cJSON_GetObjectItem(root, "value")->valueint;
    YM2149_setChannelLevelMode (&channel, &value);

    ESP_LOGI(YM2149_REST_TAG, "YM2149_setChannelLevelModeHandler channel:%d value:%d", channel, value);
    cJSON_Delete(root);
    httpd_resp_sendstr(req, "Post control value successfully");
    return ESP_OK;
}

static esp_err_t YM2149_setChannelLevelHandler(httpd_req_t *req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
    int received = 0;
    if (total_len >= SCRATCH_BUFSIZE) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    cJSON *root = cJSON_Parse(buf);
    uint8_t channel = (uint8_t)cJSON_GetObjectItem(root, "channel")->valueint;
    uint8_t value = (uint8_t)cJSON_GetObjectItem(root, "value")->valueint;
    YM2149_setChannelLevel (&channel, &value);

    ESP_LOGI(YM2149_REST_TAG, "setChannelLevelHandler channel:%d value:%d", channel, value);
    cJSON_Delete(root);
    httpd_resp_sendstr(req, "Post control value successfully");
    return ESP_OK;
}

static esp_err_t YM2149_setEnvelopeFreqFineHandler(httpd_req_t *req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
    int received = 0;
    if (total_len >= SCRATCH_BUFSIZE) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    cJSON *root = cJSON_Parse(buf);
    uint8_t value = (uint8_t)cJSON_GetObjectItem(root, "value")->valueint;
    YM2149_setEnvelopeFreqFine (&value);

    ESP_LOGI(YM2149_REST_TAG, "setEnvelopeFreqFineHandler value:%d", value);
    cJSON_Delete(root);
    httpd_resp_sendstr(req, "Post control value successfully");
    return ESP_OK;
}

static esp_err_t YM2149_setEnvelopeFreqRoughHandler(httpd_req_t *req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
    int received = 0;
    if (total_len >= SCRATCH_BUFSIZE) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    cJSON *root = cJSON_Parse(buf);
    uint8_t value = (uint8_t)cJSON_GetObjectItem(root, "value")->valueint;
    YM2149_setEnvelopeFreqRough (&value);

    ESP_LOGI(YM2149_REST_TAG, "setEnvelopeFreqRoughHandler value:%d", value);
    cJSON_Delete(root);
    httpd_resp_sendstr(req, "Post control value successfully");
    return ESP_OK;
}



static esp_err_t YM2149_setEnvelopeShapeHandler(httpd_req_t *req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
    int received = 0;
    if (total_len >= SCRATCH_BUFSIZE) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    cJSON *root = cJSON_Parse(buf);
    uint8_t env_shape_type = (uint8_t)cJSON_GetObjectItem(root, "env_shape_type")->valueint;
    bool value = (bool)cJSON_GetObjectItem(root, "value")->valueint;
    YM2149_setEnvelopeShape (&env_shape_type, &value);

    ESP_LOGI(YM2149_REST_TAG, "setEnvelopeShapeHandler env_shape_type:%d value:%d", env_shape_type, value);
    cJSON_Delete(root);
    httpd_resp_sendstr(req, "Post control value successfully");
    return ESP_OK;
}
// ############### YM2149 END ###############

esp_err_t YM2149_start_rest_server(const char *base_path)
{
    REST_CHECK(base_path, "wrong base path", err);
    rest_server_context_t *rest_context = calloc(1, sizeof(rest_server_context_t));
    REST_CHECK(rest_context, "No memory for rest context", err);
    strlcpy(rest_context->base_path, base_path, sizeof(rest_context->base_path));

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.max_uri_handlers = 20;

    ESP_LOGI(YM2149_REST_TAG, "Starting HTTP Server");
    REST_CHECK(httpd_start(&server, &config) == ESP_OK, "Start server failed", err_start);

    // ########################### OLD ####################################
    /* URI handler for fetching system info */
    httpd_uri_t system_info_get_uri = {
        .uri = "/api/v1/system/info",
        .method = HTTP_GET,
        .handler = system_info_get_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &system_info_get_uri);

    /* URI handler for fetching temperature data */
    httpd_uri_t temperature_data_get_uri = {
        .uri = "/api/v1/temp/raw",
        .method = HTTP_GET,
        .handler = temperature_data_get_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &temperature_data_get_uri);

    // ########################################## YM2149 ##########################################

    httpd_uri_t channel_freq_fine_post_uri = {
        .uri = "/api/v1/ym2149/channel/freq/fine",
        .method = HTTP_POST,
        .handler = YM2149_setChannelFreqFineHandler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &channel_freq_fine_post_uri);


	httpd_uri_t channel_freq_rough_post_uri = {
		.uri = "/api/v1/ym2149/channel/freq/rough",
		.method = HTTP_POST,
		.handler = YM2149_setChannelFreqRoughHandler,
		.user_ctx = rest_context
	};
	httpd_register_uri_handler(server, &channel_freq_rough_post_uri);

	httpd_uri_t noise_freq_post_uri = {
		.uri = "/api/v1/ym2149/noise/freq",
		.method = HTTP_POST,
		.handler = YM2149_setNoiseFreqHandler,
		.user_ctx = rest_context
	};
	httpd_register_uri_handler(server, &noise_freq_post_uri);

	httpd_uri_t channel_noise_post_uri = {
		.uri = "/api/v1/ym2149/channel/noise",
		.method = HTTP_POST,
		.handler = YM2149_setChannelNoiseHandler,
		.user_ctx = rest_context
	};
	httpd_register_uri_handler(server, &channel_noise_post_uri);

	httpd_uri_t channel_tone_post_uri = {
		.uri = "/api/v1/ym2149/channel/tone",
		.method = HTTP_POST,
		.handler = YM2149_setChannelToneHandler,
		.user_ctx = rest_context
	};
	httpd_register_uri_handler(server, &channel_tone_post_uri);

	httpd_uri_t channel_level_mode_post_uri = {
		.uri = "/api/v1/ym2149/channel/level/mode",
		.method = HTTP_POST,
		.handler = YM2149_setChannelLevelModeHandler,
		.user_ctx = rest_context
	};
	httpd_register_uri_handler(server, &channel_level_mode_post_uri);

	httpd_uri_t channel_level_post_uri = {
		.uri = "/api/v1/ym2149/channel/level",
		.method = HTTP_POST,
		.handler = YM2149_setChannelLevelHandler,
		.user_ctx = rest_context
	};
	httpd_register_uri_handler(server, &channel_level_post_uri);

	httpd_uri_t env_freq_fine_post_uri = {
		.uri = "/api/v1/ym2149/envelope/freq/fine",
		.method = HTTP_POST,
		.handler = YM2149_setEnvelopeFreqFineHandler,
		.user_ctx = rest_context
	};
	httpd_register_uri_handler(server, &env_freq_fine_post_uri);

	httpd_uri_t env_freq_rough_post_uri = {
		.uri = "/api/v1/ym2149/envelope/freq/rough",
		.method = HTTP_POST,
		.handler = YM2149_setEnvelopeFreqRoughHandler,
		.user_ctx = rest_context
	};
	httpd_register_uri_handler(server, &env_freq_rough_post_uri);

	httpd_uri_t env_shape_handler_post_uri = {
		.uri = "/api/v1/ym2149/envelope/shape",
		.method = HTTP_POST,
		.handler = YM2149_setEnvelopeShapeHandler,
		.user_ctx = rest_context
	};
	httpd_register_uri_handler(server, &env_shape_handler_post_uri);

    // ########################################## COMMON ##########################################
    /* URI handler for getting web server files */
    httpd_uri_t common_get_uri = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = rest_common_get_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &common_get_uri);

    return ESP_OK;
err_start:
    free(rest_context);
err:
    return ESP_FAIL;
}
