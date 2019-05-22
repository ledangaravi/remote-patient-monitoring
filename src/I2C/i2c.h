#ifndef I2C_h
#define I2C_h

#include "nrfx_twim.h"
#include "nrf_drv_gpiote.h"
#include "nrfx_gpiote.h"
#include "nrfx_twim.h"

#include <vector>

class I2C
{

public:

    I2C(nrfx_twim_t * _m_twi);
  
    int32_t i2c_write(uint8_t slv_adr, uint8_t reg_adr, uint8_t *reg, uint8_t reg_size);

    int32_t i2c_read(uint8_t slv_adr, uint8_t reg_adr, uint8_t *reg, uint8_t reg_size);

private:

    nrfx_twim_t *m_twi;
};

#endif I2C_h