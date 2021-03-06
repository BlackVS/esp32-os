#pragma once

#define DEFAULT_SCAN_LIST_SIZE 100
#define DEFAULT_ESP_WIFI_STA_MAXIMUM_RETRY 5

#define WiFi_JOIN2AP_BIT_CONNECTED  BIT0
#define WiFi_JOIN2AP_BIT_FAILED     BIT1
#define WiFi_JOIN2AP_BITS_ALL       BIT0 + BIT1

#define WIFI_MAX_CH 14      
#define WIFI_SNAP_LEN 2324   // max len of each recieved packet


//#include "oui.h"

// #include <stdio.h>
// #include <vector>
// #include <cstdint>
#undef min
#undef max
#include <string>

// //#include "log.h"
// #include "esp_system.h"
// #include "esp_spi_flash.h"
// #include "esp_tls.h"
// #include "esp_http_client.h"
// #include "esp_https_ota.h"
// #include "esp_ota_ops.h"

// ///// netif not supports C++ compiler yet
// #ifdef __cplusplus
// extern "C" {
// #endif

#include "esp_wifi.h"
// #include "esp_event_loop.h"
#include "esp_sntp.h"

// //#include "lwip/apps/sntp.h"
// //#include "lwip/err.h"

// #ifdef __cplusplus
// }
// #endif

// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/semphr.h"
#include "freertos/event_groups.h"



typedef enum {
     WiFi_EVENT_UNKNOWN=0,
     WiFi_EVENT_SUBSCRIBED, 
     WiFi_EVENT_UNSUBSCRIBED, 
     WiFi_EVENT_MODE_CHANGING, //starting to change mode
     WiFi_EVENT_MODE_CHANGED, //just changed mode
} WiFi_EVENT;

typedef enum {
    WiFi_JOIN2AP_IDLE=0,
    WiFi_JOIN2AP_CONNECTING,
    WiFi_JOIN2AP_CONNECTED,
    WiFi_JOIN2AP_FAILED,
} WiFi_JOIN2AP_STATE;

extern "C" typedef void (*WiFi_PROMISCUOUS_CALLBACK)(void* buf, wifi_promiscuous_pkt_type_t type);
typedef void (*WiFi_EVENTS_CALLBACK)(WiFi_EVENT ev, uint32_t arg);


typedef enum {
//compatibale with wifi_mode_t modes
    WiFi_MODE_NONE = 0,  /**< null mode */
    WiFi_MODE_STA,       /**< WiFi station mode */
    WiFi_MODE_AP,        /**< WiFi soft-AP mode */
    WiFi_MODE_APSTA,     /**< WiFi station + soft-AP mode */
//
    WiFi_MODE_PROMISCUOUS,
    //
    WiFi_MODE_MAX
} WiFi_MODES;

//typedef uint8_t mac_t[6];
typedef struct _mac_struct {
    uint8_t addr[6];
    _mac_struct()
    {
        addr[0]=addr[1]=addr[2]=addr[3]=addr[4]=addr[5]=0;
    }

    _mac_struct(uint8_t* buf)//unsafe!!!
    {
        addr[0]=buf[0];
        addr[1]=buf[1];
        addr[2]=buf[2];
        addr[3]=buf[3];
        addr[4]=buf[4];
        addr[5]=buf[5];
    }
    
    _mac_struct(uint64_t i)
    {
        uint8_t* buf=(uint8_t*)&i;
        addr[0]=buf[0];
        addr[1]=buf[1];
        addr[2]=buf[2];
        addr[3]=buf[3];
        addr[4]=buf[4];
        addr[5]=buf[5];
    }

    uint64_t to_int(void)
    {
        uint64_t r = 0;
        r+=(uint64_t)addr[5];
        r<<=8;
        r+=(uint64_t)addr[4];
        r<<=8;
        r+=(uint64_t)addr[3];
        r<<=8;
        r+=(uint64_t)addr[2];
        r<<=8;
        r+=(uint64_t)addr[1];
        r<<=8;
        r+=(uint64_t)addr[0];
        return r;
    }

    bool is_broadcast(){
        return addr[0]==255 && addr[1]==255 && addr[2]==255 && addr[3]==255 && addr[4]==255 && addr[5]==255;
    }

    bool is_multicast(){
        // see https://en.wikipedia.org/wiki/Multicast_address
        if ((addr[0] == 0x33) && (addr[1] == 0x33)) return true;
        if ((addr[0] == 0x01) && (addr[1] == 0x80) && (addr[2] == 0xC2)) return true;
        if ((addr[0] == 0x01) && (addr[1] == 0x00) && ((addr[2] == 0x5E) || (addr[2] == 0x0C))) return true;
        if ((addr[0] == 0x01) && (addr[1] == 0x0C) && (addr[2] == 0xCD) &&
            ((addr[3] == 0x01) || (addr[3] == 0x02) || (addr[3] == 0x04)) &&
            ((addr[4] == 0x00) || (addr[4] == 0x01))) return true;
        if ((addr[0] == 0x01) && (addr[1] == 0x00) && (addr[2] == 0x0C) && (addr[3] == 0xCC) && (addr[4] == 0xCC) &&
            ((addr[5] == 0xCC) || (addr[5] == 0xCD))) return true;
        if ((addr[0] == 0x01) && (addr[1] == 0x1B) && (addr[2] == 0x19) && (addr[3] == 0x00) && (addr[4] == 0x00) &&
            (addr[5] == 0x00)) return true;
        return false;
    }

    bool is_valid(){
        for (uint8_t i = 0; i < 6; i++)
            if (addr[i] != 0x00) return true;
        return false;
    }

    uint8_t* buf() { return addr; }

} mac_t;

