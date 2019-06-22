#include "max86141.h"
#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define WRITE_EN 0x00
#define READ_EN 0xFF

static uint8_t       m_tx_buf[3];           /**< TX buffer. */
static uint8_t       m_rx_buf[3];    /**< RX buffer. */
static const uint8_t m_length = sizeof(m_tx_buf);        /**< Transfer length. */

static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(0);  /**< SPI instance. */
static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */


/** from the Segger SPI example
 * @brief SPI user event handler.
 * @param event
 */
void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context)
{
    spi_xfer_done = true;
   // NRF_LOG_INFO("Transfer completed.");

    if (m_rx_buf[0] != 0)
    {
        //NRF_LOG_INFO(" Received:");
        //NRF_LOG_HEXDUMP_INFO(m_rx_buf, strlen((const char *)m_rx_buf));
        //NRF_LOG_FLUSH();
    }
}

/*write to register function*/
void max86141_write_reg(uint8_t address, uint8_t data_in)
{

/**< A buffer with data to transfer. */
    m_tx_buf[0] = address;  
    m_tx_buf[1] = WRITE_EN;
    m_tx_buf[2] = data_in;

   
/**< A buffer for incoming data. */    

    m_rx_buf[0] = 0;
    m_rx_buf[1] = 0;
    m_rx_buf[2] = 0;

    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3));

}

/*read register function*/
void max86141_read_reg(uint8_t address, uint8_t *data_out)
{


    /**< A buffer with data to transfer. */
    m_tx_buf[0] = address;  
    m_tx_buf[1] = READ_EN;
    m_tx_buf[2] = 0x00;

/**< A buffer for incoming data. */ 
    m_rx_buf[0] = 0;
    m_rx_buf[1] = 0;
    m_rx_buf[2] = 0;

    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3));

    *data_out = m_rx_buf[2];

}


/*inspired by pseudo-code available on MAX86141 datasheet for initialisation*/
void max86141_init()
{

    max86141_write_reg(REG_MODE_CONFIG, 0x01); //soft reset, bit turns to 0 after reset
    nrf_delay_ms(1);
    max86141_write_reg(REG_MODE_CONFIG, 0x02); //shutdown
    nrf_delay_ms(1);
    max86141_read_reg(REG_INTR_STATUS_1, &value); // clear interrupt 1
    nrf_delay_ms(1);
    max86141_read_reg(REG_INTR_STATUS_2, &value);  // clear interrupt 2
    nrf_delay_ms(1);
    max86141_write_reg(REG_PPG_CONFIG_1, 0x2B);//sample averaging = 1, rate 25sps
    nrf_delay_ms(1);
    max86141_write_reg(REG_PPG_CONFIG_3, 0xC0);  //led settling at 12usecs
    nrf_delay_ms(1);
    max86141_write_reg(REG_PDIODE_BIAS, 0x11); //65Pf PD CAP
    nrf_delay_ms(1);
    max86141_write_reg(REG_LED1_PA, 0x20); //LED1 drive current 15.36mA
    nrf_delay_ms(1);
    max86141_write_reg(REG_LED2_PA, 0x20);//LED2 drive current 15.36mA
    nrf_delay_ms(1);
    max86141_write_reg(REG_MODE_CONFIG, 0x04); //low power mode enabled
    nrf_delay_ms(1);
    max86141_write_reg(REG_LED_SEQ_1, 0x21); //exposure settings for led1 and 2
    nrf_delay_ms(1);
    max86141_write_reg(REG_LED_SEQ_2, 0x00); //led3 and 4 off
    nrf_delay_ms(1);
    max86141_write_reg(REG_LED_SEQ_3, 0x00); //led5 and 6 off
    nrf_delay_ms(1);
    max86141_write_reg(REG_MODE_CONFIG, 0x00); //start sampling

  /*
    NRF_LOG_INFO("Initialized");
    NRF_LOG_FLUSH();*/
}

/* inspired by pseudo-code available on MAX86141 datasheet */
void device_data_read(void)
{
      uint8_t sample_count;
      uint8_t reg_val;
      uint8_t dataBuf[128*2*2*3]; ///128 FIFO samples, 2 channels, 2 PDs, 3 bytes/channel

      int led1A[32];
      int led1B[32];
      int led2A[32];
      int led2B[32];

      max86141_read_reg(REG_FIFO_DATA_COUNT, &sample_count); //number of items available in FIFO to read 

      max86141_read_fifo(dataBuf, sample_count*3);

  
      /*suitable formatting of data for 2 LEDs*/
      int i = 0;
      for (i=0; i<sample_count; i++)
      {

       led1A[i] = ((dataBuf[i*12+0] << 16 ) | (dataBuf[i*12+1] << 8) | (dataBuf[i*12+2])) & 0x7FFFF; // LED1, PD1
       led1B[i] = ((dataBuf[i*12+3] << 16 ) | (dataBuf[i*12+4] << 8) | (dataBuf[i*12+5])) & 0x7FFFF; // LED1, PD2
       led2A[i] = ((dataBuf[i*12+6] << 16 ) | (dataBuf[i*12+7] << 8) | (dataBuf[i*12+8])) & 0x7FFFF; // LED2, PD1
       led2B[i] = ((dataBuf[i*12+9] << 16 ) | (dataBuf[i*12+10] << 8) | (dataBuf[i*12+11])) & 0x7FFFF; // LED2, PD2
 
      } 

}

void max86141_fifo_intr()
{
    uint8_t intStatus;
    max86141_read_reg(REG_INTR_STATUS_1, &intStatus); 

    if (intStatus& 0x80) //indicates full FIFO
    { 
    device_data_read();
    }
 }

/*SPI initialisation, inspired by Segger Studio example code*/
void spi_init()
{
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    nrf_drv_spi_frequency_t frequency;
    spi_config.ss_pin   = 5;
    spi_config.miso_pin = 29; //SDO
    spi_config.mosi_pin = 4; //SDI
    spi_config.sck_pin  = 28;
    spi_config.frequency = NRF_DRV_SPI_FREQ_125K; //frequency of transfer, MAX86141 supports upto 8MHz
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, NULL, NULL));
}

/*read FIFO*/
void max86141_readfifo(uint8_t data_buffer[], uint8_t count)
{
    uint8_t output;
    for(int i =0; i<count; i++){

        max86141_read_reg(REG_FIFO_DATA, &output);
        data_buffer[i] = &output;
    }
}
