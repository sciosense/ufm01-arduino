#ifndef SCIOSENSE_UFM01_H
#define SCIOSENSE_UFM01_H

#include <stdint.h>

#include <Arduino.h>
#include <Stream.h>

#include "lib/ufm01/ScioSense_Ufm01.h"
#include "lib/io/ScioSense_IOInterface_Arduino_Serial.h"
#include "lib/io/ScioSense_IOInterface_Arduino_OneWire.h"

class UFM01 : public ScioSense_Ufm01
{
public:
    UFM01();
    virtual ~UFM01();

public:
    inline void begin(OneWire* oneWire);                                // Connects to UFM01 using the given OneWire object
    inline void begin(Stream* serial);                                  // Connnects to UFM01 using the given Stream(Serial) object
    inline bool init();                                                 // Resets the device to IDLE and reads PartID and FirmwareVersion
    bool        isConnected();                                          // Checks if the read firmware version is plausible; returns true, if so.

public:
    void enableDebugging(Stream& debugStream);                          // Enables the debug log. The output is written to the given debugStream
    void disableDebugging();                                            // Stops the debug log if enabled. Does nothing otherwise.

public:
    inline void     clear();                                            // Clears IO buffers of the Stream device
    inline void     reset();                                            // Resets the UFM01 and resets all internal variables
    inline Result   update();                                           // Reads measurement data; Automaticaly calls "RequestMeasurement" if in passive mode;
    
public:
    inline bool     setOperatingMode(const Ufm01_OperatingMode mode);                                               // Toggle between active and passive measurement mode
    inline Result   clearAccumulatedFlow();                                                                         // Clears the accumulated flow
    inline Result   writeConfiguration(Ufm01_Frequency acquisitionFrequencyHz, float startupFlowRateLitresPerHour); // Writes the acquisition frequency and startup flow rate into the device

public:
    inline float            getAccumulatedFlowLiters();                 // Returns the accumulated flow in liters
    inline float            getAccumulatedFlowM3();                     // Returns the accumulated flow in cubic meters
    inline float            getInstantFlowLitersPerHour();              // Returns the instant flow in liters per hour
    inline float            getTempKelvin();                            // Returns the temperature in degrees Kelvin
    inline float            getTempCelsius();                           // Returns the temperature in degrees Celsius
    inline float            getTempFahrenheit();                        // Returns the temperature in degrees Farenheit
    inline uint32_t         getSoftwareVersion();                       // Returns the software version
    inline uint64_t         getDeviceId();                              // Returns the device ID
    inline char*            getDeviceIdString();                        // Returns the device ID string
    inline uint32_t         getManufacturingDate();                     // Returns the device manufacturing date
    inline uint16_t         getSerialNumber();                          // Returns the device serial number
    inline Ufm01_Frequency  getAcquisitionFrequency();                  // Returns the current acquisition frequency
    inline float            getStartupFlowRateLitersPerHour();          // Returns the current startup flow rate in liters per hour
    
public:
    inline Ufm01_ErrorCode  getError();                                 // Returns the errors reported from the device. The data is reported in a 16bit variable with ST1 as MSB and ST2 as LSB


protected:
    ScioSense_Arduino_OneWire_Config    oneWireConfig;
    ScioSense_Arduino_Serial_Config     serialConfig;

private:
    Stream* debugStream;
};

#include "ufm01.inl.h"

#endif //SCIOSENSE_UFM01_H