
#include "7Semi_I2C.h"

bool I2C_7Semi::beginI2C(uint8_t addr, TwoWire &wire, uint32_t speed, uint8_t sda, uint8_t scl)
{
    address = addr;
    i2c = &wire;

#if defined(ESP32)
    if (sda != 255 && scl != 255)
        i2c->begin(sda, scl);
    else
        i2c->begin();
#else
    i2c->begin();
#endif

    i2c->setClock(speed);

    i2c->beginTransmission(address);
    return (i2c->endTransmission() == 0);
}

bool I2C_7Semi::setClockSpeed(uint32_t speed)
{
    i2c->setClock(speed);
    return true;
}


bool I2C_7Semi::setAddress(uint8_t i2cAddress)
{
    address = i2cAddress;
    return true;
}

bool I2C_7Semi::getAddress(uint8_t &i2cAddress)
{
    i2cAddress = address;
    return true;
}

bool I2C_7Semi::beginSPI(uint8_t, SPIClass &, uint32_t, uint8_t)
{
    return false;
}

int8_t I2C_7Semi::read(const uint8_t *reg, uint8_t reg_len, uint8_t *data, size_t len)
{
    if (!i2c)
    {
        status = BUS_ERR_NOT_INIT;
        return BUS_ERR_NOT_INIT;
    }
    if (!reg || reg_len == 0)
    {
        status = BUS_ERR_REG;
        return BUS_ERR_REG;
    }

    if (!data || len == 0)
    {
        status = BUS_ERR_DATA;
        return BUS_ERR_DATA;
    }

    i2c->beginTransmission(address);

    i2c->write(reg, reg_len);

    if (i2c->endTransmission(false) != 0)
    {
        status = BUS_ERR_TRANSFER;
        return BUS_ERR_TRANSFER;
    }

    while (len > 0)
    {
        uint8_t request_data;

        if (len > BUSIO_7SEMI_I2C_BUFFER_SIZE)
            request_data = BUSIO_7SEMI_I2C_BUFFER_SIZE;
        else
            request_data = (uint8_t)len;

        size_t received = i2c->requestFrom(address, request_data);

        if (received != request_data)
        {
            status = BUS_ERR_SIZE;
            return BUS_ERR_SIZE;
        }

        for (uint8_t i = 0; i < request_data; i++)
        {
            if (!i2c->available())
            {
                status = BUS_ERR_AVAILABLE;
                return BUS_ERR_AVAILABLE;
            }

            data[i] = i2c->read();
        }

        data += request_data;
        len -= request_data;
    }
    status = 0;
    return 0;
}

int8_t I2C_7Semi::write(const uint8_t *reg, uint8_t reg_len, const uint8_t *data, size_t len)
{
    if (!i2c)
    {
        status = BUS_ERR_NOT_INIT;
        return BUS_ERR_NOT_INIT;
    }

    if (!reg || reg_len == 0)
    {
        status = BUS_ERR_REG;
        return BUS_ERR_REG;
    }

    if (!data || len == 0)
    {
        status = BUS_ERR_DATA;
        return BUS_ERR_DATA;
    }

    if ((reg_len + len) > BUSIO_7SEMI_I2C_BUFFER_SIZE)
    {
        status = BUS_ERR_TRANSFER;
        return BUS_ERR_TRANSFER;
    }

    i2c->beginTransmission(address);

    i2c->write(reg, reg_len);
    i2c->write(data, len);

    status = i2c->endTransmission();

    return (status == 0) ? BUS_OK : status;
}

int8_t I2C_7Semi::transfer(
    const uint8_t *tx,
    uint8_t tx_len,
    uint8_t *rx,
    size_t rx_len)
{
    if (!i2c)
    {
        status = BUS_ERR_NOT_INIT;
        return BUS_ERR_NOT_INIT;
    }
    if (!tx || tx_len == 0)
    {
        status = BUS_ERR_REG;
        return BUS_ERR_REG;
    }

    if (!rx || rx_len == 0)
    {
        status = BUS_ERR_DATA;
        return BUS_ERR_DATA;
    }

    i2c->beginTransmission(address);

    i2c->write(tx, tx_len);

    if (i2c->endTransmission(false) != 0)
    {
        status = BUS_ERR_TRANSFER;
        return BUS_ERR_TRANSFER;
    }

    if (rx_len > 0)
    {
        while (rx_len > 0)
        {
            uint8_t request_data =
                (rx_len > BUSIO_7SEMI_I2C_BUFFER_SIZE)
                    ? BUSIO_7SEMI_I2C_BUFFER_SIZE
                    : (uint8_t)rx_len;

            size_t received =
                i2c->requestFrom(
                    address,
                    request_data);

            if (received != request_data)
            {
                status = BUS_ERR_SIZE;
                return BUS_ERR_SIZE;
            }

            for (uint8_t i = 0; i < request_data; i++)
            {
                if (!i2c->available())
                {
                    status = BUS_ERR_AVAILABLE;
                    return BUS_ERR_AVAILABLE;
                }

                rx[i] = i2c->read();
            }

            rx += request_data;
            rx_len -= request_data;
        }
    }
    status = 0;
    return 0;
}