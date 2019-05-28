#ifndef BH1792GLC_h
#define BH1792GLC_h

#include "BH1792GLC_registers.h"

#include "nrf_delay.h"
#include "nrf_drv_timer.h"

#include "i2c.h"

#include <stdint.h>

typedef struct {
    uint16_t       on;
    uint16_t       off;
} u16_pair_t;

typedef struct {
    uint32_t       on;
    uint32_t       off;
} u32_pair_t;


// BH1792 Data
typedef struct {
    u16_pair_t     ir;
    u16_pair_t     green;
    u16_pair_t     fifo[35];
    u16_pair_t     fifo_lpf[35];
    uint8_t        fifo_lev;
} bh1792_data_t;

// BH1792 Register Parameters
typedef struct {
    uint8_t        sel_adc;
    uint8_t        msr;
    uint8_t        led_en;
    uint8_t        led_cur1;
    uint8_t        led_cur2;
    uint16_t       ir_th;
    uint8_t        int_sel;
} bh1792_prm_t;

// BH1792 Moving Average Parameters
typedef struct {
    u16_pair_t     buf[8];
    u32_pair_t     sum;
    int8_t         pos;
    int8_t         len;
    int8_t         num;
} bh1792_maPrm_t;

// BH1792 Configuration

class BH1792GLC
{

    public:

        I2C *m_i2c;

        bh1792_prm_t prm;
        bh1792_maPrm_t ma_prm;
        int32_t i2c_err;
        int8_t is_measuring;
        int8_t sync_seq;
        int8_t p_int;
        bh1792_data_t m_dat;
        nrfx_gpiote_evt_handler_t interrupt_handler;

        BH1792GLC(I2C *_i2c);
        BH1792GLC();
        
        int32_t init();
        int32_t configure(void);
        int32_t start_measurement(void);
        int32_t stop_measurement(void);
        int32_t get_measurement_data();
        int32_t send_sync(void);
        int32_t clear_fifo(void);
        int32_t get_out_data();
        int32_t get_fifo_data();

    private:

        void in_pin_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

        int32_t _i2c_write(uint8_t slv_adr, uint8_t reg_adr, uint8_t *reg, uint8_t reg_size);
        int32_t _i2c_read(uint8_t slv_adr, uint8_t reg_adr, uint8_t *reg, uint8_t reg_size);

};

#endif // BH1792GLC_h