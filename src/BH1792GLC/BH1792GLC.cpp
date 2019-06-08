#include "BH1792GLC.h"

#include <vector>
#include <iterator>


#include "nrf_drv_gpiote.h"
#include "nrfx_gpiote.h"


BH1792GLC::BH1792GLC(I2C * _i2c) : m_i2c(_i2c) {}

BH1792GLC::BH1792GLC(){}



int32_t BH1792GLC::init()
{
    int32_t ret     = BH1792_SUCCESS;
    int32_t err_code;
    int32_t ret_i2c = 0;
    uint8_t reg[1];


    // Set inital regiser parameters
    this->prm.sel_adc  = BH1792_PRM_SEL_ADC_GREEN;
    this->prm.msr      = BH1792_PRM_MSR_32HZ;
    this->prm.led_en   = BH1792_PRM_LED_EN1_0;
    this->prm.led_cur1 = BH1792_PRM_LED_CUR1_MIN;
    this->prm.led_cur2 = BH1792_PRM_LED_CUR2_MIN;
    this->prm.ir_th    = 0xFFFC;
    this->prm.int_sel  = BH1792_PRM_INT_SEL_DISABLE;

    // Clear flags
    this->i2c_err      = 0;
    this->is_measuring = 0;
    this->sync_seq     = 0;

    ret_i2c = this->m_i2c->i2c_read(BH1792_SLAVE_ADDR, BH1792_ADDR_PARTID, reg, 1U);
    if(ret_i2c == 0) {
        if(reg[0] != BH1792_PRM_PARTID) {
            ret = BH1792_NOT_EXIST;
        }
        else {
            reg[0] = BH1792_PRM_SWRESET << 7;
            ret_i2c = this->m_i2c->i2c_write(BH1792_SLAVE_ADDR, BH1792_ADDR_RESET, reg, 1U);
        }
    }

    this->p_int;

    if(ret_i2c != 0) {
        this->i2c_err = ret_i2c;
    }

    return ret;
}


int32_t BH1792GLC::configure(void)
{
    int32_t ret      = BH1792_SUCCESS;
    int32_t ret_i2c  = 0;
    uint8_t reg_size = 6U;
    uint8_t reg[7];

    // MEAS_CTRLx register
    reg[0] = (BH1792_PRM_RDY << 7) | (BH1792_PRM_SEL_LED << 6) | (this->prm.sel_adc << 4) | this->prm.msr;
    reg[1] = ((this->prm.led_en >> 1) << 6) | this->prm.led_cur1;
    reg[2] = ((this->prm.led_en & 0x01U) << 7) | this->prm.led_cur2;
    reg[3] = (uint8_t)this->prm.ir_th;
    reg[4] = (uint8_t)(this->prm.ir_th >> 8);
    reg[5] = this->prm.int_sel;
    // MEAS_START register
    if(this->is_measuring > 0) {
        reg[6] = BH1792_PRM_MEAS_ST;
        reg_size = 7U;
    }
 
    ret_i2c = this->m_i2c->i2c_write(BH1792_SLAVE_ADDR, BH1792_ADDR_MEAS_CTRL1, reg, reg_size);
    if(ret_i2c != 0) {
        this->i2c_err = ret_i2c;
    }

    if(this->interrupt_handler != NULL  && this->p_int != -1)
    {
      uint32_t err_code;
      nrfx_gpiote_in_config_t in_config_hitolo = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(false);
      in_config_hitolo.pull = NRF_GPIO_PIN_PULLUP;
      err_code = nrfx_gpiote_in_init(this->p_int, &in_config_hitolo, this->interrupt_handler);
      APP_ERROR_CHECK(err_code);

      nrfx_gpiote_in_event_enable(this->p_int, true);
    }

    return ret;
}


