/*
 * config.h
 *
 *  Created on: Aug 6, 2026
 *      Author: Mike
 */

#ifndef CONFIG_H_
#define CONFIG_H_
#pragma once

//Define temperature probe pin
#define DHTPIN          32
#define DHTTYPE         DHT11

#define BROKER_IP    "192.168.0.6"
#define CLIENT_ID    "keezer"


// Structure to hold the pin configuration for a single scale
struct ScalePins {
    uint8_t dataPin;
    uint8_t clockPin;
};

// Centralized pin definitions for the kegs (Add/Change these to your actual ESP32 pins)
const ScalePins KEG_PINS[] = {
    {23, 22},  // Keg 1: DATA, CLK
    {19, 18},  // Keg 2: DATA, CLK
    {17, 16}   // Keg 3: DATA, CLK
};

// Automatically calculate the total number of kegs based on the pin array
const uint8_t KEGCOUNT = sizeof(KEG_PINS) / sizeof(KEG_PINS[0]);



#endif /* CONFIG_H_ */
