/**
 * @file NgimuReceive.c
 * @author Seb Madgwick
 * @brief Module for receiving messages from an NGIMU.
 */

//------------------------------------------------------------------------------
// Includes

#include "NgimuReceive.h"
#include <stddef.h>
#include <stdio.h> // snprintf

//------------------------------------------------------------------------------
// Variable declarations

static OscSlipDecoder oscSlipDecoder;
static void (*receiveErrorCallback)(const char* const errorMessage);
static void (*sensorsCallback)(const NgimuSensors ngimuSensors);
static void (*quaternionCallback)(const NgimuQuaternion ngimuQuaternion);
static void (*eulerCallback)(const NgimuEuler ngimuEuler);

//------------------------------------------------------------------------------
// Function prototypes

static void ProcessPacket(OscPacket * const oscMessage);
static void ProcessMessage(const OscTimeTag * const oscTimeTag, OscMessage * const oscMessage);
static OscError ProcessAddress(const OscTimeTag * const oscTimeTag, OscMessage * const oscMessage);
static OscError ProcessSensors(const OscTimeTag * const oscTimeTag, OscMessage * const oscMessage);
static OscError ProcessQuaternion(const OscTimeTag * const oscTimeTag, OscMessage * const oscMessage);
static OscError ProcessEuler(const OscTimeTag * const oscTimeTag, OscMessage * const oscMessage);

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises module.  This function should be called once on system
 * start up.
 */
void NgimuReceiveInitialise() {
    OscSlipDecoderInitialise(&oscSlipDecoder);
    oscSlipDecoder.processPacket = ProcessPacket;
}

/**
 * @brief Sets receive error callback function.
 * @param newReceiveErrorCallback Receive error callback function.
 */
void NgimuReceiveSetReceiveErrorCallback(void (*newReceiveErrorCallback)(const char* const errorMessage)) {
    receiveErrorCallback = newReceiveErrorCallback;
}

/**
 * @brief Sets receive "/sensors" callback function.
 * @param newSensorsCallback "/sensors" callback function.
 */
void NgimuReceiveSetSensorsCallback(void (*newSensorsCallback)(const NgimuSensors ngimuSensors)) {
    sensorsCallback = newSensorsCallback;
}

/**
 * @brief Sets receive "/quaternion" callback function.
 * @param newQuaternionCallback "/quaternion" callback function.
 */
void NgimuReceiveSetQuaternionCallback(void (*newQuaternionCallback)(const NgimuQuaternion ngimuQuaternion)) {
    quaternionCallback = newQuaternionCallback;
}

/**
 * @brief Sets receive "/euler" callback function.
 * @param newEulerCallback "/euler" callback function.
 */
void NgimuReceiveSetEulerCallback(void (*newEulerCallback)(const NgimuEuler ngimuEuler)) {
    eulerCallback = newEulerCallback;
}

/**
 * @brief Process byte received from NGIMU via a serial communication channel.
 * This function should be called for each byte receive within a serial stream.
 * @param byte Serial byte
 */
void NgimuReceiveProcessSerialByte(const char byte) {
    OscSlipDecoderProcessByte(&oscSlipDecoder, byte);
}

/**
 * @brief Process UDP packet received from NGIMU via Wi-Fi.
 * @param source Address of source byte array.
 * @param sourceSize Source size.
 */
void NgimuReceiveProcessUdpPacket(const char * const source, const size_t sourceSize) {
    OscPacket oscPacket;
    OscPacketInitialiseFromCharArray(&oscPacket, source, sourceSize);
    oscPacket.processMessage = &ProcessMessage;
    OscPacketProcessMessages(&oscPacket);
}

/**
 * @brief Callback function executed for each OSC packet received by a SLIP
 * decoder.
 * @param oscPacket Address of the decoded OSC packet.
 */
static void ProcessPacket(OscPacket * const oscPacket) {
    oscPacket->processMessage = &ProcessMessage;
    OscError oscError = OscPacketProcessMessages(oscPacket);
    if ((oscError != OscErrorNone) && (receiveErrorCallback != NULL)) {
        receiveErrorCallback(OscErrorGetMessage(oscError));
    }
}

/**
 * @brief Callback function executed for each message found within received OSC
 * packet.
 * @param oscTimeTag OSC time tag associated with message.
 * @param oscMessage Address of OSC message.
 */
static void ProcessMessage(const OscTimeTag * const oscTimeTag, OscMessage * const oscMessage) {
    const OscError oscError = ProcessAddress(oscTimeTag, oscMessage);
    if ((oscError != OscErrorNone) && (receiveErrorCallback != NULL)) {
        receiveErrorCallback(OscErrorGetMessage(oscError));
    }
}

/**
 * @brief Process OSC message according to OSC address pattern.
 * @param oscTimeTag OSC time tag associated with message.
 * @param oscMessage Address of OSC message.
 * @return Error code (0 if successful).
 */
static OscError ProcessAddress(const OscTimeTag * const oscTimeTag, OscMessage * const oscMessage) {

    // Process known message types
    if (OscAddressMatch(oscMessage->oscAddressPattern, "/sensors")) {
        return ProcessSensors(oscTimeTag, oscMessage);
    }
    if (OscAddressMatch(oscMessage->oscAddressPattern, "/quaternion")) {
        return ProcessQuaternion(oscTimeTag, oscMessage);
    }
    if (OscAddressMatch(oscMessage->oscAddressPattern, "/euler")) {
        return ProcessEuler(oscTimeTag, oscMessage);
    }

    // OSC address not recognised
    if (receiveErrorCallback != NULL) {
        static char string[256];
        snprintf(string, sizeof (string), "OSC address pattern not recognised: %s", oscMessage->oscAddressPattern);
        receiveErrorCallback(string);
    }
    return OscErrorNone;
}

