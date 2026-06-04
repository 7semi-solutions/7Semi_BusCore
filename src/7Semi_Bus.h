#pragma once
#include <Arduino.h>

template <bool Condition>
struct TypeSelector;

template <>
struct TypeSelector<true>
{
    typedef uint8_t type;
};

template <>
struct TypeSelector<false>
{
    typedef uint16_t type;
};

template <typename Bus, uint8_t RegSize, uint8_t DataSize, bool LittleEndian = false>
class BusIO_7Semi
{
public:
    typedef typename TypeSelector<(RegSize == 1)>::type reg_t;

    typedef typename TypeSelector<(DataSize == 1)>::type data_t;

    BusIO_7Semi(Bus &busRef) : bus(busRef) {}

    inline bool read(reg_t reg, uint8_t *data, size_t len)
    {
        uint8_t regBuf[RegSize];

        if (RegSize == 1)
        {
            regBuf[0] = (uint8_t)reg;
        }
        else
        {
            regBuf[0] = (uint8_t)(reg >> 8);
            regBuf[1] = (uint8_t)(reg & 0xFF);
        }

        return (bus.read(regBuf, RegSize, data, len) == 0);
    }

    inline bool write(reg_t reg, const uint8_t *data, size_t len)
    {
        uint8_t regBuf[RegSize];

        if (RegSize == 1)
        {
            regBuf[0] = (uint8_t)reg;
        }
        else
        {
            regBuf[0] = (uint8_t)(reg >> 8);
            regBuf[1] = (uint8_t)(reg & 0xFF);
        }

        return (bus.write(regBuf, RegSize, data, len) == 0);
    }

    inline bool read(reg_t reg, data_t &value)
    {
        uint8_t regBuf[RegSize];

        if (RegSize == 1)
        {
            regBuf[0] = reg;
        }
        else
        {
            regBuf[0] = reg >> 8;
            regBuf[1] = reg & 0xFF;
        }

        uint8_t data[DataSize];

        if (bus.read(regBuf, RegSize, data, DataSize) != 0)
            return false;

        if (DataSize == 1)
        {
            value = data[0];
        }
        else
        {
            if (LittleEndian)
            {
                value = ((uint16_t)data[1] << 8) | data[0];
            }
            else
            {
                value = ((uint16_t)data[0] << 8) | data[1];
            }
        }

        return true;
    }

    inline bool write(reg_t reg, data_t value)
    {
        uint8_t regBuf[RegSize];

        if (RegSize == 1)
        {
            regBuf[0] = reg;
        }
        else
        {
            regBuf[0] = reg >> 8;
            regBuf[1] = reg & 0xFF;
        }

        uint8_t data[DataSize];

        if (DataSize == 1)
        {
            data[0] = value;
        }
        else
        {
            if (LittleEndian)
            {
                data[0] = value & 0xFF;
                data[1] = value >> 8;
            }
            else
            {
                data[0] = value >> 8;
                data[1] = value & 0xFF;
            }
        }

        return bus.write(regBuf, RegSize, data, DataSize) == 0;
    }

    bool readBits(reg_t reg, uint8_t pos, uint8_t len, data_t &value)
    {
        const uint8_t data_bits = sizeof(data_t) * 8;

        if (len == 0)
            return false;

        if (len > data_bits)
            return false;

        if ((pos + len) > data_bits)
            return false;

        data_t reg_val;

        if (!read(reg, reg_val))
            return false;

        data_t mask;

        if (len >= data_bits)
            mask = (data_t)(~0);
        else
            mask = (((data_t)1 << len) - 1);

        value = (reg_val >> pos) & mask;

        return true;
    }

    bool writeBits(reg_t reg, uint8_t pos, uint8_t len, data_t value)
    {
        const uint8_t data_bits = sizeof(data_t) * 8;

        if (len == 0)
            return false;

        if (len > data_bits)
            return false;

        if ((pos + len) > data_bits)
            return false;

        data_t reg_val;

        if (!read(reg, reg_val))
            return false;

        data_t mask;

        if (len >= data_bits)
            mask = (data_t)(~0);
        else
            mask = (((data_t)1 << len) - 1);

        reg_val &= ~(mask << pos);
        reg_val |= ((value & mask) << pos);

        return write(reg, reg_val);
    }

    bool readBit(reg_t reg, uint8_t pos, data_t &value)
    {
        return this->readBits(reg, pos, 1, value);
    }

    bool writeBit(reg_t reg, uint8_t pos, data_t value)
    {
        return this->writeBits(reg, pos, 1, value);
    }

private:
    Bus &bus;
};