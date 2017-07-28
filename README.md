# NGIMU-C-Cpp-Example

This is an example project demonstrating how to receive data from an NGIMU on a C/C++ platform using the [OSC99](https://github.com/xioTechnologies/OSC99) library.  The example setup uses a [Teensy 3.2](https://www.pjrc.com/store/teensy32.html) with a serial connection to the NGIMU.  The receive code (implemented in *NgimuReceive.h* and *NgimuReceive.c*) is platform independent and can be used on any C/C++ platform for receiving data via both serial and UDP.

## Teensy board connections

* 3V3 - NGIMU serial interface voltage output
* GND - NGIMU serial interface ground
* RX1 - NGIMU serial interface TX
* TX1 - NGIMU serial interface RX

![](https://github.com/xioTechnologies/NGIMU-C-Cpp-Example/blob/master/Example%20Setup.jpg)
