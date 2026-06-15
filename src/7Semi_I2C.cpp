
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
    bool found = (i2c->endTransmission() == 0);

    status = found ? BUS_OK : BUS_ERR_TRANSFER;

    return found;
}

bool I2C_7Semi::setClockSpeed(uint32_t speed)
{
    if (!i2c)
        return false;

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
        uint8_t i;
        for (i = 0; i < request_data; i++)
        {
            if (!i2c->available())
            {
                status = BUS_ERR_AVAILABLE;
                return BUS_ERR_AVAILABLE;
            }

            data[i] = i2c->read();
        }

#if defined(DEBUG_7SEMI_CORE)

        Serial.print("[I2C RX] Addr:0x");

        if (address < 0x10)
            Serial.print('0');

        Serial.print(address, HEX);

        Serial.print(" Reg:");

        for (i = 0; i < reg_len; i++)
        {
            Serial.print(" 0x");
            if (reg[i] < 0x10)
                Serial.print('0');

            Serial.print(reg[i], HEX);
        }

        Serial.print(" | Data:");

        for (i = 0; i < request_data; i++)
        {
            Serial.print(" 0x");
            if (data[i] < 0x10)
                Serial.print('0');

            Serial.print(data[i], HEX);
        }

        Serial.println();

#endif

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

#if defined(DEBUG_7SEMI_CORE)

    Serial.print("[I2C TX] Addr:0x");
    Serial.print(address, HEX);

    Serial.print(" Reg:");

    for (uint8_t i = 0; i < reg_len; i++)
    {
        Serial.print(" ");
        if (reg[i] < 0x10)
            Serial.print("0");
        Serial.print(reg[i], HEX);
    }

    Serial.print(" Data:");

    for (uint32_t i = 0; i < len; i++)
    {
        Serial.print(" ");
        if (data[i] < 0x10)
            Serial.print("0");
        Serial.print(data[i], HEX);
    }

    Serial.println();

#endif

    return (status == 0) ? BUS_OK : -status;
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
    if ((tx == nullptr) && (tx_len > 0))
    {
        status = BUS_ERR_DATA;
        return BUS_ERR_DATA;
    }

    if ((rx == nullptr) && (rx_len > 0))
    {
        status = BUS_ERR_DATA;
        return BUS_ERR_DATA;
    }

    if ((tx_len == 0) && (rx_len == 0))
    {
        status = BUS_ERR_DATA;
        return BUS_ERR_DATA;
    }

    i2c->beginTransmission(address);

    i2c->write(tx, tx_len);

    if (i2c->endTransmission(rx_len > 0 ? false : true) != 0)
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
            uint8_t i;
            for (i = 0; i < request_data; i++)
            {
                if (!i2c->available())
                {
                    status = BUS_ERR_AVAILABLE;
                    return BUS_ERR_AVAILABLE;
                }

                rx[i] = i2c->read();
            }

#if defined(DEBUG_7SEMI_CORE)

            Serial.print("[I2C TF] Addr:0x");

            if (address < 0x10)
                Serial.print('0');

            Serial.print(address, HEX);

            Serial.print(" TX:");

            for (i = 0; i < tx_len; i++)
            {
                Serial.print(" 0x");
                if (tx[i] < 0x10)
                    Serial.print("0");
                if (tx[i] < 0x10)
                    Serial.print('0');

                Serial.print(tx[i], HEX);
            }

            Serial.print(" | RX:");

            for (i = 0; i < request_data; i++)
            {
                Serial.print(" 0x");
                if (rx[i] < 0x10)
                    Serial.print("0");
                if (rx[i] < 0x10)
                    Serial.print('0');

                Serial.print(rx[i], HEX);
            }

            Serial.println();

#endif

            rx += request_data;
            rx_len -= request_data;
        }
    }

    status = 0;
    return 0;
}