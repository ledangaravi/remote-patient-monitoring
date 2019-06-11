#include <stdint.h>

#include "nordic_common.h"
#include "nrf.h"
#include "nrf_sdm.h"
#include "app_error.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "app_timer.h"

#include "nrfx_gpiote.h"
#include "nrfx_twim.h"
#include "nrf_delay.h"
#include "nrf_drv_timer.h"

#include "ble.h"
#include "ble_err.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_bas.h"
#include "ble_hrs.h"
#include "ble_dis.h"
#include "ble_conn_params.h"
#include "nrf_sdh_ble.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "fds.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_lesc.h"
#include "nrf_ble_qwr.h"
#include "ble_conn_state.h"

#include "sensorsim.h"


extern "C" {
  #include "nrf_pwr_mgmt.h"
}

#include "BH1792GLC.h"
#include "BH1792GLC_registers.h"

#include "i2c.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "arm_const_structs.h"

#include "DWT.h"

#include "lsm6dsl_reg.h"



#define DEVICE_NAME                         "Patient 24"                            /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME                   "Your Mom"                   /**< Manufacturer. Will be passed to Device Information Service. */
#define APP_ADV_INTERVAL                    300                                     /**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */

#define APP_ADV_DURATION                    18000                                   /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define APP_BLE_CONN_CFG_TAG                1                                       /**< A tag identifying the SoftDevice BLE configuration. */
#define APP_BLE_OBSERVER_PRIO               3                                       /**< Application's BLE observer priority. You shouldn't need to modify this value. */

#define BATTERY_LEVEL_MEAS_INTERVAL         APP_TIMER_TICKS(2000)                   /**< Battery level measurement interval (ticks). */
#define MIN_BATTERY_LEVEL                   81                                      /**< Minimum simulated battery level. */
#define MAX_BATTERY_LEVEL                   100                                     /**< Maximum simulated 7battery level. */
#define BATTERY_LEVEL_INCREMENT             1                                       /**< Increment between each simulated battery level measurement. */

#define HEART_RATE_MEAS_INTERVAL            APP_TIMER_TICKS(2000)                   /**< Heart rate measurement interval (ticks). */
#define PPG_INTERVAL                        APP_TIMER_TICKS(14000)


#define RR_INTERVAL_INTERVAL                APP_TIMER_TICKS(300)                    /**< RR interval interval (ticks). */
#define MIN_RR_INTERVAL                     100                                     /**< Minimum RR interval as returned by the simulated measurement function. */
#define MAX_RR_INTERVAL                     500                                     /**< Maximum RR interval as returned by the simulated measurement function. */
#define RR_INTERVAL_INCREMENT               1                                       /**< Value by which the RR interval is incremented/decremented for each call to the simulated measurement function. */

#define SENSOR_CONTACT_DETECTED_INTERVAL    APP_TIMER_TICKS(5000)                   /**< Sensor Contact Detected toggle interval (ticks). */

#define MIN_CONN_INTERVAL                   MSEC_TO_UNITS(400, UNIT_1_25_MS)        /**< Minimum acceptable connection interval (0.4 seconds). */
#define MAX_CONN_INTERVAL                   MSEC_TO_UNITS(650, UNIT_1_25_MS)        /**< Maximum acceptable connection interval (0.65 second). */
#define SLAVE_LATENCY                       0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                    MSEC_TO_UNITS(4000, UNIT_10_MS)         /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY      APP_TIMER_TICKS(5000)                   /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY       APP_TIMER_TICKS(30000)                  /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT        3                                       /**< Number of attempts before giving up the connection parameter negotiation. */

#define LESC_DEBUG_MODE                     0                                       /**< Set to 1 to use LESC debug keys, allows you to use a sniffer to inspect traffic. */

#define SEC_PARAM_BOND                      1                                       /**< Perform bonding. */
#define SEC_PARAM_MITM                      0                                       /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                      1                                       /**< LE Secure Connections enabled. */
#define SEC_PARAM_KEYPRESS                  0                                       /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES           BLE_GAP_IO_CAPS_NONE                    /**< No I/O capabilities. */
#define SEC_PARAM_OOB                       0                                       /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE              7                                       /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE              16                                      /**< Maximum encryption key size. */

