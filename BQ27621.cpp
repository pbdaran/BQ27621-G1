#include "BQ27621.h"

BQ27621::BQ27621(uint8_t i2cAddress) : _i2cAddress(i2cAddress) {}

bool BQ27621::begin() {
    Wire.begin();
    // Verify device communication
    Wire.beginTransmission(_i2cAddress);
    return (Wire.endTransmission() == 0);
}

uint16_t BQ27621::getControlStatus() {
    writeWord(0x00, 0x0000); // CONTROL_STATUS subcommand
    return readWord(0x00);
}

void BQ27621::setBatteryInsert() {
    writeWord(0x00, 0x000C); // BAT_INSERT subcommand
}

void BQ27621::setBatteryRemove() {
    writeWord(0x00, 0x000D); // BAT_REMOVE subcommand
}

void BQ27621::enterShutdown() {
    writeWord(0x00, 0x001C); // SHUTDOWN subcommand
}

void BQ27621::setSealed() {
    writeWord(0x00, 0x0020); // SEALED subcommand
}

void BQ27621::setUnsealed() {
    writeWord(0x00, 0x8000); // First key
    writeWord(0x00, 0x8000); // Second key
}

void BQ27621::toggleGPOUT() {
    writeWord(0x00, 0x0023); // TOGGLE_GPOUT subcommand
}

void BQ27621::selectAltChem1() {
    writeWord(0x00, 0x0033); // ALT_CHEM1 subcommand
}

void BQ27621::selectAltChem2() {
    writeWord(0x00, 0x0034); // ALT_CHEM2 subcommand
}

void BQ27621::enterConfigUpdate() {
    writeWord(0x00, 0x0042); // SET_CFGUPDATE subcommand
}

void BQ27621::exitConfigUpdate() {
    writeWord(0x00, 0x0043); // EXIT_CFGUPDATE subcommand
}

void BQ27621::exitResim() {
    writeWord(0x00, 0x0044); // EXIT_RESIM subcommand
}

uint16_t BQ27621::getTemperature() {
    return readWord(0x02);
}

void BQ27621::setTemperature(uint16_t temp) {
    writeWord(0x02, temp);
}

uint16_t BQ27621::getVoltage() {
    return readWord(0x04);
}

uint16_t BQ27621::getFlags() {
    return readWord(0x06);
}

uint16_t BQ27621::getNominalAvailableCapacity() {
    return readWord(0x08);
}

uint16_t BQ27621::getFullAvailableCapacity() {
    return readWord(0x0A);
}

uint16_t BQ27621::getRemainingCapacity() {
    return readWord(0x0C);
}

uint16_t BQ27621::getFullChargeCapacity() {
    return readWord(0x0E);
}

int16_t BQ27621::getEffectiveCurrent() {
    return (int16_t)readWord(0x10);
}

int16_t BQ27621::getAveragePower() {
    return (int16_t)readWord(0x18);
}

uint16_t BQ27621::getStateOfCharge() {
    return readWord(0x1C);
}

uint16_t BQ27621::getInternalTemperature() {
    return readWord(0x1E);
}

uint16_t BQ27621::getRemainingCapacityUnfiltered() {
    return readWord(0x28);
}

uint16_t BQ27621::getRemainingCapacityFiltered() {
    return readWord(0x2A);
}

uint16_t BQ27621::getFullChargeCapacityUnfiltered() {
    return readWord(0x2C);
}

uint16_t BQ27621::getFullChargeCapacityFiltered() {
    return readWord(0x2E);
}

uint16_t BQ27621::getStateOfChargeUnfiltered() {
    return readWord(0x30);
}

uint16_t BQ27621::getOpConfig() {
    return readWord(0x3A);
}

uint16_t BQ27621::getDesignCapacity() {
    return readWord(0x3C);
}

void BQ27621::setDesignCapacity(uint16_t capacity) {
    writeWord(0x3C, capacity);
}

uint8_t BQ27621::getBlockDataChecksum() {
    return readByte(0x60);
}

void BQ27621::setBlockDataChecksum(uint8_t checksum) {
    writeByte(0x60, checksum);
}

void BQ27621::setBlockDataControl(uint8_t value) {
    writeByte(0x61, value);
}

void BQ27621::selectDataClass(uint8_t classId) {
    writeByte(0x3E, classId);
}

void BQ27621::selectDataBlock(uint8_t blockId) {
    writeByte(0x3F, blockId);
}

void BQ27621::writeBlockData(uint8_t offset, uint8_t* data, uint8_t len) {
    writeBlock(0x40 + offset, data, len);
}

void BQ27621::readBlockData(uint8_t offset, uint8_t* data, uint8_t len) {
    readBlock(0x40 + offset, data, len);
}

