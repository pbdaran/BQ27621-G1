#ifndef BQ27621_H
#define BQ27621_H

#include <Arduino.h>
#include <Wire.h>

class BQ27621 {
public:
    // Constructor
    BQ27621(uint8_t i2cAddress = 0x55);

    // Initialization
    bool begin();

    // Control Commands
    uint16_t getControlStatus();
    void setBatteryInsert();
    void setBatteryRemove();
    void enterShutdown();
    void setSealed();
    void setUnsealed();
    void toggleGPOUT();
    void selectAltChem1();
    void selectAltChem2();
    void enterConfigUpdate();
    void exitConfigUpdate();
    void exitResim();

    // Standard Commands
    uint16_t getTemperature();
    void setTemperature(uint16_t temp);
    uint16_t getVoltage();
    uint16_t getFlags();
    uint16_t getNominalAvailableCapacity();
    uint16_t getFullAvailableCapacity();
    uint16_t getRemainingCapacity();
    uint16_t getFullChargeCapacity();
    int16_t getEffectiveCurrent();
    int16_t getAveragePower();
    uint16_t getStateOfCharge();
    uint16_t getInternalTemperature();
    uint16_t getRemainingCapacityUnfiltered();
    uint16_t getRemainingCapacityFiltered();
    uint16_t getFullChargeCapacityUnfiltered();
    uint16_t getFullChargeCapacityFiltered();
    uint16_t getStateOfChargeUnfiltered();

    // Extended Commands
    uint16_t getOpConfig();
    uint16_t getDesignCapacity();
    void setDesignCapacity(uint16_t capacity);
    uint8_t getBlockDataChecksum();
    void setBlockDataChecksum(uint8_t checksum);
    void setBlockDataControl(uint8_t value);
    void selectDataClass(uint8_t classId);
    void selectDataBlock(uint8_t blockId);
    void writeBlockData(uint8_t offset, uint8_t* data, uint8_t len);
    void readBlockData(uint8_t offset, uint8_t* data, uint8_t len);

    // Data Memory Parameter Access
    void setSafetyParameters(uint16_t ot, uint16_t ut, uint8_t tHysteresis);
    void setChargeTerminationParameters(uint16_t minTaperCap, uint8_t taperWindow, int8_t fcSet, uint8_t fcClear);
    void setSOCThresholds(uint8_t socfSet, uint8_t socfClear);
    void setOpConfig(uint16_t config);
    void setOpConfigB(uint8_t config);
    void setSleepCurrent(uint16_t current);
    void setTerminateVoltage(uint16_t voltage);
    void setChemID(uint16_t chemId);

private:
    uint8_t _i2cAddress;

    // I2C Communication Helpers
    uint16_t readWord(uint8_t command);
    void writeWord(uint8_t command, uint16_t data);
    uint8_t readByte(uint8_t command);
    void writeByte(uint8_t command, uint8_t data);
    void writeBlock(uint8_t command, uint8_t* data, uint8_t len);
    void readBlock(uint8_t command, uint8_t* data, uint8_t len);

    // CRC Calculation for Data Memory
    uint8_t calculateChecksum(uint8_t* data, uint8_t len);
};

#endif
