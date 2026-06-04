#pragma once
#include "7Semi_Interface.h"
#include <Wire.h>

#ifndef BUSIO_7SEMI_I2C_BUFFER_SIZE

#if defined(ESP32)

#define BUSIO_7SEMI_I2C_BUFFER_SIZE 128

#elif defined(ARDUINO_ARCH_RP2040)

#define BUSIO_7SEMI_I2C_BUFFER_SIZE 256

#else

#define BUSIO_7SEMI_I2C_BUFFER_SIZE 32

#endif

#endif

enum
{
    BUS_OK = 0,
    BUS_ERR_NOT_INIT = -1,
    BUS_ERR_TRANSFER = -2,
    BUS_ERR_SIZE = -3,
    BUS_ERR_AVAILABLE = -4,
    BUS_ERR_REG = -5,
    BUS_ERR_DATA = -6
};

class I2C_7Semi : public BusLink_7Semi
{
public:
    TwoWire *i2c = nullptr;
    int8_t status = 0;

    bool beginI2C(uint8_t addr, TwoWire &wire, uint32_t speed, uint8_t sda = 255, uint8_t scl = 255) override;

    bool setClockSpeed(uint32_t speed);

    bool setAddress(uint8_t i2cAddress);

    bool getAddress(uint8_t &i2cAddress);

    bool beginSPI(uint8_t, SPIClass &, uint32_t, uint8_t) override;

    int8_t read(const uint8_t *reg, uint8_t reg_len, uint8_t *data, size_t len) override;

    int8_t write(const uint8_t *reg, uint8_t reg_len, const uint8_t *data, size_t len) override;

    int8_t transfer(const uint8_t *tx, uint8_t tx_len, uint8_t *rx, size_t rx_len);

    private:

    uint8_t address = 0;
};