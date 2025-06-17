#ifndef SCIOSENSE_UFM01_C_H
#define SCIOSENSE_UFM01_C_H

#include "ScioSense_Ufm01_defines.h"

#include <stdbool.h>
#include <inttypes.h>

typedef struct ScioSense_Ufm01_IO
{
    Result  (*read)     (void* config, const uint16_t address, uint8_t* data, const size_t size);
    Result  (*write)    (void* config, const uint16_t address, uint8_t* data, const size_t size);
    Result  (*clear)    (void* config);
    void    (*wait)     (const uint32_t ms);
    Ufm01_Protocol protocol;
    void* config;
} ScioSense_Ufm01_IO;

typedef struct ScioSense_Ufm01
{
    ScioSense_Ufm01_IO      io;
    uint8_t                 deviceIdData[           UFM01_BUFFER_ID_LENGTH                 ];
    char                    deviceIdDataString[     UFM01_BUFFER_ID_STRING_LENGTH + 1      ];
    uint8_t                 softwareVersionData[    UFM01_BUFFER_SOFTWARE_VERSION_LENGTH   ];
    uint8_t                 statusData[             UFM01_BUFFER_STATUS_LENGTH             ];
    uint8_t                 deviceOneWireData[      UFM01_BUFFER_ONEWIRE_DATA_LENGTH       ];
    float                   accumulatedFlowLiters;
    float                   instantFlowDataLitersPerHour;
    float                   temperatureDataDegC;
    uint32_t                startupFlowRateDeciLitersPerHour;
    Ufm01_Frequency         acquisitionFrequencyHz;
    Ufm01_OperatingMode     measurementMode;
} ScioSense_Ufm01;

static inline Result            Ufm01_Reset                             (ScioSense_Ufm01* apc1);                            // Resets the UFM01 and resets all internal variables
static inline Result            Ufm01_Update                            (ScioSense_Ufm01* apc1);                            // Reads measurement data; Automaticcaly calls "RequestMeasurement" if in passive mode;
static inline uint64_t          Ufm01_DecodeDecimalNibbles              (const uint8_t *data, size_t len);                  // Transforms a byte array whose nibbels are decimals into a number. For example, {0x12, 0x34} into 1234 