#define DEAD_BEEF                           0xDEADBEEF                              /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define FPU_EXCEPTION_MASK 0x0000009F
#define FPU_FPSCR_REG_STACK_OFF 0x40



BLE_HRS_DEF(m_hrs);                                                 /**< Heart rate service instance. */
BLE_BAS_DEF(m_bas);                                                 /**< Structure used to identify the battery service. */
NRF_BLE_GATT_DEF(m_gatt);                                           /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                             /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);                                 /**< Advertising module instance. */
APP_TIMER_DEF(m_battery_timer_id);                                  /**< Battery timer. */
APP_TIMER_DEF(m_heart_rate_timer_id);                               /**< Heart rate measurement timer. */
APP_TIMER_DEF(m_rr_interval_timer_id);                              /**< RR interval timer. */
APP_TIMER_DEF(m_sensor_contact_timer_id);                           /**< Sensor contact detected timer. */
APP_TIMER_DEF(m_ppg_timer_id);

static uint16_t m_conn_handle         = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */
static bool     m_rr_interval_enabled = true;                       /**< Flag for enabling and disabling the registration of new RR interval measurements (the purpose of disabling this is just to test sending HRM without RR interval data. */

static sensorsim_cfg_t   m_battery_sim_cfg;                         /**< Battery Level sensor simulator configuration. */
static sensorsim_state_t m_battery_sim_state;                       /**< Battery Level sensor simulator state. */
static sensorsim_cfg_t   m_rr_interval_sim_cfg;                     /**< RR Interval sensor simulator configuration. */
static sensorsim_state_t m_rr_interval_sim_state;                   /**< RR Interval sensor simulator state. */

static ble_uuid_t m_adv_uuids[] =                                   /**< Universally unique service identifiers. */
{
    {BLE_UUID_HEART_RATE_SERVICE,           BLE_UUID_TYPE_BLE},
    {BLE_UUID_BATTERY_SERVICE,              BLE_UUID_TYPE_BLE},
    {BLE_UUID_DEVICE_INFORMATION_SERVICE,   BLE_UUID_TYPE_BLE}
};


#define BH1792_INT_PIN     15

/* TWI instance ID. */
#define TWI_INSTANCE_ID     0

#define FPU_EXCEPTION_MASK               0x0000009F                      //!< FPU exception mask used to clear exceptions in FPSCR register.
#define FPU_FPSCR_REG_STACK_OFF 0x40 


/* TWI instance. */
static const nrfx_twim_t m_twi = NRFX_TWIM_INSTANCE(TWI_INSTANCE_ID);

static const I2C m_i2c = I2C(&m_twi);

static const nrfx_timer_t TIMER_PPG_SYNC = NRF_DRV_TIMER_INSTANCE(1);

static uint8_t ctr = 0;

uint32_t previous_tick = 0;
uint32_t current_tick = 0;
static uint16_t step_count = 0;

//NRF_ATFIFO_DEF(sample_fifo, uint16_t, 1116U);

static float32_t samples[1024];
static float32_t PSD[1024];
static uint16_t sample_counter = 0;

BH1792GLC m_bh1792 = BH1792GLC(&m_i2c);


static bool sensor_contact_detected = true;
static bool bh_finished = true;
static bool bh_started = false;
static uint8_t heart_rate = 0;



static arm_rfft_fast_instance_f32 fft_instance;



