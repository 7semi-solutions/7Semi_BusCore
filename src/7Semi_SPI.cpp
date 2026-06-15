#include "7Semi_SPI.h"

    bool SPI_7Semi::beginSPI(
        uint8_t csPin,
        SPIClass &spiPort,
        uint32_t spiSpeed,
        uint8_t spiMode) 

    {
        if (csPin == 255)
            return false;

        spi = &spiPort;
        cs = csPin;
        speed = spiSpeed;
        mode = spiMode;

        pinMode(cs, OUTPUT);
        digitalWrite(cs, HIGH);

        spi->begin();
        settings = SPISettings(speed, MSBFIRST, spiMode);

        return true;
    }

    bool SPI_7Semi::beginI2C(uint8_t, TwoWire &, uint32_t, uint8_t, uint8_t) 
    {
        return false;
    }

    int8_t SPI_7Semi:: write(const uint8_t *reg, uint8_t reg_len, const uint8_t *data, size_t len) 
    {
        if (!spi || !reg || !data || reg_len == 0 || len == 0)
            return -1;

        spi->beginTransaction(settings);

        digitalWrite(cs, LOW);

        for (uint8_t i = 0; i < reg_len; i++)
        {
            uint8_t regByte = reg[i];

            if (i == (reg_len - 1))
                regByte &= 0x7F;

            spi->transfer(regByte);
        }

        for (size_t i = 0; i < len; i++)
            spi->transfer(data[i]);

        digitalWrite(cs, HIGH);

        spi->endTransaction();

        return 0;
    }

    int8_t SPI_7Semi::read(
        const uint8_t *reg,
        uint8_t reg_len,
        uint8_t *data,
        size_t len) 
    {
        if (!spi || !reg || !data || reg_len == 0 || len == 0)
            return -1;

        spi->beginTransaction(settings);

        digitalWrite(cs, LOW);

        if (reg_len == 1)
        {
            spi->transfer(reg[0] | BUSIO_7SEMI_SPI_READ_MASK);
        }
        else
        {
            for (uint8_t i = 0; i < reg_len; i++)
            {
                spi->transfer(reg[i]);
            }
        }

        for (size_t i = 0; i < len; i++)
            data[i] = spi->transfer(0x00);

        digitalWrite(cs, HIGH);

        spi->endTransaction();

        return 0;
    }
