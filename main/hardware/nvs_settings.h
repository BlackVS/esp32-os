#pragma once

#include "nvs.h"
#include "nvs_flash.h"

#define NVS_NAMESPACE "nvs_data"


void        nvs_initialize(void);
void        nvs_settings_setU32(const char *name, uint32_t value);
uint32_t    nvs_settings_getU32(const char *name);
void        nvs_settings_setI32(const char *name, int32_t value);
int32_t     nvs_settings_getI32(const char *name);
esp_err_t   nvs_settings_setString(const char *name, const char *value);
esp_err_t   nvs_settings_setString(const char *name, std::string  value);
esp_err_t   nvs_settings_getString(const char *name, std::string& value);
esp_err_t   nvs_settings_getString(const char *name, char *value, size_t maxLen);