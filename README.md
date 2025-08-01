# BQ27621 Arduino Library

This Arduino library provides a comprehensive interface for the Texas Instruments BQ27621-G1 battery fuel gauge IC, enabling easy integration with Arduino-based projects. The library supports all major features of the BQ27621-G1 as described in the Technical Reference Manual (SLUUAD4C, December 2013 - Revised August 2014).

## Features

- **Control Commands**: Manage device states (SEALED/UNSEALED, CONFIG UPDATE, SHUTDOWN, etc.).
- **Standard Commands**: Read battery parameters such as Temperature, Voltage, StateOfCharge, RemainingCapacity, and more.
- **Extended Commands**: Access and modify Operation Configuration, Design Capacity, and block data operations.
- **Data Memory Access**: Configure safety parameters, charge termination, SOC thresholds, sleep current, terminate voltage, and battery chemistry ID.
- **Battery Profile Selection**: Support for default (CHEM_ID 0x1202) and alternate chemistry profiles (0x1210 for 4.3V, 0x354 for 4.35V).
- **I2C Communication**: Robust functions for reading and writing word and block data over I2C.

## Requirements

- **Hardware**:
  - Arduino-compatible board with I2C support (e.g., Arduino Uno, Mega, etc.).
  - BQ27621-G1 battery fuel gauge IC connected via I2C (SDA, SCL, power lines).
- **Software**:
  - Arduino IDE (version 1.8.0 or later recommended).
  - Wire library (included with Arduino IDE).
- **Dependencies**: None beyond the standard Arduino Wire library.

## Installation

1. **Download the Library**:
   - Clone this repository or download the ZIP file.
   - Extract the `BQ27621` folder containing `BQ27621.h`, `BQ27621.cpp`, and the example sketch.

2. **Install the Library**:
   - Place the `BQ27621` folder in your Arduino `libraries` directory (typically `~/Arduino/libraries/`).
   - Restart the Arduino IDE if it was open during installation.