void timer_sync_event_handler(nrf_timer_event_t event_type, void* p_context)
{
    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:
            if(ctr == 5U)
            {
              if (m_bh1792.prm.msr <= BH1792_PRM_MSR_1024HZ) {
              m_bh1792.send_sync();
          
              if (m_bh1792.sync_seq < 3) {
                if (m_bh1792.sync_seq != 1) {
                  m_bh1792.clear_fifo();
                }
               }
              }
              ctr = 0;
            }
            
            if(m_bh1792.prm.msr <= BH1792_PRM_MSR_1024HZ) {
              m_bh1792.get_measurement_data();
              uint8_t fifo_l = m_bh1792.m_dat.fifo_lev;
              if(sample_counter >= 1024)
              {
                bh_finished = true;
                m_bh1792.stop_measurement();
                nrfx_timer_pause(&TIMER_PPG_SYNC);
                ctr = 0;
                return;
              }
              else
              {
                if(sample_counter + fifo_l < 1024)
                {
                  memcpy(&samples[sample_counter], &m_bh1792.m_dat.fifo, fifo_l * sizeof(float32_t));
                }
                else
                {
                  memcpy(&samples[sample_counter], &m_bh1792.m_dat.fifo, (1024 - sample_counter) * sizeof(float32_t));
                }
              }
              sample_counter += fifo_l;
              m_bh1792.clear_fifo();
            }
            ctr++;
            
            break;
        default:
            //Do nothing.
            break;
    }
}


void in_pin_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
     nrfx_gpiote_out_toggle(20);
}



void process_PPG()
{
      /*//Applying Blackman-Harris window
      for (int i = 0; i < 1024; i++)
      {
        float32_t w = 0.35875f - 0.48829f*arm_cos_f32(0.00613592315*i) + 0.14128*arm_cos_f32(0.0122718463*i) - 0.01168*arm_cos_f32(0.01840776945*i);
        samples[i] = w*samples[i]; 
      }

      arm_rfft_fast_f32(&fft_instance, samples, PSD, 0);
      arm_cmplx_mag_f32(PSD, PSD, 1024);

      uint8_t max_ind = 0U;
      float32_t max_val = 0.0f;

      for(int i = 6; i < 24; i++)
      {
        if(PSD[i] > max_val)
        {
          max_ind = i;
          max_val = PSD[i];
        }
      }
      
      heart_rate = (int)(7.5f*max_ind);*/

      wave_object wo = wave_object();
      wt_object wt = wt_object(&wo, 1024, 8);
      wt.dwt(samples);


      uint32_t fpscr_reg = __get_FPSCR();
      __set_FPSCR(fpscr_reg & ~(0x0000009F));
      (void) __get_FPSCR();
      NVIC_ClearPendingIRQ(FPU_IRQn);
}

void ppg_reinit(void)
{
    ret_code_t err_code;

    m_bh1792.init();


    m_bh1792.prm.sel_adc  = BH1792_PRM_SEL_ADC_GREEN;
    m_bh1792.prm.msr      = BH1792_PRM_MSR_128HZ;
    m_bh1792.prm.led_en   = (BH1792_PRM_LED_EN1_0 << 1);// | BH1792_PRM_LED_EN2_0;
    m_bh1792.prm.led_cur1 = 10;
    m_bh1792.prm.led_cur2 = 0;
    m_bh1792.prm.ir_th    = 0;
    m_bh1792.prm.int_sel  = BH1792_PRM_INT_SEL_SGL;
    m_bh1792.interrupt_handler = &in_pin_handler;
    m_bh1792.p_int        = BH1792_INT_PIN;
    m_bh1792.configure();

    m_bh1792.clear_fifo();
}

