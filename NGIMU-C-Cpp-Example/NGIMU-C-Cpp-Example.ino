/**
 * @file NGIMU-Teensy-IO-Expansion-Example.ino
 * @author Seb Madgwick
 * @brief Example for receiving data from an NGIMU on a Teensy via the NGIMU
 * serial interface.
 *
 * Device:
 * Teensy 3.2
 *
 * IDE/compiler:
 * Arduino 1.8.5 and Teensy Loader 1.4
 *
 * The OSC99 source files (i.e. the "Osc99" directory) must be added to the
 * Arduino libraries folder.  See: https://www.arduino.cc/en/guide/libraries
 *
 * Lower performance devices such as the Arduino MEGA do not have enough memory
 * to use this example 'as is'.  The value of MAX_TRANSPORT_SIZE must be reduced
 * to 150 in OscCommon.h if this example is used on such devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "NgimuReceive.h"

void setup() {

    // Initialise Teensy serial
    Serial.begin(115200); // Teensy USB (baud rate irrelevant)
    Serial1.begin(115200); // NGIMU serial (baud rate must match NGIMU settings)

    // Initialise NGIMU receive module
    NgimuReceiveInitialise();

    // Assign NGIMU receive callback functions
    NgimuReceiveSetReceiveErrorCallback(ngimuReceiveErrorCallback);
    NgimuReceiveSetSensorsCallback(ngimuSensorsCallback);
    NgimuReceiveSetQuaternionCallback(ngimuQuaternionCallback);
    NgimuReceiveSetEulerCallback(ngimuEulerCallback);
}

void loop() {

    // Process each received byte
    while (Serial1.available() > 0) {
        NgimuReceiveProcessSerialByte(Serial1.read());
    }
}

// This function is called each time there is a receive error
void ngimuReceiveErrorCallback(const char* const errorMessage) {
    Serial.print(errorMessage);
    Serial.print("\r\n");
}

// This function is called each time a "/sensors" message is received
void ngimuSensorsCallback(const NgimuSensors ngimuSensors) {
    Serial.print("/sensors, ");
    Serial.print(ngimuSensors.gyroscopeX);
    Serial.print(", ");
    Serial.print(ngimuSensors.gyroscopeY);
    Serial.print(", ");
    Serial.print(ngimuSensors.gyroscopeZ);
    Serial.print(", ");
    Serial.print(ngimuSensors.accelerometerX);
    Serial.print(", ");
    Serial.print(ngimuSensors.accelerometerY);
    Serial.print(", ");
    Serial.print(ngimuSensors.accelerometerZ);
    Serial.print(", ");
    Serial.print(ngimuSensors.magnetometerX);
    Serial.print(", ");
    Serial.print(ngimuSensors.magnetometerY);
    Serial.print(", ");
    Serial.print(ngimuSensors.magnetometerZ);
    Serial.print(ngimuSensors.barometer);
    Serial.print("\r\n");
}

// This function is called each time a "/quaternion" message is received
void ngimuQuaternionCallback(const NgimuQuaternion ngimuQuaternion) {
    Serial.print("/quaternion, ");
    Serial.print(ngimuQuaternion.w);
    Serial.print(", ");
    Serial.print(ngimuQuaternion.x);
    Serial.print(", ");
    Serial.print(ngimuQuaternion.y);
    Serial.print(", ");
    Serial.print(ngimuQuaternion.z);
    Serial.print("\r\n");
}

// This function is called each time a "/euler" message is received.
void ngimuEulerCallback(const NgimuEuler ngimuEuler) {
    Serial.print("/euler, ");
    Serial.print(ngimuEuler.roll);
    Serial.print(", ");
    Serial.print(ngimuEuler.pitch);
    Serial.print(", ");
    Serial.print(ngimuEuler.yaw);
    Serial.print("\r\n");
}