3. **Connect the Hardware**:
   - Connect the BQ27621-G1 to your Arduino:
     - SDA to Arduino SDA pin.
     - SCL to Arduino SCL pin.
     - VCC to 3.3V or 5V (check your board's compatibility).
     - GND to Arduino GND.
   - Verify the I2C address (default: 0x55). Adjust in the constructor if different.

4. **Open the Example**:
   - In the Arduino IDE, go to `File > Examples > BQ27621 > BQ27621_Example`.
   - Upload the example sketch to your Arduino.

## Usage

The library provides a `BQ27621` class with methods to interact with the fuel gauge. Below is a basic example of initializing the device and reading battery parameters:

```cpp
#include <Wire.h>
#include "BQ27621.h"

BQ27621 batteryGauge; // Default I2C address: 0x55

void setup() {
    Serial.begin(9600);
    while (!Serial);

    if (!batteryGauge.begin()) {
        Serial.println("Failed to initialize BQ27621!");
        while (1);
    }
    Serial.println("BQ27621 Initialized Successfully");

    // Configure battery parameters
    batteryGauge.setUnsealed();
    batteryGauge.enterConfigUpdate();
    batteryGauge.setDesignCapacity(1200); // Set to 1200mAh
    batteryGauge.setTerminateVoltage(3200); // Set to 3200mV
    batteryGauge.exitConfigUpdate();
    batteryGauge.setSealed();
}

void loop() {
    Serial.print("Voltage: ");
    Serial.print(batteryGauge.getVoltage());
    Serial.println(" mV");

    Serial.print("State of Charge: ");
    Serial.print(batteryGauge.getStateOfCharge());
    Serial.println(" %");

    Serial.print("Remaining Capacity: ");
    Serial.print(batteryGauge.getRemainingCapacity());
    Serial.println(" mAh");

    delay(5000); // Update every 5 seconds
}
```

## Example Sketch

The provided example sketch (`BQ27621_Example.ino`) demonstrates the full range of library features, including:

- Initializing the BQ27621-G1 and verifying communication.
- Configuring the device in UNSEALED mode.
- Setting safety parameters, charge termination, SOC thresholds, and operation configurations.
- Selecting an alternate battery chemistry profile (e.g., CHEM_ID 0x1210 for 4.3V batteries).
- Reading and displaying battery status (voltage, SOC, capacity, current, power, etc.).
- Toggling the GPOUT pin for debugging.

To use the example:
1. Open `BQ27621_Example.ino` in the Arduino IDE.
2. Adjust configuration parameters (e.g., DesignCapacity, TerminateVoltage) based on your battery specifications.
3. Upload the sketch and monitor the Serial output at 9600 baud.

## Library Methods

### Constructor
- `BQ27621(uint8_t i2cAddress = 0x55)`: Initialize with the I2C address (default: 0x55).

### Initialization
- `bool begin()`: Initialize I2C communication and verify device presence.

### Control Commands
- `uint16_t getControlStatus()`: Read the control status register.
- `void setBatteryInsert()`: Signal battery insertion.
- `void setBatteryRemove()`: Signal battery removal.
- `void enterShutdown()`: Enter SHUTDOWN mode.
- `void setSealed()`: Transition to SEALED mode.
- `void setUnsealed()`: Transition to UNSEALED mode (using default keys 0x8000 0x8000).
- `void toggleGPOUT()`: Pulse the GPOUT pin for 1ms.
- `void selectAltChem1()`: Select CHEM_ID 0x1210 (4.3V).
- `void selectAltChem2()`: Select CHEM_ID 0x354 (4.35V).
- `void enterConfigUpdate()`: Enter CONFIG UPDATE mode.
- `void exitConfigUpdate()`: Exit CONFIG UPDATE without OCV measurement.
- `void exitResim()`: Exit CONFIG UPDATE with resimulation.

### Standard Commands
- `uint16_t getTemperature()`: Read temperature in 0.1K.
- `void setTemperature(uint16_t temp)`: Set temperature (if OpConfig[TEMPS] = 1).
- `uint16_t getVoltage()`: Read cell voltage in mV.
- `uint16_t getFlags()`: Read status flags.
- `uint16_t getNominalAvailableCapacity()`: Read uncompensated capacity in mAh.
- `uint16_t getFullAvailableCapacity()`: Read uncompensated full capacity in mAh.
- `uint16_t getRemainingCapacity()`: Read compensated remaining capacity in mAh.
- `uint16_t getFullChargeCapacity()`: Read compensated full capacity in mAh.
- `int16_t getEffectiveCurrent()`: Read estimated current in mA.
- `int16_t getAveragePower()`: Read average power in mW.
- `uint16_t getStateOfCharge()`: Read state of charge in %.
- `uint16_t getInternalTemperature()`: Read internal temperature in 0.1K.
- `uint16_t getRemainingCapacityUnfiltered()`: Read unfiltered remaining capacity in mAh.
- `uint16_t getRemainingCapacityFiltered()`: Read filtered remaining capacity in mAh.
- `uint16_t getFullChargeCapacityUnfiltered()`: Read unfiltered full capacity in mAh.
- `uint16_t getFullChargeCapacityFiltered()`: Read filtered full capacity in mAh.
- `uint16_t getStateOfChargeUnfiltered()`: Read unfiltered state of charge in %.

### Extended Commands
- `uint16_t getOpConfig()`: Read operation configuration.
- `uint16_t getDesignCapacity()`: Read design capacity in mAh.
- `void setDesignCapacity(uint16_t capacity)`: Set design capacity.
- `uint8_t getBlockDataChecksum()`: Read block data checksum.
- `void setBlockDataChecksum(uint8_t checksum)`: Set block data checksum.
- `void setBlockDataControl(uint8_t value)`: Control data access mode.
- `void selectDataClass(uint8_t classId)`: Select data memory class.
- `void selectDataBlock(uint8_t blockId)`: Select data block.
- `void writeBlockData(uint8_t offset, uint8_t* data, uint8_t len)`: Write block data.
- `void readBlockData(uint8_t offset, uint8_t* data, uint8_t len)`: Read block data.

### Data Memory Configuration
- `void setSafetyParameters(uint16_t ot, uint16_t ut, uint8_t tHysteresis)`: Set over-temperature, under-temperature, and hysteresis.
- `void setChargeTerminationParameters(uint16_t minTaperCap, uint8_t taperWindow, int8_t fcSet, uint8_t fcClear)`: Set charge termination parameters.
- `void setSOCThresholds(uint8_t socfSet, uint8_t socfClear)`: Set SOC final discharge thresholds.
- `void setOpConfig(uint16_t config)`: Set operation configuration.
- `void setOpConfigB(uint8_t config)`: Set operation configuration B.
- `void setSleepCurrent(uint16_t current)`: Set sleep current threshold.
- `void setTerminateVoltage(uint16_t voltage)`: Set terminate voltage.
- `void setChemID(uint16_t chemId)`: Set chemistry ID.

## Notes

- **I2C Address**: The default I2C address is 0x55. If your BQ27621-G1 uses a different address, specify it in the constructor.
- **SEALED vs. UNSEALED Mode**: Configuration changes require UNSEALED mode. The library uses the default unseal keys (0x8000 0x8000). For production, set the device to SEALED mode after configuration.
- **Battery Chemistry**: The library supports switching between three chemistry profiles (0x1202, 0x1210, 0x354). Ensure the selected profile matches your battery's maximum charging voltage (4.2V, 4.3V, or 4.35V).
- **Power Modes**: The BQ27621-G1 automatically manages power modes (NORMAL, SLEEP, HIBERNATE, SHUTDOWN). Use `enterShutdown()` carefully, as it requires a GPOUT pin transition to wake up.
- **Error Handling**: The `begin()` function checks for device presence. Always verify initialization before proceeding.
- **Checksums**: Block data operations include automatic checksum calculation to ensure data integrity.

## Troubleshooting

- **No Communication**: Verify I2C connections, pull-up resistors (4.7kΩ recommended), and the correct I2C address.
- **Invalid Readings**: Ensure the device is properly configured (e.g., correct chemistry ID, design capacity). Check if the device is in SEALED mode, which restricts some operations.
- **Configuration Issues**: Enter UNSEALED mode before making configuration changes. Use `enterConfigUpdate()` for data memory modifications.

## License

This library is released under the MIT License. See the `LICENSE` file for details.

## Acknowledgments

- Based on the Texas Instruments BQ27621-G1 Technical Reference Manual (SLUUAD4C).
- Developed for use with Arduino-compatible platforms.

For further details, refer to the BQ27621-G1 datasheet (SLUSB3) and Quick Start Guide (SLUUAP5) available at [www.ti.com](http://www.ti.com).
