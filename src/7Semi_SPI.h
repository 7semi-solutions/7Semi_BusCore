#pragma once
#include "7Semi_Interface.h"

#ifndef BUSIO_7SEMI_SPI_READ_MASK
#define BUSIO_7SEMI_SPI_READ_MASK 0x80
#endif
#ifndef BUSIO_7SEMI_SPI_WRITE_MASK
#define BUSIO_7SEMI_SPI_WRITE_MASK 0x7F
#endif

class SPI_7Semi : public BusLink_7Semi
{
public:
    SPIClass *spi = nullptr;
    uint8_t cs = 255;
    uint32_t speed = 1000000;
    SPISettings settings = SPISettings(1000000, MSBFIRST, SPI_MODE0);
    uint8_t mode = SPI_MODE0;

    bool beginSPI(uint8_t csPin, SPIClass &spiPort, uint32_t spiSpeed, uint8_t spiMode) override;

    bool beginI2C(uint8_t, TwoWire &, uint32_t, uint8_t, uint8_t) override;

    int8_t write(const uint8_t *reg, uint8_t reg_len, const uint8_t *data, size_t len) override;
    
    int8_t read(const uint8_t *reg, uint8_t reg_len, uint8_t *data, size_t len) override;

    // virtual int8_t transfer(const uint8_t *tx, uint8_t tx_len, uint8_t *rx, size_t rx_len) = 0;
};