int32_t BH1792GLC::start_measurement(void)
{
    int32_t ret      = BH1792_SUCCESS;
    int32_t ret_i2c  = 0;
    uint8_t reg[7];

    if(this->prm.msr < BH1792_PRM_MSR_SINGLE) {
        this->is_measuring   = 1;

        // Initialize moving average parameters
        this->ma_prm.sum.off = 0U;
        this->ma_prm.sum.on  = 0U;
        this->ma_prm.pos     = 0;
        this->ma_prm.num     = 0;

        switch(this->prm.msr) {
            case BH1792_PRM_MSR_32HZ:
            case BH1792_PRM_MSR_128HZ:
            case BH1792_PRM_MSR_64HZ:
                this->ma_prm.len = 2;
                break;
            case BH1792_PRM_MSR_256HZ:
                this->ma_prm.len = 4;
                break;
            case BH1792_PRM_MSR_1024HZ:
                this->ma_prm.len = 8;
                break;
            default:
                this->ma_prm.len = 0;
                break;
        }
    }

    reg[0] = BH1792_PRM_MEAS_ST;
    ret_i2c = this->m_i2c->i2c_write(BH1792_SLAVE_ADDR, BH1792_ADDR_MEAS_START, reg, 1U);
    if(ret_i2c != 0) {
        this->i2c_err = ret_i2c;
    }

    return ret;
}


int32_t BH1792GLC::stop_measurement(void)
{
    int32_t ret     = BH1792_SUCCESS;
    int32_t ret_i2c = 0;
    uint8_t reg[1];

    this->is_measuring = 0;
    this->sync_seq     = 0;

    reg[0] = BH1792_PRM_SWRESET << 7;
    ret_i2c = this->m_i2c->i2c_write(BH1792_SLAVE_ADDR, BH1792_ADDR_RESET, reg, 1U);
    if(ret_i2c != 0) {
        this->i2c_err = ret_i2c;
    }

    return ret;
}

int32_t BH1792GLC::get_measurement_data()
{
    int32_t ret        = BH1792_SUCCESS;
    int32_t ret_i2c    = 0;
    uint8_t int_clear  = 0U;
    uint8_t fifo_level = 0U;

    if(this->prm.msr <= BH1792_PRM_MSR_1024HZ) {
        ret_i2c = this->m_i2c->i2c_read(BH1792_SLAVE_ADDR, BH1792_ADDR_FIFO_LEV, &m_dat.fifo_lev, 1U);
         if (ret_i2c == 0)  {
            if(m_dat.fifo_lev == BH1792_PRM_FIFO_LEV_FULL) {
                ret = BH1792_FIFO_FULL;
            }

            if(this->prm.int_sel == BH1792_PRM_INT_SEL_WTM) {
                m_dat.fifo_lev = BH1792_PRM_FIFO_LEV_WTM;
            }

            ret_i2c = this->get_fifo_data();
            if(ret_i2c == 0) {
                ret_i2c = this->m_i2c->i2c_read(BH1792_SLAVE_ADDR, BH1792_ADDR_FIFO_LEV, &fifo_level, 1U);
            }
        }
    }
    else {
        m_dat.fifo_lev = 0U;
        ret_i2c = this->get_out_data();

        if(ret_i2c == 0) {
            if(this->prm.int_sel >= BH1792_PRM_INT_SEL_IR) {
                ret_i2c = this->m_i2c->i2c_read(BH1792_SLAVE_ADDR, BH1792_ADDR_INT_CLEAR, &int_clear, 1U);
            }
        }
    }

    if(ret_i2c != 0) {
        this->i2c_err = ret_i2c;
    }

    return ret;
}


int32_t BH1792GLC::send_sync(void)
{
    int32_t ret     = BH1792_SUCCESS;
    int32_t ret_i2c = 0;
    uint8_t reg[1];

    reg[0] = BH1792_PRM_MEAS_SYNC;
    ret_i2c = this->m_i2c->i2c_write(BH1792_SLAVE_ADDR, BH1792_ADDR_MEAS_SYNC, reg, 1U);
    if(ret_i2c != 0) {
        this->i2c_err = ret_i2c;
    }
    else {
        if(this->sync_seq < 3) {
            this->sync_seq++;
        }
    }

    return ret;
}


