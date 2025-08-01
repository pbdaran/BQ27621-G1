// Example program demonstrating BQ27621-G1 battery gauge IC functionality
#include <Wire.h>
#include "BQ27621.h"

// Initialize BQ27621 with default I2C address (0x55)
BQ27621 batteryGauge;

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ; // Wait for serial port to connect
    }

    // Initialize I2C communication and verify device
    if (!batteryGauge.begin()) {
        Serial.println("Failed to initialize BQ27621!");
        while (1);
    }
    Serial.println("BQ27621 Initialized Successfully");

    // Set to UNSEALED mode for configuration
    batteryGauge.setUnsealed();
    delay(100);

    // Enter CONFIG UPDATE mode
    batteryGauge.enterConfigUpdate();
    delay(100);

    // Configure battery parameters
    // Set safety parameters: Over-Temperature (55°C), Under-Temperature (0°C), Hysteresis (5°C)
    batteryGauge.setSafetyParameters(550, 0, 50);

    // Set charge termination parameters
    batteryGauge.setChargeTerminationParameters(25, 40, -1, 98);

    // Set SOC thresholds for final discharge warning
    batteryGauge.setSOCThresholds(2, 5);

    // Set Operation Configuration (enable external temp, disable BAT_LOW)
    batteryGauge.setOpConfig(0xB4D8 & ~0x08); // Clear BATLOWEN bit

    // Enable SOC smoothing
    batteryGauge.setOpConfigB(0x0C | 0x04); // Set SMTHEN bit

    // Set sleep current threshold (10mA)
    batteryGauge.setSleepCurrent(10);

    // Set terminate voltage (3200mV)
    batteryGauge.setTerminateVoltage(3200);

    // Select alternate chemistry (e.g., CHEM_ID 0x1210 for 4.3V)
    batteryGauge.selectAltChem1();
    batteryGauge.setChemID(0x1210);

    // Exit CONFIG UPDATE mode
    batteryGauge.exitConfigUpdate();
    delay(100);

    // Set to SEALED mode for normal operation
    batteryGauge.setSealed();
}

void loop() {
    // Read and display battery status every 5 seconds
    Serial.println("=== Battery Status ===");
    
    // Get control status
    uint16_t controlStatus = batteryGauge.getControlStatus();
    Serial.print("Control Status: 0x");
    Serial.println(controlStatus, HEX);

    // Read temperature (in 0.1K)
    uint16_t temp = batteryGauge.getTemperature();
    Serial.print("Temperature: ");
    Serial.print((temp / 10.0) - 273.15); // Convert to Celsius
    Serial.println(" °C");

    // Read voltage
    uint16_t voltage = batteryGauge.getVoltage();
    Serial.print("Voltage: ");
    Serial.print(voltage);
    Serial.println(" mV");

    // Read flags
    uint16_t flags = batteryGauge.getFlags();
    Serial.print("Flags: 0x");
    Serial.println(flags, HEX);

    // Read capacities
    Serial.print("Nominal Available Capacity: ");
    Serial.print(batteryGauge.getNominalAvailableCapacity());
    Serial.println(" mAh");

    Serial.print("Full Available Capacity: ");
    Serial.print(batteryGauge.getFullAvailableCapacity());
    Serial.println(" mAh");

    Serial.print("Remaining Capacity: ");
    Serial.print(batteryGauge.getRemainingCapacity());
    Serial.println(" mAh");

    Serial.print("Full Charge Capacity: ");
    Serial.print(batteryGauge.getFullChargeCapacity());
    Serial.println(" mAh");

    // Read current and power
    Serial.print("Effective Current: ");
    Serial.print(batteryGauge.getEffectiveCurrent());
    Serial.println(" mA");

    Serial.print("Average Power: ");
    Serial.print(batteryGauge.getAveragePower());
    Serial.println(" mW");

    // Read state of charge
    Serial.print("State of Charge: ");
    Serial.print(batteryGauge.getStateOfCharge());
    Serial.println(" %");

    // Toggle GPOUT for testing
    batteryGauge.toggleGPOUT();

    delay(5000); // Update every 5 seconds
}