void ppg_init(void)
{
    ret_code_t err_code;

    m_bh1792.init();


    m_bh1792.prm.sel_adc  = BH1792_PRM_SEL_ADC_GREEN;
    m_bh1792.prm.msr      = BH1792_PRM_MSR_128HZ;
    m_bh1792.prm.led_en   = (BH1792_PRM_LED_EN1_0 << 1);// | BH1792_PRM_LED_EN2_0;
    m_bh1792.prm.led_cur1 = 10;
    m_bh1792.prm.led_cur2 = 0;
    m_bh1792.prm.ir_th    = 0;
    m_bh1792.prm.int_sel  = BH1792_PRM_INT_SEL_SGL;
    m_bh1792.interrupt_handler = &in_pin_handler;
    m_bh1792.p_int        = BH1792_INT_PIN;
    m_bh1792.configure();

    m_bh1792.clear_fifo();

    //NRF_ATFIFO_INIT(sample_fifo);

    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_cfg.interrupt_priority = 3;
    err_code = nrfx_timer_init(&TIMER_PPG_SYNC, &timer_cfg, timer_sync_event_handler);
    APP_ERROR_CHECK(err_code);

    uint32_t time_ms = 200U;
    uint32_t time_ticks;

    time_ticks = nrfx_timer_ms_to_ticks(&TIMER_PPG_SYNC, time_ms);

    nrfx_timer_extended_compare(
         &TIMER_PPG_SYNC, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

    nrfx_timer_enable(&TIMER_PPG_SYNC);
    nrfx_timer_pause(&TIMER_PPG_SYNC);
}



/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Clear bond information from persistent storage.
 */
static void delete_bonds(void)
{
    ret_code_t err_code;

    NRF_LOG_INFO("Erase bonds!");

    err_code = pm_peers_delete();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting advertising.
 */
void advertising_start(bool erase_bonds)
{
    if (erase_bonds == true)
    {
        delete_bonds();
        // Advertising is started by PM_EVT_PEERS_DELETE_SUCCEEDED event.
    }
    else
    {
        ret_code_t err_code;

        err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    pm_handler_on_pm_evt(p_evt);
    pm_handler_flash_clean(p_evt);

    switch (p_evt->evt_id)
    {
        case PM_EVT_PEERS_DELETE_SUCCEEDED:
            advertising_start(false);
            break;

        default:
            break;
    }
}


/**@brief Function for performing battery measurement and updating the Battery Level characteristic
 *        in Battery Service.
 */
static void battery_level_update(void)
{
    ret_code_t err_code;
    uint8_t  battery_level;

    battery_level = (uint8_t)sensorsim_measure(&m_battery_sim_state, &m_battery_sim_cfg);

    err_code = ble_bas_battery_level_update(&m_bas, battery_level, BLE_CONN_HANDLE_ALL);
    if ((err_code != NRF_SUCCESS) &&
        (err_code != NRF_ERROR_INVALID_STATE) &&
        (err_code != NRF_ERROR_RESOURCES) &&
        (err_code != NRF_ERROR_BUSY) &&
        (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
       )
    {
        APP_ERROR_HANDLER(err_code);
    }
}


/**@brief Function for handling the Battery measurement timer timeout.
 *
 * @details This function will be called each time the battery level measurement timer expires.
 *
 * @param[in] p_context  Pointer used for passing some arbitrary information (context) from the
 *                       app_start_timer() call to the timeout handler.
 */
static void battery_level_meas_timeout_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);
    battery_level_update();
}


/**@brief Function for handling the Heart rate measurement timer timeout.
 *
 * @details This function will be called each time the heart rate measurement timer expires.
 *          It will exclude RR Interval data from every third measurement.
 *
 * @param[in] p_context  Pointer used for passing some arbitrary information (context) from the
 *                       app_start_timer() call to the timeout handler.
 */
static void heart_rate_meas_timeout_handler(void * p_context)
{
    ret_code_t      err_code;

    UNUSED_PARAMETER(p_context);
     
    err_code = ble_hrs_heart_rate_measurement_send(&m_hrs, heart_rate);
    if ((err_code != NRF_SUCCESS) &&
        (err_code != NRF_ERROR_INVALID_STATE) &&
        (err_code != NRF_ERROR_RESOURCES) &&
        (err_code != NRF_ERROR_BUSY) &&
        (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
       )
    {
        APP_ERROR_HANDLER(err_code);
    }
    
}

static void ppg_meas_timeout_handler(void * p_context)
{
    ret_code_t      err_code;

    UNUSED_PARAMETER(p_context);

    if(bh_finished)
    {
      if(bh_started)
      {
        process_PPG();
      }
      nrf_gpio_pin_toggle(20);
      ppg_reinit();
      sample_counter = 0;
      bh_finished = false;
      bh_started = true;
      nrfx_timer_clear(&TIMER_PPG_SYNC);
      nrfx_timer_resume(&TIMER_PPG_SYNC);
      m_bh1792.start_measurement();
    }    
}


/**@brief Function for handling the RR interval timer timeout.
 *
 * @details This function will be called each time the RR interval timer expires.
 *
 * @param[in] p_context  Pointer used for passing some arbitrary information (context) from the
 *                       app_start_timer() call to the timeout handler.
 */
static void rr_interval_timeout_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);

    if (m_rr_interval_enabled)
    {
        uint16_t rr_interval;

        rr_interval = (uint16_t)sensorsim_measure(&m_rr_interval_sim_state,
                                                  &m_rr_interval_sim_cfg);
        ble_hrs_rr_interval_add(&m_hrs, rr_interval);
        rr_interval = (uint16_t)sensorsim_measure(&m_rr_interval_sim_state,
                                                  &m_rr_interval_sim_cfg);
        ble_hrs_rr_interval_add(&m_hrs, rr_interval);
        rr_interval = (uint16_t)sensorsim_measure(&m_rr_interval_sim_state,
                                                  &m_rr_interval_sim_cfg);
        ble_hrs_rr_interval_add(&m_hrs, rr_interval);
        rr_interval = (uint16_t)sensorsim_measure(&m_rr_interval_sim_state,
                                                  &m_rr_interval_sim_cfg);
        ble_hrs_rr_interval_add(&m_hrs, rr_interval);
        rr_interval = (uint16_t)sensorsim_measure(&m_rr_interval_sim_state,
                                                  &m_rr_interval_sim_cfg);
        ble_hrs_rr_interval_add(&m_hrs, rr_interval);
        rr_interval = (uint16_t)sensorsim_measure(&m_rr_interval_sim_state,
                                                  &m_rr_interval_sim_cfg);
        ble_hrs_rr_interval_add(&m_hrs, rr_interval);
    }
}


/**@brief Function for handling the Sensor Contact Detected timer timeout.
 *
 * @details This function will be called each time the Sensor Contact Detected timer expires.
 *
 * @param[in] p_context  Pointer used for passing some arbitrary information (context) from the
 *                       app_start_timer() call to the timeout handler.
 */
static void sensor_contact_detected_timeout_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);

    

    ble_hrs_sensor_contact_detected_update(&m_hrs, sensor_contact_detected);
}


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{
    ret_code_t err_code;

    // Initialize timer module.
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    // Create timers.
    err_code = app_timer_create(&m_battery_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                battery_level_meas_timeout_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_heart_rate_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                heart_rate_meas_timeout_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_rr_interval_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                rr_interval_timeout_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_sensor_contact_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                sensor_contact_detected_timeout_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_ppg_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                ppg_meas_timeout_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_HEART_RATE_SENSOR_HEART_RATE_BELT);
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief GATT module event handler.
 */
static void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
    if (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED)
    {
        NRF_LOG_INFO("GATT ATT MTU on connection 0x%x changed to %d.",
                     p_evt->conn_handle,
                     p_evt->params.att_mtu_effective);
    }

    ble_hrs_on_gatt_evt(&m_hrs, p_evt);
}


