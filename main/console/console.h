#pragma once

#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"

#include "cmd_version.h"
#include "cmd_join.h"
#include "cmd_npm.h"


#define CONSOLE_HISTORY_PATH "/data/history.txt"


void console_task(void *param);
void console_init(void);