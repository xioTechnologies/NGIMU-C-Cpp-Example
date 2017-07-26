/**
 * @file NgimuReceive.h
 * @author Seb Madgwick
 * @brief Module for receiving messages from an NGIMU.
 */

#ifndef NGIMU_RECEIVE_H
#define NGIMU_RECEIVE_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Includes

#include "Osc99.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Timestamp and argument values for "/sensors" message.
 */
typedef struct {
    OscTimeTag timestamp;
    float gyroscopeX;
    float gyroscopeY;
    float gyroscopeZ;
    float accelerometerX;
    float accelerometerY;
    float accelerometerZ;
    float magnetometerX;
    float magnetometerY;
    float magnetometerZ;
    float barometer;
} NgimuSensors;

/**
 * @brief Timestamp and argument values for "/quaternion" message.
 */
typedef struct {
    OscTimeTag timestamp;
    float w;
    float x;
    float y;
    float z;
} NgimuQuaternion;

/**
 * @brief Timestamp and argument values for "/euler" message.
 */
typedef struct {
    OscTimeTag timestamp;
    float roll;
    float pitch;
    float yaw;
} NgimuEuler;

//------------------------------------------------------------------------------
// Function prototypes

void NgimuReceiveInitialise();
void NgimuReceiveSetReceiveErrorCallback(void (*newReceiveErrorCallback)(const char* const errorMessage));
void NgimuReceiveSetSensorsCallback(void (*newSensorsCallback)(const NgimuSensors ngimuSensors));
void NgimuReceiveSetQuaternionCallback(void (*newQuaternionCallback)(const NgimuQuaternion ngimuQuaternion));
void NgimuReceiveSetEulerCallback(void (*newEulerCallback)(const NgimuEuler ngimuEuler));
void NgimuReceiveProcessSerialByte(const char byte);
void NgimuReceiveProcessUdpPacket(const char * const source, const size_t sourceSize);

#ifdef __cplusplus
}
#endif

#endif

//------------------------------------------------------------------------------
// End of file