/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing services that will be used by the application.
 *
 * @details Initialize the Heart Rate, Battery and Device Information services.
 */
static void services_init(void)
{
    ret_code_t         err_code;
    ble_hrs_init_t     hrs_init;
    ble_bas_init_t     bas_init;
    ble_dis_init_t     dis_init;
    nrf_ble_qwr_init_t qwr_init = {0};
    uint8_t            body_sensor_location;

    // Initialize Queued Write Module.
    qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Heart Rate Service.
    body_sensor_location = BLE_HRS_BODY_SENSOR_LOCATION_FINGER;

    memset(&hrs_init, 0, sizeof(hrs_init));

    hrs_init.evt_handler                 = NULL;
    hrs_init.is_sensor_contact_supported = true;
    hrs_init.p_body_sensor_location      = &body_sensor_location;

    // Here the sec level for the Heart Rate Service can be changed/increased.
    hrs_init.hrm_cccd_wr_sec = SEC_OPEN;
    hrs_init.bsl_rd_sec      = SEC_OPEN;

    err_code = ble_hrs_init(&m_hrs, &hrs_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Battery Service.
    memset(&bas_init, 0, sizeof(bas_init));

    bas_init.evt_handler          = NULL;
    bas_init.support_notification = true;
    bas_init.p_report_ref         = NULL;
    bas_init.initial_batt_level   = 100;

    // Here the sec level for the Battery Service can be changed/increased.
    bas_init.bl_rd_sec        = SEC_OPEN;
    bas_init.bl_cccd_wr_sec   = SEC_OPEN;
    bas_init.bl_report_rd_sec = SEC_OPEN;

    err_code = ble_bas_init(&m_bas, &bas_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Device Information Service.
    memset(&dis_init, 0, sizeof(dis_init));

    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, (char *)MANUFACTURER_NAME);

    dis_init.dis_char_rd_sec = SEC_OPEN;

    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the sensor simulators.
 */
static void sensor_simulator_init(void)
{
    m_battery_sim_cfg.min          = MIN_BATTERY_LEVEL;
    m_battery_sim_cfg.max          = MAX_BATTERY_LEVEL;
    m_battery_sim_cfg.incr         = BATTERY_LEVEL_INCREMENT;
    m_battery_sim_cfg.start_at_max = true;

    sensorsim_init(&m_battery_sim_state, &m_battery_sim_cfg);

    m_rr_interval_sim_cfg.min          = MIN_RR_INTERVAL;
    m_rr_interval_sim_cfg.max          = MAX_RR_INTERVAL;
    m_rr_interval_sim_cfg.incr         = RR_INTERVAL_INCREMENT;
    m_rr_interval_sim_cfg.start_at_max = false;

    sensorsim_init(&m_rr_interval_sim_state, &m_rr_interval_sim_cfg);
}


/**@brief Function for starting application timers.
 */
static void application_timers_start(void)
{
    ret_code_t err_code;

    // Start application timers.
    err_code = app_timer_start(m_battery_timer_id, BATTERY_LEVEL_MEAS_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_heart_rate_timer_id, HEART_RATE_MEAS_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_rr_interval_timer_id, RR_INTERVAL_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_sensor_contact_timer_id, SENSOR_CONTACT_DETECTED_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_ppg_timer_id, PPG_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    ret_code_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    ret_code_t             err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = m_hrs.hrm_handles.cccd_handle;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    ret_code_t err_code;

    nrf_gpio_pin_set(20);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    ret_code_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            NRF_LOG_INFO("Fast advertising.");
            nrf_gpio_pin_clear(18);
            break;

        case BLE_ADV_EVT_IDLE:
            sleep_mode_enter();
            break;

        default:
            break;
    }
}


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ret_code_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected.");
            nrf_gpio_pin_set(18);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected, reason %d.",
                          p_ble_evt->evt.gap_evt.params.disconnected.reason);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .tx_phys = BLE_GAP_PHY_AUTO,
                .rx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;
    
        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            NRF_LOG_DEBUG("BLE_GAP_EVT_SEC_PARAMS_REQUEST");
            break;
        
        case BLE_GAP_EVT_AUTH_KEY_REQUEST:
            NRF_LOG_INFO("BLE_GAP_EVT_AUTH_KEY_REQUEST");
            break;

        case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
            NRF_LOG_INFO("BLE_GAP_EVT_LESC_DHKEY_REQUEST");
            break;

         case BLE_GAP_EVT_AUTH_STATUS:
             NRF_LOG_INFO("BLE_GAP_EVT_AUTH_STATUS: status=0x%x bond=0x%x lv4: %d kdist_own:0x%x kdist_peer:0x%x",
                          p_ble_evt->evt.gap_evt.params.auth_status.auth_status,
                          p_ble_evt->evt.gap_evt.params.auth_status.bonded,
                          p_ble_evt->evt.gap_evt.params.auth_status.sm1_levels.lv4,
                          *((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_own),
                          *((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_peer));
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}


/**@brief Function for the Peer Manager initialization.
 */
static void peer_manager_init(void)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    ret_code_t             err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      = true;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids  = m_adv_uuids;

    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout  = APP_ADV_DURATION;

    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}


static void gpio_init(){

    nrfx_err_t err_code;

    if(!nrfx_gpiote_is_init())
    {
        err_code = nrfx_gpiote_init();
        APP_ERROR_CHECK(err_code);
    }

    nrf_gpio_cfg_output(18);
    nrf_gpio_pin_set(18);

    nrf_gpio_cfg_output(13);
    nrf_gpio_pin_set(13);

    nrf_gpio_cfg_output(20);
    nrf_gpio_pin_set(20);
}


/**
 * @brief UART initialization.
 */
void twi_init (void)
{
    ret_code_t err_code;

    nrf_gpio_cfg_input(25, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(30, NRF_GPIO_PIN_PULLUP);


    const nrfx_twim_config_t twi_config = {
       .scl                = 25,
       .sda                = 30,
       .frequency          = NRF_TWIM_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH
    };

    err_code = nrfx_twim_init(&m_twi, &twi_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    nrfx_twim_enable(&m_twi);
}


void FPU_init(void)
{
  arm_status err_code = arm_rfft_fast_init_f32(&fft_instance, 1024);
  if(err_code != ARM_MATH_SUCCESS)
  {
    NRF_LOG_ERROR("Could not initialize fft");
    NRF_LOG_FLUSH();
  }
}




/***********************************************************************************************************/



static axis3bit16_t data_raw_acceleration;
static axis3bit16_t data_raw_angular_rate;
static axis1bit16_t data_raw_temperature;
static float acceleration_mg[3];
static float angular_rate_mdps[3];
static float temperature_degC;
static uint8_t whoamI, rst;
static lsm6dsl_ctx_t dev_ctx;
static lsm6dsl_int1_route_t step;
lsm6dsl_ctrl10_c_t enable_func;
lsm6dsl_ctrl10_c_t enable_ped;
static mems_status_t error = MEMS_ERROR;



static lsm6dsl_func_cfg_access_t access_me;

//static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(0);

/************************************************************SH*****************************************************/
static float _x_last_odr = 26.0f;
static uint8_t _x_is_enabled = 0;

int set_x_odr_when_enabled (float odr){ 

	lsm6dsl_odr_xl_t new_odr;	  
  
	new_odr = ( odr <=   13.0f ) ? LSM6DSL_XL_ODR_12Hz5
		  : ( odr <=   26.0f ) ? LSM6DSL_XL_ODR_26Hz
		  : ( odr <=   52.0f ) ? LSM6DSL_XL_ODR_52Hz
		  : ( odr <=  104.0f ) ? LSM6DSL_XL_ODR_104Hz
		  : ( odr <=  208.0f ) ? LSM6DSL_XL_ODR_208Hz
		  : ( odr <=  416.0f ) ? LSM6DSL_XL_ODR_416Hz
		  : ( odr <=  833.0f ) ? LSM6DSL_XL_ODR_833Hz
		  : ( odr <= 1660.0f ) ? LSM6DSL_XL_ODR_1k66Hz
		  : ( odr <= 3330.0f ) ? LSM6DSL_XL_ODR_3k33Hz
		  :                      LSM6DSL_XL_ODR_6k66Hz;
            
  if ( lsm6dsl_xl_data_rate_set(&dev_ctx, new_odr ) == error ) 
  {   
   return 1;
  }
  
  
  return 0;
}

int set_x_odr_when_disabled (float odr)
{ 
  _x_last_odr = ( odr <=   13.0f ) ? 13.0f
             : ( odr <=   26.0f ) ? 26.0f
             : ( odr <=   52.0f ) ? 52.0f
             : ( odr <=  104.0f ) ? 104.0f
             : ( odr <=  208.0f ) ? 208.0f
             : ( odr <=  416.0f ) ? 416.0f
             : ( odr <=  833.0f ) ? 833.0f
             : ( odr <= 1660.0f ) ? 1660.0f
             : ( odr <= 3330.0f ) ? 3330.0f
             :                      6660.0f;
                                 
  return 0;
}

int set_x_odr(float odr)
{

  if(_x_is_enabled == 1)
  {
    if(set_x_odr_when_enabled(odr) == 1) 
    {
      return 1;
    }
  }
  else
  {
    if(set_x_odr_when_disabled(odr) == 1) 
    {
      return 1;
    }
  }
  
  return 0;
}

int set_x_fs(float fullScale)
{

  lsm6dsl_fs_xl_t new_fs;
  
  new_fs = ( fullScale <= 2.0f ) ? LSM6DSL_2g
         : ( fullScale <= 4.0f ) ? LSM6DSL_4g
         : ( fullScale <= 8.0f ) ? LSM6DSL_8g
         :                         LSM6DSL_16g;
           
  if ( lsm6dsl_xl_full_scale_set( &dev_ctx, new_fs ) == error)
  {
    return 1;
  }
  
  return 0;
}





int enable_x(void) {

   if ( _x_is_enabled == 1 )
  {
    return 0;
  }
  
  /* Output data rate selection. */
  if ( set_x_odr_when_enabled( _x_last_odr ) == 1 )
  {
    return 1;
  }
  
  _x_is_enabled = 1;
  
  return 0;	
}

int set_pedometer_threshold(uint8_t ped_thresh)
{

  if (lsm6dsl_pedo_threshold_set( &dev_ctx, ped_thresh) == error)
  {
    return 1;
  }
  
  return 0;
}


int enable_pedometer(void)
{

  /* Output Data Rate selection */
  if( set_x_odr(26.0f) == error )
  {
    return 1;
  }
  
  /* Full scale selection. */
  if( set_x_fs(2.0f) == error )
  {
    return 1;
  }
  
  /* Set pedometer threshold. */
  if (set_pedometer_threshold(LSM6DSL_PEDOMETER_THRESHOLD_MID_LOW) == 1)
  {
    return 1;
  }


    if ( lsm6dsl_pedo_sens_set(&dev_ctx, 1) == error ) //uint8_t pedo_en == 1
  {
    return 1;
  }

  step.int1_step_detector = 1;
  if(lsm6dsl_pin_int1_route_set(&dev_ctx, &step) == error)
  {
	return 1;
  }
  
  return 0;
}



/***callback peter*** like int1_cb()*/

void int1_cb(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
      
      uint8_t stepcount = 0;
      lsm6dsl_pedo_sens_get(&dev_ctx, &stepcount);
      NRF_LOG_INFO("# steps, %d", stepcount);
      NRF_LOG_FLUSH();
}






/**@brief Function for application main entry.
 */
int main(void)
{
    bool erase_bonds;

    // Initialize.
    log_init();

    NRF_LOG_INFO("hello from nrf52832");
    NRF_LOG_FLUSH();

    gpio_init();
    twi_init();
    nrf_delay_ms(10);
    timers_init();

    dev_ctx.slv_adr = LSM6DSL_ID; 
    dev_ctx.i2c = &m_i2c;

    power_management_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    advertising_init();
    services_init();
    sensor_simulator_init();
    conn_params_init();
    peer_manager_init();

    FPU_init();
    ppg_init();

    //enable_x();
    //enable_pedometer();


    nrf_gpio_pin_clear(20);

    // Start execution.
    NRF_LOG_INFO("Heart Rate Sensor example started.");
    application_timers_start();
    advertising_start(true);


    nrf_gpio_pin_toggle(20);
    ppg_reinit();
    sample_counter = 0;
    bh_finished = false;
    bh_started = true;
    nrfx_timer_clear(&TIMER_PPG_SYNC);
    nrfx_timer_resume(&TIMER_PPG_SYNC);
    m_bh1792.start_measurement();

    while(true)
    {
        ret_code_t err_code;
        err_code = nrf_ble_lesc_request_handler();
        APP_ERROR_CHECK(err_code);

        if (NRF_LOG_PROCESS() == false)
        {
            nrf_pwr_mgmt_run();
        }
    }

}