void BQ27621::setSafetyParameters(uint16_t ot, uint16_t ut, uint8_t tHysteresis) {
    uint8_t data[5];
    data[0] = ot & 0xFF;
    data[1] = (ot >> 8) & 0xFF;
    data[2] = ut & 0xFF;
    data[3] = (ut >> 8) & 0xFF;
    data[4] = tHysteresis;
    selectDataClass(2); // Safety class
    selectDataBlock(0);
    writeBlockData(0, data, 5);
    setBlockDataChecksum(calculateChecksum(data, 5));
}

void BQ27621::setChargeTerminationParameters(uint16_t minTaperCap, uint8_t taperWindow, int8_t fcSet, uint8_t fcClear) {
    uint8_t data[6];
    data[0] = minTaperCap & 0xFF;
    data[1] = (minTaperCap >> 8) & 0xFF;
    data[2] = taperWindow;
    data[3] = fcSet;
    data[4] = fcClear;
    selectDataClass(36); // Charge Termination class
    selectDataBlock(0);
    writeBlockData(0, data, 5);
    setBlockDataChecksum(calculateChecksum(data, 5));
}

void BQ27621::setSOCThresholds(uint8_t socfSet, uint8_t socfClear) {
    uint8_t data[2];
    data[0] = socfSet;
    data[1] = socfClear;
    selectDataClass(49); // Discharge class
    selectDataBlock(0);
    writeBlockData(2, data, 2);
    setBlockDataChecksum(calculateChecksum(data, 2));
}

void BQ27621::setOpConfig(uint16_t config) {
    uint8_t data[2];
    data[0] = config & 0xFF;
    data[1] = (config >> 8) & 0xFF;
    selectDataClass(64); // Registers class
    selectDataBlock(0);
    writeBlockData(0, data, 2);
    setBlockDataChecksum(calculateChecksum(data, 2));
}

void BQ27621::setOpConfigB(uint8_t config) {
    uint8_t data[1] = {config};
    selectDataClass(64); // Registers class
    selectDataBlock(0);
    writeBlockData(2, data, 1);
    setBlockDataChecksum(calculateChecksum(data, 1));
}

void BQ27621::setSleepCurrent(uint16_t current) {
    uint8_t data[2];
    data[0] = current & 0xFF;
    data[1] = (current >> 8) & 0xFF;
    selectDataClass(82); // State class
    selectDataBlock(0);
    writeBlockData(0, data, 2);
    setBlockDataChecksum(calculateChecksum(data, 2));
}

void BQ27621::setTerminateVoltage(uint16_t voltage) {
    uint8_t data[2];
    data[0] = voltage & 0xFF;
    data[1] = (voltage >> 8) & 0xFF;
    selectDataClass(82); // State class
    selectDataBlock(0);
    writeBlockData(9, data, 2);
    setBlockDataChecksum(calculateChecksum(data, 2));
}

void BQ27621::setChemID(uint16_t chemId) {
    uint8_t data[2];
    data[0] = chemId & 0xFF;
    data[1] = (chemId >> 8) & 0xFF;
    selectDataClass(82); // State class
    selectDataBlock(0);
    writeBlockData(36, data, 2);
    setBlockDataChecksum(calculateChecksum(data, 2));
}

uint16_t BQ27621::readWord(uint8_t command) {
    Wire.beginTransmission(_i2cAddress);
    Wire.write(command);
    Wire.endTransmission();
    Wire.requestFrom(_i2cAddress, (uint8_t)2);
    uint16_t low = Wire.read();
    uint16_t high = Wire.read();
    return (high << 8) | low;
}

void BQ27621::writeWord(uint8_t command, uint16_t data) {
    Wire.beginTransmission(_i2cAddress);
    Wire.write(command);
    Wire.write(data & 0xFF);
    Wire.write((data >> 8) & 0xFF);
    Wire.endTransmission();
}

uint8_t BQ27621::readByte(uint8_t command) {
    Wire.beginTransmission(_i2cAddress);
    Wire.write(command);
    Wire.endTransmission();
    Wire.requestFrom(_i2cAddress, (uint8_t)1);
    return Wire.read();
}

void BQ27621::writeByte(uint8_t command, uint8_t data) {
    Wire.beginTransmission(_i2cAddress);
    Wire.write(command);
    Wire.write(data);
    Wire.endTransmission();
}

void BQ27621::writeBlock(uint8_t command, uint8_t* data, uint8_t len) {
    Wire.beginTransmission(_i2cAddress);
    Wire.write(command);
    for (uint8_t i = 0; i < len; i++) {
        Wire.write(data[i]);
    }
    Wire.endTransmission();
}

void BQ27621::readBlock(uint8_t command, uint8_t* data, uint8_t len) {
    Wire.beginTransmission(_i2cAddress);
    Wire.write(command);
    Wire.endTransmission();
    Wire.requestFrom(_i2cAddress, len);
    for (uint8_t i = 0; i < len; i++) {
        data[i] = Wire.read();
    }
}

uint8_t BQ27621::calculateChecksum(uint8_t* data, uint8_t len) {
    uint8_t sum = 0;
    for (uint8_t i = 0; i < len; i++) {
        sum += data[i];
    }
    return 255 - sum;
}
