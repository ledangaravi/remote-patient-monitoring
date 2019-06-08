/**
 * Copyright (c) 2014 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @example examples/ble_peripheral/ble_app_hrs/main.c
 *
 * @brief Heart Rate Service Sample Application main file.
 *
 * This file contains the source code for a sample application using the Heart Rate service
 * (and also Battery and Device Information services). This application uses the
 * @ref srvlib_conn_params module.
 */

#include <stdint.h>
#include <string>
#include <vector>
#include <iterator>

#include "nordic_common.h"
#include "nrf.h"
#include "nrf_sdm.h"
#include "app_error.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "app_timer.h"
#include "nrf_atfifo.h"

#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "nrfx_gpiote.h"
#include "nrfx_twim.h"
#include "nrf_delay.h"
#include "nrf_drv_timer.h"
//#include "arm_const_structs.h"


extern "C" {
  #include "nrf_pwr_mgmt.h"
}

#include "BH1792GLC.h"
#include "BH1792GLC_registers.h"

#include "i2c.h"
#include "lsm6ds3_reg.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


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


/**************************************************************************************************/


#define BH1792_INT_PIN     11

/* TWI instance ID. */
#define TWI_INSTANCE_ID     0


/* TWI instance. */
static const nrfx_twim_t m_twi = NRFX_TWIM_INSTANCE(TWI_INSTANCE_ID);

static const I2C m_i2c = I2C(&m_twi);

static const nrfx_timer_t TIMER_SYNC = NRF_DRV_TIMER_INSTANCE(0);

static uint8_t ctr = 0;

uint32_t previous_tick = 0;
uint32_t current_tick = 0;
static uint16_t step_count = 0;

//NRF_ATFIFO_DEF(sample_fifo, uint16_t, 1116U);

static uint16_t samples[3072];
static uint16_t sample_counter = 0;

BH1792GLC m_bh1792;


