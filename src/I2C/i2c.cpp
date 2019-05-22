
#include "i2c.h"

I2C::I2C(nrfx_twim_t * _m_twi) : m_twi(_m_twi) {}


int32_t I2C::i2c_write(uint8_t slv_adr, uint8_t reg_adr, uint8_t *reg, uint8_t reg_size)
{
    int32_t err_code;

    std::vector<uint8_t> i2c_cmd(reg, reg + reg_size);

    i2c_cmd.insert(i2c_cmd.begin(), reg_adr);

    err_code = nrfx_twim_tx(m_twi, slv_adr, &i2c_cmd[0], reg_size + 1U, false);
    APP_ERROR_CHECK(err_code);

    return 0;
}


int32_t I2C::i2c_read(uint8_t slv_adr, uint8_t reg_adr, uint8_t *reg, uint8_t reg_size)
{
    int32_t err_code;

    err_code = nrfx_twim_tx(m_twi, slv_adr, &reg_adr, 1U, false);
    APP_ERROR_CHECK(err_code);

    err_code = nrfx_twim_rx(m_twi, slv_adr, reg, reg_size);
    APP_ERROR_CHECK(err_code);

    return 0;
}
