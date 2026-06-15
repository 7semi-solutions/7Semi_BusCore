#pragma once
// #define SENSOR_DEBUG_7SEMI 1

#ifndef SENSOR_DEBUG_7SEMI
#define SENSOR_DEBUG_7SEMI 0
#endif

#if SENSOR_DEBUG_7SEMI
#define DEBUG_7SEMI_CORE
#endif

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

class BusLink_7Semi
{
public:
    virtual ~BusLink_7Semi() {}

    virtual bool beginI2C(uint8_t address, TwoWire &wire, uint32_t speed, uint8_t sda = 255, uint8_t scl = 255) = 0;

    virtual bool beginSPI(uint8_t cs_pin, SPIClass &spi, uint32_t speed, uint8_t spiMode = SPI_MODE0) = 0;

    virtual int8_t read(const uint8_t *reg, uint8_t reg_len, uint8_t *data, size_t len) = 0;

    virtual int8_t write(const uint8_t *reg, uint8_t reg_len, const uint8_t *data, size_t len) = 0;

    // virtual int8_t transfer(const uint8_t *tx, uint8_t tx_len, uint8_t *rx, size_t rx_len) = 0;
protected:
    static void delay_us(uint32_t us)
    {
        delayMicroseconds(us);
    }
};