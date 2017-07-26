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
 * Arduino 1.6.12 and Teensy Loader 1.27
 *
 * The OSC99 source files (i.e. the "Osc99" directory) must be added to the
 * Arduino libraries folder.  See: https://www.arduino.cc/en/guide/libraries
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
    char string[256];
    snprintf(string, sizeof (string),
            "/sensors, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\r\n",
            ngimuSensors.gyroscopeX,
            ngimuSensors.gyroscopeY,
            ngimuSensors.gyroscopeZ,
            ngimuSensors.accelerometerX,
            ngimuSensors.accelerometerY,
            ngimuSensors.accelerometerZ,
            ngimuSensors.magnetometerX,
            ngimuSensors.magnetometerY,
            ngimuSensors.magnetometerZ,
            ngimuSensors.barometer);
    Serial.print(string);
}

// This function is called each time a "/quaternion" message is received
void ngimuQuaternionCallback(const NgimuQuaternion ngimuQuaternion) {
    char string[256];
    snprintf(string, sizeof (string),
            "/quaternion, %f, %f, %f, %f\r\n",
            ngimuQuaternion.w,
            ngimuQuaternion.x,
            ngimuQuaternion.y,
            ngimuQuaternion.z);
    Serial.print(string);
}

// This function is called each time a "/euler" message is received.
void ngimuEulerCallback(const NgimuEuler ngimuEuler) {
    char string[256];
    snprintf(string, sizeof (string),
            "/euler, %f, %f, %f\r\n",
            ngimuEuler.roll,
            ngimuEuler.pitch,
            ngimuEuler.yaw);
    Serial.print(string);
}
