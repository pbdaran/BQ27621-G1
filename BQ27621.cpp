#include "BQ27621.h"

BQ27621::BQ27621(uint8_t i2cAddress) : _i2cAddress(i2cAddress) {}

bool BQ27621::begin() {
    Wire.begin();
    Wire.beginTransmission(_i2cAddress);
    return (Wire.endTransmission() == 0);
}

// -------- Control Commands ----------
uint16_t BQ27621::getControlStatus() {
    writeWord(0x00, 0x0000); // CONTROL_STATUS subcommand
    return readWord(0x00);
}

void BQ27621::setBatteryInsert()    { writeWord(0x00, 0x000C); }
void BQ27621::setBatteryRemove()    { writeWord(0x00, 0x000D); }
void BQ27621::enterShutdown()       { writeWord(0x00, 0x001C); }
void BQ27621::setSealed()           { writeWord(0x00, 0x0020); }
void BQ27621::setUnsealed()         { writeWord(0x00, 0x8000); writeWord(0x00, 0x8000); }
void BQ27621::toggleGPOUT()         { writeWord(0x00, 0x0023); }

// Fixed: Correct subcommands for chemistry selection per TRM
void BQ27621::selectAltChem1()      { writeWord(0x00, 0x0031); } // ALT_CHEM1 = 0x0031
void BQ27621::selectAltChem2()      { writeWord(0x00, 0x0032); } // ALT_CHEM2 = 0x0032

// Fixed: Proper subcommand for SET_CFGUPDATE
void BQ27621::enterConfigUpdate()   { writeWord(0x00, 0x0013); } // SET_CFGUPDATE
void BQ27621::exitConfigUpdate()    { writeWord(0x00, 0x0043); } // EXIT_CFGUPDATE
void BQ27621::exitResim()           { writeWord(0x00, 0x0044); } // EXIT_RESIM

// ------- Standard Commands ----------
uint16_t BQ27621::getTemperature()                 { return readWord(0x02); }
void BQ27621::setTemperature(uint16_t temp)        { writeWord(0x02, temp); } // Only if OpConfig[TEMPS]=1!
uint16_t BQ27621::getVoltage()                     { return readWord(0x04); }
uint16_t BQ27621::getFlags()                       { return readWord(0x06); }
uint16_t BQ27621::getNominalAvailableCapacity()    { return readWord(0x08); }
uint16_t BQ27621::getFullAvailableCapacity()       { return readWord(0x0A); }
uint16_t BQ27621::getRemainingCapacity()           { return readWord(0x0C); }
uint16_t BQ27621::getFullChargeCapacity()          { return readWord(0x0E); }
int16_t  BQ27621::getEffectiveCurrent()            { return (int16_t)readWord(0x10); }
int16_t  BQ27621::getAveragePower()                { return (int16_t)readWord(0x18); }
uint16_t BQ27621::getStateOfCharge()               { return readWord(0x1C); }
uint16_t BQ27621::getInternalTemperature()         { return readWord(0x1E); }
uint16_t BQ27621::getRemainingCapacityUnfiltered() { return readWord(0x28); }
uint16_t BQ27621::getRemainingCapacityFiltered()   { return readWord(0x2A); }
uint16_t BQ27621::getFullChargeCapacityUnfiltered(){ return readWord(0x2C); }
uint16_t BQ27621::getFullChargeCapacityFiltered()  { return readWord(0x2E); }
uint16_t BQ27621::getStateOfChargeUnfiltered()     { return readWord(0x30); }

// ---------- Extended --------------
uint16_t BQ27621::getOpConfig()        { return readWord(0x3A); }
uint16_t BQ27621::getDesignCapacity()  { return readWord(0x3C); }

// ----------- Improved DesignCapacity Setter -----------
// If permanent==false, updates DesignCapacity( ) register only -- will be lost after reset (debug use).
// If permanent==true, uses Data Flash block write to update the value in subclass 82, offset 3. Persists after reset!
void BQ27621::setDesignCapacity(uint16_t capacity, bool permanent) {
    if (permanent) {
        // Data Flash (persistent) update: subclass 82 (State), block 0, offset 3
        uint8_t block[32];
        readDataBlock(82, 0, block);   // Read the current 32-byte block
        block[3] = capacity & 0xFF;    // LSB @ offset 3
        block[4] = (capacity >> 8) & 0xFF; // MSB @ offset 4
        writeDataBlock(82, 0, block);  // Write back modified block (with correct checksum)
    } else {
        // Volatile, non-persistent, debug only; TRM says this doesn't persist after reset
        writeWord(0x3C, capacity);
    }
}

// -------- Block Data Helpers ------------
uint8_t BQ27621::getBlockDataChecksum()        { return readByte(0x60); }
void BQ27621::setBlockDataChecksum(uint8_t cs) { writeByte(0x60, cs); }
void BQ27621::setBlockDataControl(uint8_t val) { writeByte(0x61, val); }
void BQ27621::selectDataClass(uint8_t clId)    { writeByte(0x3E, clId); }
void BQ27621::selectDataBlock(uint8_t blockId) { writeByte(0x3F, blockId); }
void BQ27621::writeBlockData(uint8_t offset, const uint8_t* data, uint8_t len) {
    writeBlock(0x40 + offset, data, len);
}
void BQ27621::readBlockData(uint8_t offset, uint8_t* data, uint8_t len) {
    readBlock(0x40 + offset, data, len);
}

