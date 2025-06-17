#ifndef SCIOSENSE_IO_INTERFACE_ARDUINO_ONEWIRE_H
#define SCIOSENSE_IO_INTERFACE_ARDUINO_ONEWIRE_H

#include <Arduino.h>
#include <OneWire.h>

//// simple example IO Interface implementation

typedef struct ScioSense_Arduino_OneWire_Config
{
    OneWire* oneWire;
} ScioSense_Arduino_OneWire_Config;

static inline void ScioSense_Arduino_OneWire_Wait(uint32_t ms)
{
    delay(ms);
}

static inline int8_t ScioSense_Arduino_OneWire_Read(void* config, const uint16_t address, uint8_t* data, const size_t size)
{
    OneWire* oneWire    = ((ScioSense_Arduino_OneWire_Config*)config)->oneWire;

    oneWire->read_bytes(data, size);

    return 0; // RESULT_OK
}

static inline int8_t ScioSense_Arduino_OneWire_Write(void* config, const uint16_t address, uint8_t* data, const size_t size)
{
    OneWire* oneWire = ((ScioSense_Arduino_OneWire_Config*)config)->oneWire;

    uint8_t sensorPresent = oneWire->reset();
    ScioSense_Arduino_OneWire_Wait(5);
    sensorPresent = oneWire->reset();

    if ( !sensorPresent )
    {
        return 1; // RESULT_IO_ERROR;
    }
    oneWire->write( (uint8_t)address );

    for ( uint8_t i=0; i<size; i++ )
    {
        oneWire->write(data[i]);
    }

    return 0; // RESULT_OK
}

#endif // SCIOSENSE_IO_INTERFACE_ARDUINO_ONEWIRE_H