typedef struct
{
		uint16_t protocol:2;
		uint16_t type:2;
		uint16_t subtype:4;
		uint16_t to_ds:1;
		uint16_t from_ds:1;
		uint16_t more_frag:1;
		uint16_t retry:1;
		uint16_t pwr_mgmt:1;
		uint16_t more_data:1;
		uint16_t wep:1;
		uint16_t strict:1;
} wifi_header_frame_control_t;

typedef enum
{
		WiFi_MGMT_ASSOCIATION_REQ,
		WiFi_MGMT_ASSOCIATION_RES,
		WiFi_MGMT_REASSOCIATION_REQ,
		WiFi_MGMT_REASSOCIATION_RES,
		WiFi_MGMT_PROBE_REQ,
		WiFi_MGMT_PROBE_RES,
		WiFi_MGMT_NU1,	/* ......................*/
		WiFi_MGMT_NU2,	/* 0110, 0111 not used */
		WiFi_MGMT_BEACON,
		WiFi_MGMT_ATIM,
		WiFi_MGMT_DISASSOCIATION,
		WiFi_MGMT_AUTHENTICATION,
		WiFi_MGMT_DEAUTHENTICATION,
		WiFi_MGMT_ACTION,
		WiFi_MGMT_ACTION_NACK,
} wifi_mgmt_subtypes_t;

typedef struct {
	//uint16_t frame_ctrl;
    wifi_header_frame_control_t frame_ctrl;
	uint16_t duration_id;
	mac_t    mac_to; /* receiver address */
	mac_t    mac_from; /* sender address */
	mac_t    mac_mask; /* filtering address */
	uint16_t sequence_ctrl;
	uint8_t  addr4[6]; /* optional */
} wifi_ieee80211_mac_hdr_t;

typedef struct {
	wifi_ieee80211_mac_hdr_t hdr;
	uint8_t payload[0]; /* network data ended with 4 bytes csum (CRC32) */
} wifi_ieee80211_packet_t;

class CWiFi
{
    WiFi_MODES mode;
    wifi_country_t wf_country;
    uint32_t channel;
    //promiscuous
    WiFi_PROMISCUOUS_CALLBACK _promiscuous_cb;
    WiFi_EVENTS_CALLBACK      _events_cb;
    //join2AP
    WiFi_JOIN2AP_STATE     _join2ap_state;
    uint32_t            _join2ap_retries;
protected:
    EventGroupHandle_t  wifi_event_group;

public:
    CWiFi();
    void init(void);

    //mode
    esp_err_t set_mode(WiFi_MODES m, wifi_bandwidth_t bandwidth=WIFI_BW_HT40, wifi_config_t *cfg=NULL);
    WiFi_MODES get_mode(void);
    const char * mode2str(WiFi_MODES mode);

    //events
    void set_event_handler(WiFi_EVENTS_CALLBACK cb);

    //channel
    void set_channel(uint32_t ch, bool bForce=false);
    uint32_t get_channel(void);

    void set_promiscuous_callback(WiFi_PROMISCUOUS_CALLBACK callback);

    //scan APs
    uint32_t scan_APs(void); //return a number of found APs
    int  scan_APs_get_count(void);
    bool scan_APs_get_data(uint32_t idx, wifi_ap_record_t& ap);

    //join to AP
    esp_err_t           join2AP(const char* ssid, const char* pass, int timeout, bool bWait=true);
    WiFi_JOIN2AP_STATE  join2AP_getstate(void) {return _join2ap_state;}
    bool                join2AP_is_connected(void) {return _join2ap_state==WiFi_JOIN2AP_CONNECTED;}

    mac_t get_mac(wifi_interface_t ifx=WIFI_IF_AP);

//friend void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
//friend void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
friend esp_err_t wifi_event_handler(void* ctx, system_event_t *event);
};

extern CWiFi WiFi;

//static tools
const char* authmode2str(wifi_auth_mode_t authmode);
const char* cipher2str(wifi_cipher_type_t cipher);
std::string mac2str(mac_t mac);
uint32_t    mac2str_n(char* buf, uint32_t bufsize, mac_t mac);
uint64_t mac2int(mac_t mac);
void     int2mac(uint64_t i, mac_t& mac);

//http
esp_err_t http_get(const char *url, std::string& data);

//extern CTaskPool pool_wifi_tasks;


void set_ntp();



