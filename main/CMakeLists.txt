idf_component_register(SRCS "esp_rest_main.c"
							"rest_common.c" 
                            "rest_ym2149.c"
                            "led_handler.c"
                            "ym2149.c"
                    INCLUDE_DIRS ".")

if(CONFIG_EXAMPLE_WEB_DEPLOY_SF)
    set(WEB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../front/ym2149")
    if(EXISTS ${WEB_SRC_DIR})
        spiffs_create_partition_image(www ${WEB_SRC_DIR} FLASH_IN_PROJECT)
    else()
        message(FATAL_ERROR "${WEB_SRC_DIR} doesn't exit. Please check content in ${WEB_SRC_DIR}")
    endif()
endif()