int32_t BH1792GLC::clear_fifo(void)
{
    int32_t ret        = BH1792_SUCCESS;
    int32_t ret_i2c    = 0;
    uint8_t i          = 0U;
    uint8_t r_cnt      = 35U;
    uint8_t fifo_level = 0U;
    uint8_t reg[4];

    if(this->prm.msr == BH1792_PRM_MSR_32HZ) {
        r_cnt = 32U;
    }

    for(i = 0; i < r_cnt; i++) {
        ret_i2c = this->m_i2c->i2c_read(BH1792_SLAVE_ADDR, BH1792_ADDR_FIFO_DATA0_LSBS, reg, 4U);
        if(ret_i2c != 0) {
            break;
        }
    }

    if(ret_i2c == 0) {
        ret_i2c = this->m_i2c->i2c_read(BH1792_SLAVE_ADDR, BH1792_ADDR_FIFO_LEV, &fifo_level, 1U);
    }

    if(ret_i2c != 0) {
        this->i2c_err = ret_i2c;
    }

    return ret;
}



int32_t BH1792GLC::get_out_data()
{
    int32_t ret_i2c = 0;
    uint8_t idx     = 0U;
    uint8_t reg[8]  = {0};

    if(this->prm.sel_adc == BH1792_PRM_SEL_ADC_GREEN) {
        idx = 4U;
    }

    ret_i2c = this->m_i2c->i2c_read(BH1792_SLAVE_ADDR, (BH1792_ADDR_IRDATA_LEDOFF_LSBS + idx), &reg[idx], 4U);
    if(ret_i2c == 0) {
        m_dat.ir.off    = ((uint16_t)reg[1] << 8) | (uint16_t)reg[0];
        m_dat.ir.on     = ((uint16_t)reg[3] << 8) | (uint16_t)reg[2];
        m_dat.green.off = ((uint16_t)reg[5] << 8) | (uint16_t)reg[4];
        m_dat.green.on  = ((uint16_t)reg[7] << 8) | (uint16_t)reg[6];
    }

    return ret_i2c;
}


int32_t BH1792GLC::get_fifo_data()
{
    int32_t ret_i2c = 0;
    uint8_t i       = 0U;
    uint8_t reg[4];
    bh1792_maPrm_t *ma_prm;

    ma_prm = &(this->ma_prm);

    

    for(i = 0U; i < m_dat.fifo_lev; i++) {
        
      ret_i2c = this->m_i2c->i2c_read(BH1792_SLAVE_ADDR, BH1792_ADDR_FIFO_DATA0_LSBS, reg, 4U);
        if(ret_i2c != 0) {
            break;
        }

        m_dat.fifo[i] = ((uint16_t)reg[3] << 8) | (uint16_t)reg[2] - ((uint16_t)reg[1] << 8) | (uint16_t)reg[0];

        
        /*if(ma_prm->num == ma_prm->len) {
            ma_prm->sum.off -= ma_prm->buf[ma_prm->pos].off;
            ma_prm->sum.on  -= ma_prm->buf[ma_prm->pos].on;
        }
        ma_prm->sum.off += m_dat.fifo[i].off;
        ma_prm->sum.on  += m_dat.fifo[i].on;
        ma_prm->buf[ma_prm->pos].off = m_dat.fifo[i].off;
        ma_prm->buf[ma_prm->pos].on  = m_dat.fifo[i].on;

        ma_prm->pos++;
        if(ma_prm->pos >= ma_prm->len) {
            ma_prm->pos = 0;
        }
        if(ma_prm->num < ma_prm->len) {
            ma_prm->num++;
        }

        if(ma_prm->num > 0) {
            m_dat.fifo_lpf[i].off = ma_prm->sum.off / ma_prm->num;
            m_dat.fifo_lpf[i].on  = ma_prm->sum.on  / ma_prm->num;
        }*/
    }

    return ret_i2c;
}