//========= Data Memory Routines (Safe Block/Checksum) ==========
// All parameter updates now use read-modify-write of entire 32-byte block
// Helper: read 32-byte Data Flash block
void BQ27621::readDataBlock(uint8_t classId, uint8_t blockId, uint8_t* blockData32) {
    setBlockDataControl(0x00);         // enable block data control
    selectDataClass(classId);          // set subclass
    selectDataBlock(blockId);          // set block (0 = offsets 0..31)
    delay(5);                          // short delay for gauge, not always needed
    readBlockData(0, blockData32, 32); // read entire 32 bytes
}

// Helper: write 32-byte Data Flash block and set its checksum
void BQ27621::writeDataBlock(uint8_t classId, uint8_t blockId, const uint8_t* blockData32) {
    setBlockDataControl(0x00);
    selectDataClass(classId);
    selectDataBlock(blockId);
    writeBlockData(0, blockData32, 32);                                 // Write 32 bytes
    uint8_t cs = calculateChecksum(blockData32, 32);
    setBlockDataChecksum(cs);                                           // Write checksum; commits block!
    delay(5);
}

// Easy field update: copy whole block, modify fields, commit
void BQ27621::setSafetyParameters(uint16_t ot, uint16_t ut, uint8_t tHys) {
    uint8_t block[32];
    readDataBlock(2, 0, block);      // Safety class
    block[0] = ot & 0xFF;
    block[1] = (ot >> 8) & 0xFF;
    block[2] = ut & 0xFF;
    block[3] = (ut >> 8) & 0xFF;
    block[4] = tHys;
    writeDataBlock(2, 0, block);
}

void BQ27621::setChargeTerminationParameters(uint16_t minTaperCap, uint8_t taperWindow, int8_t fcSet, uint8_t fcClear) {
    uint8_t block[32];
    readDataBlock(36, 0, block);         // Charge class
    block[0] = minTaperCap & 0xFF;
    block[1] = (minTaperCap >> 8) & 0xFF;
    block[2] = taperWindow;
    block[5] = fcSet;
    block[6] = fcClear;
    writeDataBlock(36, 0, block);
}

// Update SOC thresholds; all 4 fields (SOC1 Set, SOC1 Clear, SOCF Set, SOCF Clear) in offsets 0–3 (TRM p29)
void BQ27621::setSOCThresholds(uint8_t soc1Set, uint8_t soc1Clear, uint8_t socfSet, uint8_t socfClear) {
    uint8_t block[32];
    readDataBlock(49, 0, block);
    block[0] = soc1Set;
    block[1] = soc1Clear;
    block[2] = socfSet;
    block[3] = socfClear;
    writeDataBlock(49, 0, block);
}

void BQ27621::setOpConfig(uint16_t config) {
    uint8_t block[32];
    readDataBlock(64, 0, block);
    block[0] = config & 0xFF;
    block[1] = (config >> 8) & 0xFF;
    writeDataBlock(64, 0, block);
}

void BQ27621::setOpConfigB(uint8_t config) {
    uint8_t block[32];
    readDataBlock(64, 0, block);
    block[2] = config;
    writeDataBlock(64, 0, block);
}

void BQ27621::setSleepCurrent(uint16_t current) {
    uint8_t block[32];
    readDataBlock(82, 0, block);
    block[24] = current & 0xFF;
    block[25] = (current >> 8) & 0xFF;
    writeDataBlock(82, 0, block);
}

void BQ27621::setTerminateVoltage(uint16_t voltage) {
    uint8_t block[32];
    readDataBlock(82, 0, block);
    block[9]  = voltage & 0xFF;
    block[10] = (voltage >> 8) & 0xFF;
    writeDataBlock(82, 0, block);
}

void BQ27621::setChemID(uint16_t chemId) {
    uint8_t block[32];
    readDataBlock(82, 0, block);
    block[36] = chemId & 0xFF;
    block[37] = (chemId >> 8) & 0xFF;
    writeDataBlock(82, 0, block);
}

// ---------- I2C Primitives ----------
uint16_t BQ27621::readWord(uint8_t command) {
    Wire.beginTransmission(_i2cAddress);
    Wire.write(command);
    Wire.endTransmission();
    Wire.requestFrom(_i2cAddress, (uint8_t)2);
    uint8_t lsb = Wire.read();
    uint8_t msb = Wire.read();
    return (msb << 8) | lsb;
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

// Blocks for Data Flash access
void BQ27621::writeBlock(uint8_t command, const uint8_t* data, uint8_t len) {
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

uint8_t BQ27621::calculateChecksum(const uint8_t* data, uint8_t len) {
    uint16_t sum = 0;
    for (uint8_t i = 0; i < len; i++) sum += data[i];
    return 255 - (sum & 0xFF);
}