static inline Result            Ufm01_InvokePassiveMeasurement          (ScioSense_Ufm01* ufm01);                           // Send the command to set the UFM01 into passive mode
static inline Result            Ufm01_InvokeActiveMeasurement           (ScioSense_Ufm01* ufm01);                           // Send the command to set the UFM01 into active mode
static inline Result            Ufm01_InvokeReset                       (ScioSense_Ufm01* ufm01);                           // Send the command to reset the UFM01
static inline Result            Ufm01_InvokeClearAccumulatedFlow        (ScioSense_Ufm01* ufm01);                           // Clears the accumulated flow register
static inline Result            Ufm01_InvokeGetSoftwareVersion          (ScioSense_Ufm01* ufm01);                           // Reads the software version
static inline Result            Ufm01_InvokeGetSensorDataWithId         (ScioSense_Ufm01* ufm01);                           // Performs a passive measurement together with the sensor ID
static inline Result            Ufm01_InvokeGetSensorDataNoId           (ScioSense_Ufm01* ufm01);                           // Performs a passive measurement without the sensor ID
static inline Result            Ufm01_InvokeGetSensorDataActive         (ScioSense_Ufm01* ufm01);                           // Get the measurement when the sensor is running in active mode
static inline Result            Ufm01_InvokeWriteConfiguration          (ScioSense_Ufm01* ufm01, Ufm01_Frequency acquisitionFrequency, uint32_t startFlowLitresPerHour);    // Sends configuration into the sensor
static inline Result            Ufm01_WriteConfiguration                (ScioSense_Ufm01* ufm01, Ufm01_Frequency acquisitionFrequency, float startFlowlitersPerHour);       // Prepares and writes configuration to sensor
static inline bool              Ufm01_IsConnected                       (ScioSense_Ufm01* ufm01);                           // Returns if the UFM01 initialization was correct
static inline float             Ufm01_GetAccumulatedFlowLiters          (ScioSense_Ufm01* ufm01);                           // Get the accumulated flow in liters
static inline float             Ufm01_GetInstantFlowLitersPerHour       (ScioSense_Ufm01* ufm01);                           // Get the instant flow in liters per hour
static inline float             Ufm01_GetTemperatureDegC                (ScioSense_Ufm01* ufm01);                           // Get the temperature in degrees celsius
static inline Ufm01_ErrorCode   Ufm01_GetStatus                         (ScioSense_Ufm01* ufm01);                           // Get the error codes from the status bytes
static inline uint64_t          Ufm01_GetDeviceId                       (ScioSense_Ufm01* ufm01);                           // Get the device ID
static inline void              Ufm01_CreateDeviceIdString              (ScioSense_Ufm01* ufm01);                           // Prepare the device ID string
static inline char*             Ufm01_GetDeviceIdString                 (ScioSense_Ufm01* ufm01);                           // Get a string with the device ID
static inline uint32_t          Ufm01_GetManufacturingDate              (ScioSense_Ufm01* ufm01);                           // Get the manufacturing date in format YYMMDD
static inline uint16_t          Ufm01_GetSerialNumber                   (ScioSense_Ufm01* ufm01);                           // Get the serial number
static inline Result            Ufm01_ClearAccumulatedFlow              (ScioSense_Ufm01* ufm01);                           // Clears the accumulated flow
static inline uint32_t          Ufm01_GetSoftwareVersion                (ScioSense_Ufm01* ufm01);                           // Get the software version
static inline Ufm01_Frequency   Ufm01_GetAcquisitionFrequency           (ScioSense_Ufm01* ufm01);                           // Get the current acquisition frequency in Hz
static inline float             Ufm01_GetStartupFlowRate                (ScioSense_Ufm01* ufm01);                           // Get the current startup flow rate in liters per hour
static inline float             Ufm01_AccumulatedFlowToLitersUart       (uint8_t* accumulatedFlowData, uint8_t accumulatedFlowFlag);    // Transform the Accumulated Flow measurement read with UART into the actual value
static inline float             Ufm01_InstantFlowToLitersPerHourUart    (uint8_t* instantFlowData, uint8_t instantFlowFlag);            // Transform the Instant Flow measurement read with UART into the actual value
static inline float             Ufm01_TemperatureToDegCUart             (uint8_t* temperatureData);                                     // Transform the Temperature measurement read with UART into the actual value
static inline float             Ufm01_AccumulatedFlowToLitersOneWire    (uint8_t* accumulatedFlowData);                     // Transform the Accumulated Flow measurement read with One Wire into the actual value
static inline float             Ufm01_InstantFlowToLitersPerHourOneWire (uint8_t* instantFlowData);                         // Transform the Instant Flow measurement read with One Wire into the actual value
static inline float             Ufm01_TemperatureToDegCOneWire          (uint8_t* temperatureData);                         // Transform the Temperature measurement read with One Wire into the actual value
static inline uint8_t           Ufm01_CalculateChecksum_Uart            (uint8_t *val, uint8_t startCrcByte, uint8_t stopCrcByte);      // Perform the UART checksum calculation
static inline uint8_t           Ufm01_CalculateCrc8_OneWire             (uint8_t *val, uint8_t length);                     // Perform the One-Wire CRC calculation
static inline Result            Ufm01_CheckValueCrcOneWire              (uint8_t* measurement, uint8_t measurementCrc);     // Check if a One-Wire measurement has the correct CRC
static inline Result            Ufm01_CheckAcknowledgeResponse          (uint8_t* data, const Ufm01_CommandResponse size);  // Check if the sensor acknowledges the command (for commands that return no payload)
static inline Result            Ufm01_CheckSensorDataWithIdResponse     (uint8_t* data, const Ufm01_CommandResponse size);  // Check if the passive measurement with sensor ID is correct
static inline Result            Ufm01_CheckSensorDataNoIdResponse       (uint8_t* data, const Ufm01_CommandResponse size);  // Check if the passive measurement without sensor ID is correct
static inline Result            Ufm01_CheckSensorDataActiveResponse     (uint8_t* data, const Ufm01_CommandResponse size);  // Check if the active measurement is correct
static inline Result            Ufm01_CheckWriteConfigurationResponse   (uint8_t* data, const Ufm01_CommandResponse size , Ufm01_Command configurationCommand);     // Check if the configuration reply is correct
static inline Result            Ufm01_CheckReadSoftwareVersionResponse  (uint8_t* data, const Ufm01_CommandResponse size);  // Check if the software version received is correct

#include "ScioSense_Ufm01.inl.h"
#endif // SCIOSENSE_UFM01_C_H