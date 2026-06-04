# 7Semi Bus

Universal I2C and SPI communication framework for sensor drivers.

## Features

- I2C support
- SPI support
- 8-bit and 16-bit register addressing
- 8-bit and 16-bit data access
- Big-endian and little-endian support
- Burst read/write
- Bit-level register operations
- Hardware-independent sensor drivers

## Installation

Arduino IDE:

Sketch → Include Library → Add ZIP Library

## Example

```cpp
#include <7Semi_Bus.h>
#include <7Semi_I2C.h>

I2C_Interface bus;

BusIO_7Semi<
    I2C_Interface,
    1,
    1> reg(bus);
```

## Supported Configurations

| Register | Data |
|-----------|------|
| 8-bit | 8-bit |
| 8-bit | 16-bit |
| 16-bit | 8-bit |
| 16-bit | 16-bit |

## License

MIT