/**
 * @brief Process "/sensors" message.
 * @param oscTimeTag OSC time tag associated with message.
 * @param oscMessage Address of OSC message.
 * @return Error code (0 if successful).
 */
static OscError ProcessSensors(const OscTimeTag * const oscTimeTag, OscMessage * const oscMessage) {

    // Do nothing if no callback assigned
    if (sensorsCallback == NULL) {
        return OscErrorNone;
    }

    // Get timestamp
    NgimuSensors ngimuSensors;
    ngimuSensors.timestamp = *oscTimeTag;

    // Get gyroscope X axis
    OscError oscError;
    oscError = OscMessageGetArgumentAsFloat32(oscMessage, &ngimuSensors.gyroscopeX);
    if (oscError != OscErrorNone) {
        return oscError;
    }

    // Get gyroscope Y axis
    oscError = OscMessageGetArgumentAsFloat32(oscMessage, &ngimuSensors.gyroscopeY);
    if (oscError != OscErrorNone) {
        return oscError;
    }

    // Get gyroscope Z axis
    oscError = OscMessageGetArgumentAsFloat32(oscMessage, &ngimuSensors.gyroscopeZ);
    if (oscError != OscErrorNone) {
        return oscError;
    }

    // Get accelerometer X axis
    oscError = OscMessageGetArgumentAsFloat32(oscMessage, &ngimuSensors.accelerometerX);
    if (oscError != OscErrorNone) {
        return oscError;
    }

    // Get accelerometer Y axis
    oscError = OscMessageGetArgumentAsFloat32(oscMessage, &ngimuSensors.accelerometerY);
    if (oscError != OscErrorNone) {
        return oscError;
    }

    // Get accelerometer Z axis
    oscError = OscMessageGetArgumentAsFloat32(oscMessage, &ngimuSensors.accelerometerZ);
    if (oscError != OscErrorNone) {
        return oscError;
    }

    // Get magnetometer X axis
    oscError = OscMessageGetArgumentAsFloat32(oscMessage, &ngimuSensors.magnetometerX);
    if (oscError != OscErrorNone) {
        return oscError;
    }

    // Get magnetometer Y axis
    oscError = OscMessageGetArgumentAsFloat32(oscMessage, &ngimuSensors.magnetometerY);
    if (oscError != OscErrorNone) {
        return oscError;
    }

    // Get magnetometer Z axis
    oscError = OscMessageGetArgumentAsFloat32(oscMessage, &ngimuSensors.magnetometerZ);
    if (oscError != OscErrorNone) {
        return oscError;
    }

    // Get barometer
    oscError = OscMessageGetArgumentAsFloat32(oscMessage, &ngimuSensors.barometer);
    if (oscError != OscErrorNone) {
        return oscError;
    }

    // Callback
    sensorsCallback(ngimuSensors);
    return OscErrorNone;
}

/**
 * @brief Process "/quaternion" message.
 * @param oscTimeTag OSC time tag associated with message.
 * @param oscMessage Address of OSC message.
 * @return Error code (0 if successful).
 */
static OscError ProcessQuaternion(const OscTimeTag * const oscTimeTag, OscMessage * const oscMessage) {

    // Do nothing if no callback assigned
    if (quaternionCallback == NULL) {
        return OscErrorNone;
    }

    // Get timestamp
    NgimuQuaternion ngimuQuaternion;
    ngimuQuaternion.timestamp = *oscTimeTag;

    // Get W element
    OscError oscError;
    oscError = OscMessageGetArgumentAsFloat32(oscMessage, &ngimuQuaternion.w);
    if (oscError != OscErrorNone) {
        return oscError;
    }

    // Get X element
    oscError = OscMessageGetArgumentAsFloat32(oscMessage, &ngimuQuaternion.x);
    if (oscError != OscErrorNone) {
        return oscError;
    }

    // Get Y element
    oscError = OscMessageGetArgumentAsFloat32(oscMessage, &ngimuQuaternion.y);
    if (oscError != OscErrorNone) {
        return oscError;
    }

    // Get Z element
    oscError = OscMessageGetArgumentAsFloat32(oscMessage, &ngimuQuaternion.z);
    if (oscError != OscErrorNone) {
        return oscError;
    }

    // Callback
    quaternionCallback(ngimuQuaternion);
    return OscErrorNone;
}

/**
 * @brief Process "/euler" message.
 * @param oscTimeTag OSC time tag associated with message.
 * @param oscMessage Address of OSC message.
 * @return Error code (0 if successful).
 */
static OscError ProcessEuler(const OscTimeTag * const oscTimeTag, OscMessage * const oscMessage) {

    // Do nothing if no callback assigned
    if (eulerCallback == NULL) {
        return OscErrorNone;
    }

    // Get timestamp
    NgimuEuler ngimuEuler;
    ngimuEuler.timestamp = *oscTimeTag;

    // Get roll
    OscError oscError;
    oscError = OscMessageGetArgumentAsFloat32(oscMessage, &ngimuEuler.roll);
    if (oscError != OscErrorNone) {
        return oscError;
    }

    // Get pitch
    oscError = OscMessageGetArgumentAsFloat32(oscMessage, &ngimuEuler.pitch);
    if (oscError != OscErrorNone) {
        return oscError;
    }

    // Get yaw
    oscError = OscMessageGetArgumentAsFloat32(oscMessage, &ngimuEuler.yaw);
    if (oscError != OscErrorNone) {
        return oscError;
    }

    // Callback
    eulerCallback(ngimuEuler);
    return OscErrorNone;
}

//------------------------------------------------------------------------------
// End of file
