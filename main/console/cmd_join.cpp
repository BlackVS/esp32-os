#include "app.h"

/** Arguments used by 'join' function */
static struct {
  struct arg_int *timeout;
  struct arg_str *ssid;
  struct arg_str *password;
  struct arg_end *end;
} join_args;

std::string wifi_join_ssid;
std::string wifi_join_password;
int wifi_join_timeout=0;


esp_err_t _start_internal(void)
{
  esp_err_t err = WiFi.join2AP(wifi_join_ssid.c_str(), wifi_join_password.c_str(), wifi_join_timeout, true);
  return err;
}

esp_err_t _stop_internal(void)
{
    WiFi.set_mode(WiFi_MODE_NONE); 
    return ESP_OK;
}


static int cmd_join(int argc, char **argv) 
{
  int nerrors = arg_parse(argc, argv, (void **)&join_args);
  if (nerrors != 0) {
    arg_print_errors(stderr, join_args.end, argv[0]);
    return 1;
  }

  esp_err_t err=ESP_OK;
  bool bNewCreds=false;

  if(join_args.ssid->count) {
    wifi_join_ssid=join_args.ssid->sval[0];
    bNewCreds=true;
  }
  else {
    err=nvs_settings_getString("wifi-join-ssid", wifi_join_ssid);
    if(err!=ESP_OK) {
      printf("Error: no SSID set!!!");
      return 0;
    }
  }

  if(join_args.password->count) {
    wifi_join_password=join_args.password->sval[0];
    bNewCreds=true;
  }
  else {
    err=nvs_settings_getString("wifi-join-pass", wifi_join_password);
    if(err!=ESP_OK) {
      printf("Warning: no password set");
      wifi_join_password.clear();
    }
  }

  wifi_join_timeout=join_args.timeout->ival[0];

  err = _start_internal();
  if(err==ESP_OK&&WiFi.join2AP_is_connected()&&bNewCreds)
  {
    nvs_settings_setString("wifi-join-ssid", wifi_join_ssid.c_str());
    nvs_settings_setString("wifi-join-pass", wifi_join_password.c_str());
    printf("Credentials saved\n");
  }

  return 0;
}

void register_cmd_join() 
{
  join_args.timeout = arg_int0(NULL, "timeout", "<t>", "Connection timeout, ms");
  join_args.timeout->ival[0] = 15000; // set default value
  join_args.ssid = arg_str0(NULL, NULL, "<ssid>", "SSID of AP");
  join_args.password = arg_str0(NULL, NULL, "<pass>", "PSK of AP");
  join_args.end = arg_end(2);

  const esp_console_cmd_t join_cmd = {.command = "join",
                                      .help = "Join to WiFi AP as a station. If no ssid/password set - try use stored in nvs",
                                      .hint = NULL,
                                      .func = &cmd_join,
                                      .argtable = &join_args};

  ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd));
}