//IMU static variables
static axis3bit16_t data_raw_acceleration;
static float acceleration_mg[3];
static uint8_t whoamI, rst;
static lsm6ds3_ctx_t dev_ctx;



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
              if(sample_counter >= 2945)
              {
                nrfx_gpiote_out_toggle(13);
              }
              else
              {
                memcpy(&samples[sample_counter], &m_bh1792.m_dat.fifo, fifo_l*2);
                /*for (int i = 0; i < fifo_l; i++) {
                  uint16_t dat = (m_bh1792.m_dat.fifo[i].on - m_bh1792.m_dat.fifo[i].off);
                  //uint16_t dat = m_bh1792.m_dat.fifo[i].on;
                  //uint8_t ret = nrf_atfifo_alloc_put(sample_fifo,&dat,2U,NULL);
                  samples[sample_counter + i] = dat;
                }*/
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
     nrfx_gpiote_out_toggle(18);
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

    const nrfx_twim_config_t twi_bh1792_config = {
       .scl                = 25,
       .sda                = 30,
       .frequency          = NRF_TWIM_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH
    };

    err_code = nrfx_twim_init(&m_twi, &twi_bh1792_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    nrfx_twim_enable(&m_twi);
}

/**@brief Function for application main entry.
 */
int main(void)
{
    bool erase_bonds;

    // Initialize.
    log_init();
    /*timers_init();
    buttons_leds_init(&erase_bonds);
    power_management_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    advertising_init();
    services_init();
    sensor_simulator_init();
    conn_params_init();
    peer_manager_init();

    // Start execution.
    NRF_LOG_INFO("Heart Rate Sensor example started.");

    application_timers_start();
    advertising_start(erase_bonds);

    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
    }*/

    NRF_LOG_INFO("hello from nrf52dk");
    NRF_LOG_FLUSH();

    twi_init();
    gpio_init();


    uint16_t pattern_len, pattern_numbers;

    dev_ctx.i2c = &m_i2c;
    dev_ctx.slv_adr = LSM6DS3_I2C_ADD_L;

    lsm6ds3_device_id_get(&dev_ctx, &whoamI);
    if (whoamI != LSM6DS3_ID)
    {
      NRF_LOG_INFO("Couldn't connect to IMU");
      NRF_LOG_FLUSH();
    }

    lsm6ds3_reset_set(&dev_ctx, PROPERTY_ENABLE);
    do {
            lsm6ds3_reset_get(&dev_ctx, &rst);
    } while (rst);

    lsm6ds3_xl_full_scale_set(&dev_ctx, LSM6DS3_2g);

    lsm6ds3_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);

    pattern_len = 3;
    pattern_numbers = 1248;
    lsm6ds3_fifo_watermark_set(&dev_ctx, pattern_numbers * pattern_len);
    lsm6ds3_fifo_stop_on_wtm_set(&dev_ctx, PROPERTY_ENABLE);

    lsm6ds3_fifo_mode_set(&dev_ctx, LSM6DS3_STREAM_TO_FIFO_MODE);

    lsm6ds3_fifo_xl_batch_set(&dev_ctx, LSM6DS3_FIFO_XL_NO_DEC);

    lsm6ds3_fifo_data_rate_set(&dev_ctx, LSM6DS3_FIFO_52Hz);
  
    lsm6ds3_xl_data_rate_set(&dev_ctx, LSM6DS3_XL_ODR_52Hz);



    m_bh1792 = BH1792GLC(&m_i2c);

    m_bh1792.init();


    ret_code_t err_code;


    m_bh1792.prm.sel_adc  = BH1792_PRM_SEL_ADC_GREEN;
    m_bh1792.prm.msr      = BH1792_PRM_MSR_128HZ;
    m_bh1792.prm.led_en   = (BH1792_PRM_LED_EN1_0 << 1);// | BH1792_PRM_LED_EN2_0;
    m_bh1792.prm.led_cur1 = 20;
    m_bh1792.prm.led_cur2 = 0;
    m_bh1792.prm.ir_th    = 0;
    m_bh1792.prm.int_sel  = BH1792_PRM_INT_SEL_SGL;
    m_bh1792.interrupt_handler = &in_pin_handler;
    m_bh1792.p_int        = BH1792_INT_PIN;
    m_bh1792.configure();

    m_bh1792.clear_fifo();

    //NRF_ATFIFO_INIT(sample_fifo);

    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_cfg.interrupt_priority = 1;
    err_code = nrfx_timer_init(&TIMER_SYNC, &timer_cfg, timer_sync_event_handler);
    APP_ERROR_CHECK(err_code);

    uint32_t time_ms = 200U;
    uint32_t time_ticks;

    time_ticks = nrfx_timer_ms_to_ticks(&TIMER_SYNC, time_ms);

    nrfx_timer_extended_compare(
         &TIMER_SYNC, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

    nrfx_timer_enable(&TIMER_SYNC);

    m_bh1792.start_measurement();

    nrf_gpio_pin_clear(20);

    while (true)
    {
        
        __WFI();
        
        //NRF_LOG_FLUSH();
        
        //nrf_delay_ms(5);
        //m_bh1792.get_measurement_data();
        
        //NRF_LOG_INFO("%d, %d", m_bh1792.m_dat.green.on, m_bh1792.m_dat.green.off);
        //NRF_LOG_INFO("%d, %d", m_bh1792.m_dat.ir.on, m_bh1792.m_dat.ir.off);
        //NRF_LOG_FLUSH();
        //nrf_delay_ms(100);
        //NRF_LOG_FLUSH();
    
        /*lsm6dsl_reg_t reg;
        lsm6dsl_status_reg_get(&dev_ctx, &reg.status_reg);

        if (reg.status_reg.xlda)
        {
          //Read magnetic field data
          memset(data_raw_acceleration.u8bit, 0x00, 3*sizeof(int16_t));
          lsm6dsl_acceleration_raw_get(&dev_ctx, data_raw_acceleration.u8bit);
          acceleration_mg[0] = lsm6dsl_from_fs2g_to_mg( data_raw_acceleration.i16bit[0]);
          acceleration_mg[1] = lsm6dsl_from_fs2g_to_mg( data_raw_acceleration.i16bit[1]);
          acceleration_mg[2] = lsm6dsl_from_fs2g_to_mg( data_raw_acceleration.i16bit[2]);
      
          NRF_LOG_INFO("Acceleration [mg]:%4.2f\t%4.2f\t%4.2f\r\n",
                  acceleration_mg[0], acceleration_mg[1], acceleration_mg[2]);
          NRF_LOG_FLUSH();
        } 
        if (reg.status_reg.gda)
        {
          //Read magnetic field data
          memset(data_raw_angular_rate.u8bit, 0x00, 3*sizeof(int16_t));
          lsm6dsl_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate.u8bit);
          angular_rate_mdps[0] = lsm6dsl_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[0]);
          angular_rate_mdps[1] = lsm6dsl_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[1]);
          angular_rate_mdps[2] = lsm6dsl_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[2]);
      
          NRF_LOG_INFO("Angular rate [mdps]:%4.2f\t%4.2f\t%4.2f\r\n",
                  angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);
          NRF_LOG_FLUSH();
        }    
        if (reg.status_reg.tda)
        {   
          //Read temperature data
          memset(data_raw_temperature.u8bit, 0x00, sizeof(int16_t));
          lsm6dsl_temperature_raw_get(&dev_ctx, data_raw_temperature.u8bit);
          temperature_degC = lsm6dsl_from_lsb_to_celsius( data_raw_temperature.i16bit );
       
          NRF_LOG_INFO("Temperature [degC]:%6.2f\r\n", temperature_degC );
          NRF_LOG_FLUSH();
        }*/


        /*__set_FPSCR(__get_FPSCR() & ~(FPU_EXCEPTION_MASK));
        (void) __get_FPSCR();
        NVIC_ClearPendingIRQ(FPU_IRQn);*/
    }


}


