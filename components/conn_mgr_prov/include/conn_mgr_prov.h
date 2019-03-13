/* Unified Provisioning Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_event_loop.h>
#include <protocomm.h>


typedef enum {
    CM_ENDPOINT_CONFIG,
    CM_ENDPOINT_ADD,
    CM_ENDPOINT_REMOVE,
    CM_PROV_START,
    CM_PROV_END,
} conn_mgr_cb_event_t;

typedef struct {
    esp_err_t (*prov_start) (protocomm_t *pc, void *config);
    esp_err_t (*prov_stop)  (protocomm_t *pc);
    void     *(*new_config) (void);
    void      (*delete_config) (void *config);
    esp_err_t (*set_config_service) (void *config, const char *service_name, const char *service_key);
    esp_err_t (*set_config_endpoint) (void *config, const char *endpoint_name, uint16_t uuid);
    int       wifi_mode;
    int       (*event_cb)(void *user_data, conn_mgr_cb_event_t event);
    void      *cb_user_data;
} conn_mgr_prov_t;

/**
 * @brief   Event handler for provisioning app
 *
 * This is called from the main event handler and controls the
 * provisioning application, depeding on WiFi events
 *
 * @param[in] ctx   Event context data
 * @param[in] event Event info
 *
 * @return
 *  - ESP_OK      : Event handled successfully
 *  - ESP_FAIL    : Failed to start server on event AP start
 */
esp_err_t conn_mgr_prov_event_handler(void *ctx, system_event_t *event);

/**
 * @brief   Checks if device is provisioned
 * *
 * @param[out] provisioned  True if provisioned, else false
 *
 * @return
 *  - ESP_OK      : Retrieved provision state successfully
 *  - ESP_FAIL    : Failed to retrieve provision state
 */
esp_err_t conn_mgr_prov_is_provisioned(bool *provisioned);

/**
 * @brief   Start provisioning
 *
 * @param[in] mode          Provisioning mode to use (BLE/SoftAP)
 * @param[in] security      Security mode
 * @param[in] pop           Pointer to proof of possession (NULL if not needed)
 * @param[in] service_name  Unique name of the service. This translates to:
 *                              - WiFi SSID when provisioning mode is softAP
 *                              - Device name when provisioning mode is BLE
 * @param[in] service_key   Key required by client to access the service (NULL if not needed).
 *                          This translates to:
 *                              - WiFi password when provisioning mode is softAP
 *                              - ignored when provisioning mode is BLE
 *
 * @return
 *  - ESP_OK      : Provisioning started successfully
 *  - ESP_FAIL    : Failed to start
 */
esp_err_t conn_mgr_prov_start_provisioning(conn_mgr_prov_t mode, int security, const char *pop,
        const char *service_name, const char *service_key);

/**
 * @brief   Configure an extra endpoint
 *
 * This API can be called by the application if it wants to configure an extra endpoint. The
 * API must be called when the application gets the callback with event CM_ENDPOINT_CONFIG.
 *
 * @param[in] ep_name  Name of the endpoint
 */
void conn_mgr_prov_endpoint_configure(const char *ep_name);

/**
 * @brief   Add a configured endpoint
 *
 * This API can be called by the application if it wants to add i.e. enable the endpoint. The
 * API must be called when the application gets the callback with event CM_ENDPOINT_ADD.
 *
 * @param[in] ep_name   Name of the endpoint
 * @param[in] handler   Pointer to the endpoint handler
 * @param[in] user_ctx  User data
 */
void conn_mgr_prov_endpoint_add(const char *ep_name, protocomm_req_handler_t handler, void *user_ctx);

/**
 * @brief   Remove an endpoint
 *
 * This API can be called if the application wants to selectively remove an endpoint while the
 * provisioning is still in progress.
 * All the endpoints are removed automatically when the provisioning stops.
 *
 * @param[in] ep_name  Name of the endpoint
 */
void conn_mgr_prov_endpoint_remove(const char *ep_name);

/**
 * @brief   Release the memory used by provisioning service
 *
 * If device is already found to be provisioned (ie. provisioning service
 * doesn't need to be started) then call this API to free the memory used
 * by both BT and BLE stack in case conn_mgr_prov_mode_ble mode is selected.
 */
void conn_mgr_prov_mem_release();
