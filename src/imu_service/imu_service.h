
#ifndef IMU_SERVICE_H__
#define IMU_SERVICE_H__

#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"


//34D6xxxx-E9CE-476C-BAD6-FB0F310898E1
#define BLE_UUID_P24_BASE_UUID              {{0xE1, 0x98, 0x08, 0x31, 0x0F, 0xFB, 0xD6, 0xBA, 0x6C, 0x47, 0xCE, 0xE9, 0x00, 0x00, 0xD6, 0x34}} // 128-bit base UUID
#define BLE_UUID_IMU_SERVICE_UUID                0x1520 // Just a random, but recognizable value, IMU
#define BLE_UUID_IMU_STEPS_CHARACTERISTC_UUID    0x51E7 // Just a random, but recognizable value, STEP

// This structure contains various status information for our service. 
typedef struct
{
    uint16_t                    conn_handle;    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).*/
    uint16_t                    service_handle; /**< Handle of imu Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    char_handles;   // handles for the characteristic attributes to our struct
}ble_imu_t;

/**@brief Function for handling BLE Stack events related to our service and characteristic.
 *
 * @details Handles all events from the BLE stack of interest to Our Service.
 *
 * @param[in]   p_our_service       Our Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_imu_service_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

/**@brief Function for initializing our new service.
 *
 * @param[in]   p_our_service       Pointer to Our Service structure.
 */
void imu_service_init(ble_imu_t * p_imu_service);

/**@brief Function for updating and sending new characteristic values
 *
 * @details The application calls this function whenever our timer_timeout_handler triggers
 *
 * @param[in]   p_our_service                     Our Service structure.
 * @param[in]   characteristic_value     New characteristic value.
 */
void imu_steps_characteristic_update(ble_imu_t *p_imu_service, int32_t *steps_value);

#endif  /* _ IMU_SERVICE_H__ */
