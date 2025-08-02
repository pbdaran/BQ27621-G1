// Example: BQ27621-G1 Battery Fuel Gauge Arduino Demo
#include <Wire.h>
#include "BQ27621.h"

// Instantiate BQ27621 using the default I2C address (0x55)
BQ27621 batteryGauge;

void setup() {
    Serial.begin(9600);
    while (!Serial) {;} // Wait for serial port

    // 1. Initialize I2C and check for device
    if (!batteryGauge.begin()) {
        Serial.println("BQ27621 init failed -- device not responding!");
        while (1);
    }
    Serial.println("BQ27621 Initialized Successfully");

    // 2. UNSEAL the device to allow parameter/config writes
    batteryGauge.setUnsealed();
    delay(100);

    // 3. Enter CONFIG UPDATE mode
    batteryGauge.enterConfigUpdate();
    delay(100);

    // 4. Wait until CONFIG UPDATE flag is set (optional, improves reliability)
    uint16_t flags;
    unsigned long t0 = millis();
    do {
        flags = batteryGauge.getFlags();
        if (flags & (1 << 11)) break;          // Bit 11: CFGUPD
        delay(20);
    } while (millis() - t0 < 1500);
    Serial.println("Entered CONFIG UPDATE mode");

    // ========== Main Configuration Parameters ==========

    // Set safety: OverTemp (55°C), UnderTemp (0°C), Hyst (5°C) -- all x10 for 0.1°C units
    batteryGauge.setSafetyParameters(550, 0, 50);

    // Set charge termination: MinTaperCap=25mAh, TaperWin=40s, FC set=-1, FC clear=98 (see TRM)
    batteryGauge.setChargeTerminationParameters(25, 40, -1, 98);

    // Set SOC thresholds: SOC1=10/15, SOCF=2/5 (covers both warnings)
    batteryGauge.setSOCThresholds(10, 15, 2, 5);

    // Set Operation Configuration: external temp, disable BATLOWEN for SOC_INT on GPOUT
    batteryGauge.setOpConfig(0xB4D8 & ~0x08);

    // Set OpConfigB: enable SOC smoothing
    batteryGauge.setOpConfigB(0x0C | 0x04);

    // Set sleep current threshold (mA)
    batteryGauge.setSleepCurrent(10);

    // Set terminate voltage (in mV)
    batteryGauge.setTerminateVoltage(3200);

    // Set Design Capacity:
    // - Volatile (debug): batteryGauge.setDesignCapacity(1200, false);
    // - Permanent (for shipped product): batteryGauge.setDesignCapacity(1200, true);
    batteryGauge.setDesignCapacity(1200, true);

    // --- Chemistry change recommendation ---
    // TI recommends selecting alt chemistry when in config update mode for CHEM_ID change.
    // For 4.3V maximum battery: ALT_CHEM1 --> CHEM_ID 0x1210
    batteryGauge.selectAltChem1();
    delay(100); // Allow the gauge processing time
    batteryGauge.setChemID(0x1210);

    // 5. Exit CONFIG UPDATE mode using exitConfigUpdate (exits WITHOUT resimulation; for most cases, preferred)
    batteryGauge.exitConfigUpdate();
    delay(100);

    // 6. Wait until CONFIG UPDATE bit clears, then SEAL (if desired for production)
    t0 = millis();
    do {
        flags = batteryGauge.getFlags();
        if (!(flags & (1 << 11))) break;       // Wait for CFGUPD clear
        delay(20);
    } while (millis() - t0 < 1500);

    batteryGauge.setSealed();

    Serial.println("Configuration complete. Entering monitoring mode.");
}

void loop() {
    Serial.println("=== Battery Status ===");
    uint16_t controlStatus = batteryGauge.getControlStatus();
    Serial.print("Control Status: 0x"); Serial.println(controlStatus, HEX);

    uint16_t temp = batteryGauge.getTemperature();
    Serial.print("Temperature: ");
    Serial.print((float)(temp) * 0.1 - 273.15, 1);
    Serial.println(" °C");

    uint16_t voltage = batteryGauge.getVoltage();
    Serial.print("Voltage: "); Serial.print(voltage); Serial.println(" mV");

    uint16_t flags = batteryGauge.getFlags();
    Serial.print("Flags: 0x"); Serial.println(flags, HEX);

    Serial.print("Nominal Available Capacity: ");
    Serial.print(batteryGauge.getNominalAvailableCapacity()); Serial.println(" mAh");

    Serial.print("Full Available Capacity: ");
    Serial.print(batteryGauge.getFullAvailableCapacity()); Serial.println(" mAh");

    Serial.print("Remaining Capacity: ");
    Serial.print(batteryGauge.getRemainingCapacity()); Serial.println(" mAh");

    Serial.print("Full Charge Capacity: ");
    Serial.print(batteryGauge.getFullChargeCapacity()); Serial.println(" mAh");

    Serial.print("Effective Current: ");
    Serial.print(batteryGauge.getEffectiveCurrent()); Serial.println(" mA");

    Serial.print("Average Power: ");
    Serial.print(batteryGauge.getAveragePower()); Serial.println(" mW");

    Serial.print("State of Charge: ");
    Serial.print(batteryGauge.getStateOfCharge()); Serial.println(" %");

    // Test pulse: GPOUT pin
    batteryGauge.toggleGPOUT();

    delay(5000);
}
