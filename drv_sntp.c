/* *****************************************************************************
 * File:   drv_sntp.c
 * Author: Dimitar Lilov
 *
 * Created on 2022 06 18
 * 
 * Description: ...
 * 
 **************************************************************************** */

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "drv_sntp.h"

#include <stdbool.h>
#include "esp_log.h"
#include "esp_sntp.h"
#include "esp_event.h"

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define TAG "drv_sntp"

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */
sntp_sync_time_cb_t sync_time_cb_request = NULL;


void drv_sntp_init(sntp_sync_time_cb_t sync_time_cb)
{
    sync_time_cb_request = sync_time_cb;

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,2,0)
    if (esp_sntp_enabled())
    {
        ESP_LOGI(TAG, "Stop before Initializing SNTP");
        esp_sntp_stop();
    }
    ESP_LOGI(TAG, "Initializing SNTP");
        /**
         * NTP server address could be aquired via DHCP,
         * see LWIP_DHCP_GET_NTP_SRV menuconfig option
         */
    #if LWIP_DHCP_GET_NTP_SRV
        esp_sntp_servermode_dhcp(1);
    #endif

    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_setservername(0, "solo.cmsmon.com");
    esp_sntp_set_time_sync_notification_cb(sync_time_cb);
    #ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
    #endif
    esp_sntp_init();
#else
    if (sntp_enabled())
    {
        ESP_LOGI(TAG, "Stop before Initializing SNTP");
        sntp_stop();
    }
    ESP_LOGI(TAG, "Initializing SNTP");
        /**
         * NTP server address could be aquired via DHCP,
         * see LWIP_DHCP_GET_NTP_SRV menuconfig option
         */
    #ifdef LWIP_DHCP_GET_NTP_SRV
        sntp_servermode_dhcp(1);
    #endif

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_setservername(0, "solo.cmsmon.com");
    sntp_set_time_sync_notification_cb(sync_time_cb);
    #ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
    #endif
    sntp_init();
#endif

}

void drv_sntp_set_time_manual(bool input)
{
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,2,0)
    if (input)
    {
        if (esp_sntp_enabled())
        {
            esp_sntp_stop();
        }
    }
    else
    {
        if (esp_sntp_enabled() == 0)
        {
            drv_sntp_init(sync_time_cb_request);
        }

    }
#else
    if (input)
    {
        if (sntp_enabled())
        {
            sntp_stop();
        }
    }
    else
    {
        if (sntp_enabled() == 0)
        {
            drv_sntp_init(sync_time_cb_request);
        }

    }
#endif
}
