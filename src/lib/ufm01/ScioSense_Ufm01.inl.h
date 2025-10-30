#ifndef SCIOSENSE_UFM01_C_INL
#define SCIOSENSE_UFM01_C_INL

#include "ScioSense_Ufm01.h"

#include <math.h>

#define clear()             if (ufm01->io.clear) { ufm01->io.clear(ufm01->io.config); }
#define wait(ms)            ufm01->io.wait(ms)

#define hasAnyFlag(a, b)    (((a) & (b)) != 0)
#define hasFlag(a, b)       (((a) & (b)) == (b))

#define memset(a, b, s) for(size_t i = 0; i < s; i++) {a[i] = b;}
#define memcpy(a, b, s) for(size_t i = 0; i < s; i++) {a[i] = b[i];}

static inline uint8_t Ufm01_CompareArrays(const uint8_t* a, const uint8_t* b, size_t s)
{
    for (size_t i = 0; i < s; i++)
    {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

static inline uint16_t Ufm01_GetValueOf16(const uint8_t* data, const uint16_t resultAddress)
{
    return (uint16_t)((data[resultAddress] << 8) | data[resultAddress + 1]);
}

uint64_t Ufm01_DecodeDecimalNibbles(const uint8_t *data, size_t len)
{
    uint64_t result = 0;
    uint64_t multiplier = 1;

    for (size_t i = 0; i < len; ++i)
    {
        uint8_t current_byte = data[i];
        uint8_t lowNibble = current_byte & 0x0F;
        uint8_t highNibble = (current_byte >> 4) & 0x0F;

        if (lowNibble > 9 || highNibble > 9)
        {
            return 0xFFFFFFFF;
        }
        result += lowNibble * multiplier;
        multiplier *= 10;
        result += highNibble * multiplier;
        multiplier *= 10;
    }

    return result;
}

static void Ufm01_EncodeDecimalNibbles(uint32_t numberToEncode, uint8_t *buffer, size_t len)
{
    for (size_t i = 0; i < len; ++i)
    {
        uint8_t low_nibble = (uint8_t) (numberToEncode % 10);
        numberToEncode /= 10;
        uint8_t high_nibble = (uint8_t) (numberToEncode % 10);
        numberToEncode /= 10;

        buffer[i] = (high_nibble << 4) | low_nibble;
    }
}

static inline void Ufm01_CreateDeviceIdString(ScioSense_Ufm01* ufm01)
{
    memset( ufm01->deviceIdDataString, '\0', UFM01_BUFFER_ID_STRING_LENGTH );
    uint64_t deviceId = Ufm01_GetDeviceId(ufm01);
    for (int i = ( UFM01_BUFFER_ID_STRING_LENGTH - 1 ); i >= 0; --i )
    {
        ufm01->deviceIdDataString[i] = '0' + (char)(deviceId % 10);
        deviceId /= 10;
    }
}

static inline Result Ufm01_Read(ScioSense_Ufm01* ufm01, const uint16_t address, uint8_t* data, const size_t size)
{
    return ufm01->io.read(ufm01->io.config, 0, data, size);

    return RESULT_IO_ERROR;
}

static inline Result Ufm01_Write(ScioSense_Ufm01* ufm01, const uint16_t address, uint8_t* data, const size_t size)
{
    if (ufm01->io.protocol == UFM01_PROTOCOL_UART)
    {
        return ufm01->io.write(ufm01->io.config, 0x00, data, size);
    }
    else if (ufm01->io.protocol == UFM01_PROTOCOL_ONE_WIRE)
    {
        return ufm01->io.write(ufm01->io.config, address, data, size);
    }

    return RESULT_IO_ERROR;
}

static inline Result Ufm01_Invoke(ScioSense_Ufm01* ufm01, Ufm01_Command command, const size_t sizeCommand, uint8_t* resultBuf, const size_t sizeResponse)
{
    Result result;

    if( ufm01->io.protocol == UFM01_PROTOCOL_ONE_WIRE )
    {
        memset(resultBuf, 0x12, sizeResponse)
        size_t sizeData = sizeCommand - 1;
        uint8_t address = command[0];
        uint8_t data[sizeData];
        memcpy(data, (command+1), sizeData);
        result = Ufm01_Write(ufm01, address, data, sizeData);
    }
    else
    {
        result = Ufm01_Write(ufm01, 0, (uint8_t*)command, sizeCommand);
    }
    
    if (result == RESULT_OK)
    {
        if (resultBuf != NULL)
        {
            result = Ufm01_Read(ufm01, 0, resultBuf, sizeResponse);
        }
    }

    return result;
}

static inline Result Ufm01_InvokePassiveMeasurement(ScioSense_Ufm01* ufm01)
{
    Result result;

    if (ufm01->io.protocol != UFM01_PROTOCOL_UART)
    {
        result = RESULT_IO_ERROR;
        return result;
    }

    static Ufm01_Command commandSetPassiveMode = UFM01_COMMAND_SET_PASSIVE_MODE;

    size_t responseLength = UFM01_COMMAND_RESPONSE_ACKNOWLEDGE_LENGTH;
    uint8_t buff[UFM01_COMMAND_RESPONSE_ACKNOWLEDGE_LENGTH];
    
    result = Ufm01_Invoke(ufm01, commandSetPassiveMode, UFM01_COMMAND_LENGTH, buff, responseLength);

    if ( result == RESULT_OK )
    {
        result = Ufm01_CheckAcknowledgeResponse(buff, responseLength);
        if (result == RESULT_OK)
        {
            ufm01->measurementMode = UFM01_MEASUREMENT_MODE_PASSIVE;
        }
    }

    return result;
}

static inline Result Ufm01_InvokeActiveMeasurement(ScioSense_Ufm01* ufm01)
{
    Result result;

    if (ufm01->io.protocol != UFM01_PROTOCOL_UART)
    {
        result = RESULT_IO_ERROR;
        return result;
    }

    static Ufm01_Command commandSetActiveMode = UFM01_COMMAND_SET_ACTIVE_MODE;

    size_t responseLength = UFM01_COMMAND_RESPONSE_ACKNOWLEDGE_LENGTH;
    uint8_t buff[UFM01_COMMAND_RESPONSE_ACKNOWLEDGE_LENGTH];
    
    result = Ufm01_Invoke(ufm01, commandSetActiveMode, UFM01_COMMAND_LENGTH, buff, responseLength);

    if (result == RESULT_OK)
    {
        result = Ufm01_CheckAcknowledgeResponse(buff, responseLength);
        if (result == RESULT_OK)
        {
            ufm01->measurementMode = UFM01_MEASUREMENT_MODE_ACTIVE;
        }
    }

    return result;
}

static inline Result Ufm01_InvokeReset(ScioSense_Ufm01* ufm01)
{
    Result result = RESULT_INVALID;

    if (ufm01->io.protocol == UFM01_PROTOCOL_UART)
    {
        static Ufm01_Command commandReset = UFM01_COMMAND_RESET_MODULE;

        size_t responseLength = UFM01_COMMAND_RESPONSE_ACKNOWLEDGE_LENGTH;
        uint8_t buff[UFM01_COMMAND_RESPONSE_ACKNOWLEDGE_LENGTH];
        
        result = Ufm01_Invoke(ufm01, commandReset, UFM01_COMMAND_LENGTH, buff, responseLength);
        
        if (result == RESULT_OK)
        {
            result = Ufm01_CheckAcknowledgeResponse(buff, responseLength);
        }
    }

    if (ufm01->io.protocol == UFM01_PROTOCOL_ONE_WIRE)
    {
        uint8_t dummyData[1];
        result = Ufm01_Write(ufm01, UFM01_COMMAND_CODE_RESET_MODULE, (uint8_t*)dummyData, 0);
    }

    return result;
}

static inline Result Ufm01_InvokeClearAccumulatedFlow(ScioSense_Ufm01* ufm01)
{
    Result result;

    if (ufm01->io.protocol == UFM01_PROTOCOL_UART)
    {
        static Ufm01_Command commandClearAccumulatedFlow = UFM01_COMMAND_CLEAR_ACCUMULATED_FLOW;

        size_t responseLength = UFM01_COMMAND_RESPONSE_ACKNOWLEDGE_LENGTH;
        uint8_t buff[UFM01_COMMAND_RESPONSE_ACKNOWLEDGE_LENGTH];
        
        result = Ufm01_Invoke(ufm01, commandClearAccumulatedFlow, UFM01_COMMAND_LENGTH, buff, responseLength);

        if (result == RESULT_OK)
        {
            result = Ufm01_CheckAcknowledgeResponse(buff, responseLength);
        }
    }

    if (ufm01->io.protocol == UFM01_PROTOCOL_ONE_WIRE)
    {
        uint8_t dummyData[1];
        result = Ufm01_Write(ufm01, UFM01_COMMAND_CODE_CLEAR_ACCUMULATED, (uint8_t*)dummyData, 0);
    }

    return result;
}

static inline Result Ufm01_InvokeGetSoftwareVersion(ScioSense_Ufm01* ufm01)
{
    Result result = RESULT_INVALID;

    if (ufm01->io.protocol == UFM01_PROTOCOL_UART)
    {
        static Ufm01_Command commandGetSoftwareVersion = UFM01_COMMAND_GET_SOFTWARE_VERSION;

        size_t responseLength = UFM01_COMMAND_RESPONSE_GET_SOFTWARE_VERSION_LENGTH;
        uint8_t buff[UFM01_COMMAND_RESPONSE_GET_SOFTWARE_VERSION_LENGTH];
        
        result = Ufm01_Invoke(ufm01, commandGetSoftwareVersion, UFM01_COMMAND_LENGTH, buff, responseLength);

        if (result == RESULT_OK)
        {
            result = Ufm01_CheckReadSoftwareVersionResponse(buff, responseLength);

            if (result == RESULT_OK)
            {
                memcpy(ufm01->softwareVersionData, (buff+UFM01_COMMAND_RESPONSE_SOFTWARE_VERSION_START_BYTE_ADDRESS), UFM01_BUFFER_SOFTWARE_VERSION_LENGTH);
            }
        }
    }

    if (ufm01->io.protocol == UFM01_PROTOCOL_ONE_WIRE)
    {
        result = RESULT_NOT_ALLOWED;
    }

    return result;
}

static inline Result Ufm01_InvokeGetSensorDataWithId(ScioSense_Ufm01* ufm01)
{
    Result result = RESULT_IO_ERROR;

    if (ufm01->io.protocol == UFM01_PROTOCOL_UART)
    {
        static Ufm01_Command commandGetSensorDataWithId = UFM01_COMMAND_READ_SENSOR_DATA_WITH_ID;

        size_t responseLength = UFM01_COMMAND_RESPONSE_PASSIVE_MODE_WITH_ID_LENGTH;
        uint8_t buff[UFM01_COMMAND_RESPONSE_PASSIVE_MODE_WITH_ID_LENGTH];
        
        result = Ufm01_Invoke(ufm01, commandGetSensorDataWithId, UFM01_COMMAND_LENGTH, buff, responseLength);
        
        if (result == RESULT_OK)
        {
            result = Ufm01_CheckSensorDataWithIdResponse(buff, responseLength);

            if (result == RESULT_OK)
            {
                memcpy(ufm01->deviceIdData,         ( buff + UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_DEVICE_ID_ADDRESS       ),  UFM01_BUFFER_ID_LENGTH                 );
                memcpy(ufm01->statusData,           ( buff + UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_ST1_ADDRESS             ),  UFM01_BUFFER_STATUS_LENGTH             );
                
                ufm01->accumulatedFlowLiters =          Ufm01_AccumulatedFlowToLitersUart((     buff + UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_ACC_FLOW_ADDRESS),        buff[UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_ACC_FLOW_FLAG_ADDRESS]       );
                ufm01->instantFlowDataLitersPerHour =   Ufm01_InstantFlowToLitersPerHourUart((  buff + UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_INSTANT_FLOW_ADDRESS),    buff[UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_INSTANT_SIGN_ADDRESS]   );
                ufm01->temperatureDataDegC =            Ufm01_TemperatureToDegCUart((           buff + UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_TEMP_ADDRESS));
            }
        }
    }

    if (ufm01->io.protocol == UFM01_PROTOCOL_ONE_WIRE)
    {
        result = RESULT_NOT_ALLOWED;
    }

    return result;
}

static inline Result Ufm01_InvokeGetSensorDataNoId(ScioSense_Ufm01* ufm01)
{
    Result result = RESULT_IO_ERROR;

    if (ufm01->io.protocol == UFM01_PROTOCOL_UART)
    {
        static Ufm01_Command commandGetSensorDataNoId = UFM01_COMMAND_READ_SENSOR_DATA_NO_ID;

        size_t responseLength = UFM01_COMMAND_RESPONSE_PASSIVE_MODE_NO_ID_LENGTH;
        uint8_t buff[UFM01_COMMAND_RESPONSE_PASSIVE_MODE_NO_ID_LENGTH];
        
        result = Ufm01_Invoke(ufm01, commandGetSensorDataNoId, UFM01_COMMAND_LENGTH, buff, responseLength);
        
        if (result == RESULT_OK)
        {
            result = Ufm01_CheckSensorDataNoIdResponse(buff, responseLength);
            if (result == RESULT_OK)
            {
                memcpy(ufm01->statusData,           ( buff + UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_ST1_ADDRESS           ),  UFM01_BUFFER_STATUS_LENGTH             );
                
                ufm01->accumulatedFlowLiters =          Ufm01_AccumulatedFlowToLitersUart((     buff + UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_ACC_FLOW_ADDRESS),      buff[UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_ACC_FLOW_FLAG_ADDRESS]       );
                ufm01->instantFlowDataLitersPerHour =   Ufm01_InstantFlowToLitersPerHourUart((  buff + UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_INSTANT_FLOW_ADDRESS),  buff[UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_INSTANT_FLOW_SIGN_ADDRESS]   );
                ufm01->temperatureDataDegC =            Ufm01_TemperatureToDegCUart((           buff + UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_TEMP_ADDRESS));
            }
        }
    }

    if (ufm01->io.protocol == UFM01_PROTOCOL_ONE_WIRE)
    {
        result = RESULT_NOT_ALLOWED;
    }

    return result;
}

static inline Result Ufm01_InvokeGetSensorDataActive(ScioSense_Ufm01* ufm01)
{
    Result result = RESULT_IO_ERROR;

    if (ufm01->io.protocol == UFM01_PROTOCOL_UART)
    {
        size_t responseLength = UFM01_COMMAND_RESPONSE_ACTIVE_MODE_LENGTH;
        uint8_t buff[UFM01_COMMAND_RESPONSE_ACTIVE_MODE_LENGTH];
        
        result = Ufm01_Read(ufm01, 0, buff, responseLength);
        
        if (result == RESULT_OK)
        {
            result = Ufm01_CheckSensorDataActiveResponse(buff, responseLength);

            if (result == RESULT_OK)
            {
                memcpy(ufm01->deviceIdData,         ( buff + UFM01_COMMAND_RESPONSE_ACTIVE_DEVICE_ID_ADDRESS    ),  UFM01_BUFFER_ID_LENGTH                 );
                memcpy(ufm01->statusData,           ( buff + UFM01_COMMAND_RESPONSE_ACTIVE_ST1_ADDRESS          ),  UFM01_BUFFER_STATUS_LENGTH             );

                ufm01->accumulatedFlowLiters =          Ufm01_AccumulatedFlowToLitersUart((     buff + UFM01_COMMAND_RESPONSE_ACTIVE_ACC_FLOW_ADDRESS),     buff[UFM01_COMMAND_RESPONSE_ACTIVE_ACC_FLOW_FLAG_ADDRESS]       );
                ufm01->instantFlowDataLitersPerHour =   Ufm01_InstantFlowToLitersPerHourUart((  buff + UFM01_COMMAND_RESPONSE_ACTIVE_INSTANT_FLOW_ADDRESS), buff[UFM01_COMMAND_RESPONSE_ACTIVE_INSTANT_FLOW_SIGN_ADDRESS]   );
                ufm01->temperatureDataDegC =            Ufm01_TemperatureToDegCUart((           buff + UFM01_COMMAND_RESPONSE_ACTIVE_TEMP_ADDRESS));
            }
        }
    }

    if (ufm01->io.protocol == UFM01_PROTOCOL_ONE_WIRE)
    {
        result = RESULT_NOT_ALLOWED;
    }

    return result;
}

static inline Result Ufm01_InvokeGetSensorDataOneWire( ScioSense_Ufm01* ufm01 )
{
    Result result = RESULT_IO_ERROR;

    if (ufm01->io.protocol == UFM01_PROTOCOL_UART)
    {
        result = RESULT_INVALID;
    }

    if (ufm01->io.protocol == UFM01_PROTOCOL_ONE_WIRE)
    {
        static Ufm01_Command commandGetOneWireData = UFM01_COMMAND_READ_DATA_ONEWIRE;
        
        size_t responseLength = UFM01_BUFFER_ONEWIRE_DATA_LENGTH;
        uint8_t buff[UFM01_BUFFER_ONEWIRE_DATA_LENGTH];

        result = Ufm01_Invoke(ufm01, commandGetOneWireData, UFM01_COMMAND_READ_DATA_CODE_LENGTH, buff, responseLength);
        
        if (result == RESULT_OK)
        {
            result = Ufm01_CheckValueCrcOneWire(buff + UFM01_BUFFER_ACCUMULATED_FLOW_ONEWIRE_ADDRESS,   buff[UFM01_BUFFER_ACCUMULATED_FLOW_ONEWIRE_CRC_ADDRESS] );
            if( result != RESULT_OK )
            {
                return result;
            }
            result = Ufm01_CheckValueCrcOneWire(buff + UFM01_BUFFER_INSTANT_FLOW_ONEWIRE_ADDRESS,       buff[UFM01_BUFFER_INSTANT_FLOW_ONEWIRE_CRC_ADDRESS]     );
            if( result != RESULT_OK )
            {
                return result;
            }
            result = Ufm01_CheckValueCrcOneWire(buff + UFM01_BUFFER_TEMPERATURE_ONEWIRE_ADDRESS,        buff[UFM01_BUFFER_TEMPERATURE_ONEWIRE_CRC_ADDRESS]      );
            if( result != RESULT_OK )
            {
                return result;
            }

            ufm01->accumulatedFlowLiters =          Ufm01_AccumulatedFlowToLitersOneWire(       buff + UFM01_BUFFER_ACCUMULATED_FLOW_ONEWIRE_ADDRESS);
            ufm01->instantFlowDataLitersPerHour =   Ufm01_InstantFlowToLitersPerHourOneWire(    buff + UFM01_BUFFER_INSTANT_FLOW_ONEWIRE_ADDRESS);
            ufm01->temperatureDataDegC =            Ufm01_TemperatureToDegCOneWire(             buff + UFM01_BUFFER_TEMPERATURE_ONEWIRE_ADDRESS);
        }
    }
    return result;
}

static inline Result Ufm01_InvokeWriteConfiguration(ScioSense_Ufm01* ufm01, Ufm01_Frequency acquisitionFrequency, uint32_t startFlowlitersPerHour)
{
    Result result = RESULT_IO_ERROR;

    uint8_t startFlowDecilitresPerHourArray[UFM01_COMMAND_STARTUP_FLOW_RATE_LENGTH];
    Ufm01_EncodeDecimalNibbles(startFlowlitersPerHour, startFlowDecilitresPerHourArray, UFM01_COMMAND_STARTUP_FLOW_RATE_LENGTH);

    if (ufm01->io.protocol == UFM01_PROTOCOL_UART)
    {
        uint8_t commandConfigure[] = UFM01_COMMAND_SET_CONFIGURATION;
        memcpy( (commandConfigure + UFM01_COMMAND_A3_START_BYTE), startFlowDecilitresPerHourArray, UFM01_COMMAND_STARTUP_FLOW_RATE_LENGTH);
        commandConfigure[ UFM01_COMMAND_A2_START_BYTE       ]   = acquisitionFrequency;
        commandConfigure[ UFM01_COMMAND_CHECKSUM_START_BYTE ]   = Ufm01_CalculateChecksum_Uart(commandConfigure, UFM01_COMMAND_CONFIGURATION_START_BYTE, UFM01_COMMAND_CHECKSUM_START_BYTE);

        size_t responseLength = UFM01_COMMAND_RESPONSE_SET_CONFIGURATION_LENGTH;
        uint8_t buff[UFM01_COMMAND_RESPONSE_SET_CONFIGURATION_LENGTH];
        
        result = Ufm01_Invoke(ufm01, commandConfigure, UFM01_COMMAND_SET_CONFIGURATION_LENGTH, buff, responseLength);

        if (result == RESULT_OK)
        {
            result = Ufm01_CheckWriteConfigurationResponse(buff, responseLength, commandConfigure);
            
            if ( result == RESULT_OK )
            {
                ufm01->acquisitionFrequencyHz = (buff[UFM01_COMMAND_RESPONSE_CONFIGURE_A2_BYTE_ADDRESS]);
                
                ufm01->startupFlowRateDeciLitersPerHour =                                                       (uint32_t)(buff[    UFM01_COMMAND_RESPONSE_CONFIGURE_A3_BYTE_ADDRESS + 2]);
                ufm01->startupFlowRateDeciLitersPerHour = ( ufm01->startupFlowRateDeciLitersPerHour << 8 ) +    (uint32_t)(buff[    UFM01_COMMAND_RESPONSE_CONFIGURE_A3_BYTE_ADDRESS + 1]);
                ufm01->startupFlowRateDeciLitersPerHour = ( ufm01->startupFlowRateDeciLitersPerHour << 8 ) +    (uint32_t)(buff[    UFM01_COMMAND_RESPONSE_CONFIGURE_A3_BYTE_ADDRESS    ]);
            }
        }
    }

    if (ufm01->io.protocol == UFM01_PROTOCOL_ONE_WIRE)
    {
        result = RESULT_NOT_ALLOWED;
    }

    return result;
}

static inline Result Ufm01_WriteConfiguration(ScioSense_Ufm01* ufm01, Ufm01_Frequency acquisitionFrequency, float startFlowlitersPerHour)
{
    uint32_t startFlowDecilitresPerHour = (uint32_t)(startFlowlitersPerHour * 10.0f);

    Result result = Ufm01_InvokeWriteConfiguration(ufm01, acquisitionFrequency, startFlowDecilitresPerHour);

    if ( result == RESULT_OK )
    {
        ufm01->acquisitionFrequencyHz = acquisitionFrequency;
        ufm01->startupFlowRateDeciLitersPerHour = startFlowDecilitresPerHour;
    }

    return result;
}

static inline Result Ufm01_Reset(ScioSense_Ufm01* ufm01)
{
    Result result = RESULT_IO_ERROR;

    memset(ufm01->deviceIdData,         0,      UFM01_BUFFER_ID_LENGTH                  );
    memset(ufm01->deviceIdDataString,   '\0',   UFM01_BUFFER_ID_STRING_LENGTH + 1       );
    memset(ufm01->softwareVersionData,  0,      UFM01_BUFFER_SOFTWARE_VERSION_LENGTH    );
    memset(ufm01->statusData,           0,      UFM01_BUFFER_STATUS_LENGTH              );
    memset(ufm01->deviceOneWireData,    0,      UFM01_BUFFER_ONEWIRE_DATA_LENGTH        );

    ufm01->accumulatedFlowLiters            = 0;
    ufm01->instantFlowDataLitersPerHour     = 0;
    ufm01->temperatureDataDegC              = 0;
    ufm01->acquisitionFrequencyHz           = 1;
    ufm01->startupFlowRateDeciLitersPerHour = 15;
    ufm01->measurementMode                  = UFM01_MEASUREMENT_MODE_ACTIVE;

    clear();

    
    result = Ufm01_InvokeReset( ufm01 );
    
    if (result != RESULT_OK)
    {
        // retry in case the sensor is coming from a power up
        wait(800);
        clear();
        result = Ufm01_InvokeReset( ufm01 );
    }
    
    if ( result != RESULT_OK )
    {
        return result;
    }

    if ( ufm01->io.protocol == UFM01_PROTOCOL_UART )
    {
        wait(2000);
        result = Ufm01_Update( ufm01 );
        
        if ( result == RESULT_OK )
        {
            clear();
            result = Ufm01_InvokeGetSoftwareVersion( ufm01 );
        }
    }

    if ( ufm01->io.protocol == UFM01_PROTOCOL_ONE_WIRE )
    {
        ufm01->measurementMode = UFM01_MEASUREMENT_MODE_PASSIVE;
    }

    return result;
}

static inline Result Ufm01_Update(ScioSense_Ufm01* ufm01)
{
    Result result = RESULT_INVALID;

    if ( ufm01->io.protocol == UFM01_PROTOCOL_UART )
    {
        if ( ufm01->measurementMode == UFM01_MEASUREMENT_MODE_PASSIVE )
        {
            result = Ufm01_InvokeGetSensorDataNoId( ufm01 );
        }
    
        if ( ufm01->measurementMode == UFM01_MEASUREMENT_MODE_ACTIVE )
        {
            result = Ufm01_InvokeGetSensorDataActive( ufm01 );
        }
    }

    if ( ufm01->io.protocol == UFM01_PROTOCOL_ONE_WIRE )
    {
        result = Ufm01_InvokeGetSensorDataOneWire( ufm01 );
    }

    return result;
}

static inline Result Ufm01_SetOperatingMode(ScioSense_Ufm01* ufm01, const Ufm01_OperatingMode mode)
{
    Result result;

    if ( ufm01->io.protocol == UFM01_PROTOCOL_UART )
    {
        switch ( mode )
        {
            case UFM01_MEASUREMENT_MODE_PASSIVE : result = Ufm01_InvokePassiveMeasurement(  ufm01); break;
            case UFM01_MEASUREMENT_MODE_ACTIVE  : result = Ufm01_InvokeActiveMeasurement(   ufm01); break;
            default                             : result = RESULT_NOT_ALLOWED;                      break;
        }

        if ( result == RESULT_OK )
        {
            ufm01->measurementMode = mode;
        }
        clear();
    }

    if ( ufm01->io.protocol == UFM01_PROTOCOL_ONE_WIRE )
    {
        result = RESULT_INVALID;
    }

    return result;
}

static inline Ufm01_Frequency Ufm01_GetAcquisitionFrequency(ScioSense_Ufm01* ufm01)
{
    return ufm01->acquisitionFrequencyHz;
}

static inline float Ufm01_GetStartupFlowRate(ScioSense_Ufm01* ufm01)
{
    return ((float)ufm01->startupFlowRateDeciLitersPerHour) / 10.0f;
}

static inline bool Ufm01_IsConnected(ScioSense_Ufm01* ufm01)
{
    if ( ufm01->io.protocol == UFM01_PROTOCOL_UART )
    {
        for( uint8_t i=0; i<UFM01_BUFFER_ID_LENGTH; i++ )
        {
            if( ufm01->deviceIdData[i] != 0 )
            {
                return 1;
            }
        }
    }

    if ( ufm01->io.protocol == UFM01_PROTOCOL_ONE_WIRE )
    {
        uint8_t dummyData[1];
        return !Ufm01_Write(ufm01, 0, (uint8_t*)dummyData, 0);
    }

    return 0;
}

static inline Ufm01_ErrorCode Ufm01_GetStatus(ScioSense_Ufm01* ufm01)
{
    return Ufm01_GetValueOf16(ufm01->statusData, 0);
}

static inline uint64_t Ufm01_GetDeviceId(ScioSense_Ufm01* ufm01)
{
    return Ufm01_DecodeDecimalNibbles( ufm01->deviceIdData, UFM01_BUFFER_ID_LENGTH );
}

static inline char* Ufm01_GetDeviceIdString(ScioSense_Ufm01* ufm01)
{
    Ufm01_CreateDeviceIdString( ufm01 );
    return ufm01->deviceIdDataString;
}

static inline uint32_t Ufm01_GetManufacturingDate(ScioSense_Ufm01* ufm01)
{
    return (uint32_t)Ufm01_DecodeDecimalNibbles( ufm01->deviceIdData + UFM01_BUFFER_MANUFACTURING_DATE_ADDRESS, UFM01_BUFFER_MANUFACTURING_DATE_LENGTH );
}

static inline uint16_t Ufm01_GetSerialNumber(ScioSense_Ufm01* ufm01)
{
    return (uint16_t)Ufm01_DecodeDecimalNibbles( ufm01->deviceIdData + UFM01_BUFFER_SERIAL_NUMBER_ADDRESS, UFM01_BUFFER_SERIAL_NUMBER_LENGTH );
}

static inline Result Ufm01_ClearAccumulatedFlow(ScioSense_Ufm01* ufm01)
{
    return Ufm01_InvokeClearAccumulatedFlow(ufm01);
}

static inline uint32_t Ufm01_GetSoftwareVersion(ScioSense_Ufm01* ufm01)
{
    return (uint32_t)Ufm01_DecodeDecimalNibbles( ufm01->softwareVersionData, UFM01_BUFFER_SOFTWARE_VERSION_LENGTH );
}

static inline float Ufm01_GetAccumulatedFlowLiters(ScioSense_Ufm01* ufm01)
{
    return ufm01->accumulatedFlowLiters;
}

static inline float Ufm01_GetInstantFlowLitersPerHour( ScioSense_Ufm01* ufm01 )
{
    return ufm01->instantFlowDataLitersPerHour;
}

static inline float Ufm01_GetTemperatureDegC(ScioSense_Ufm01* ufm01)
{
    return ufm01->temperatureDataDegC;
}

static inline uint8_t Ufm01_CalculateChecksum_Uart( uint8_t *val, uint8_t startCrcByte, uint8_t stopCrcByte )
{
    uint8_t crc = 0x00;

    for( uint8_t i=startCrcByte; i<stopCrcByte; i++ )
    {
        crc += val[i];
    }
    return crc;
}

static inline float Ufm01_InstantFlowToLitersPerHourUart(uint8_t* instantFlowData, uint8_t instantFlowFlag)
{
    float valueInstantFlow = (float)Ufm01_DecodeDecimalNibbles( instantFlowData, UFM01_BUFFER_INSTANT_FLOW_VALUE_LENGTH );

    if ( instantFlowFlag & UFM01_INSTANT_FLOW_SIGN_MASK )
    {
        return -(valueInstantFlow / 100.0f);
    }

    return (valueInstantFlow / 100.0f);
}

static inline float Ufm01_AccumulatedFlowToLitersUart(uint8_t* accumulatedFlowData, uint8_t accumulatedFlowFlag)
{
    float accumulatedFlowMilliLiters = (float)Ufm01_DecodeDecimalNibbles( accumulatedFlowData, UFM01_BUFFER_ACCUMULATED_FLOW_LENGTH );
    switch ( accumulatedFlowFlag )
    {
    case UFM01_ACCUMULATED_FLOW_LITERS:
        return accumulatedFlowMilliLiters * 0.001f;
        break;

    case UFM01_ACCUMULATED_FLOW_M3:
        return accumulatedFlowMilliLiters;
        break;

    default:
        break;
    }
    return 0;
}

static inline float Ufm01_TemperatureToDegCUart(uint8_t* temperatureData)
{
    return (float)Ufm01_DecodeDecimalNibbles( temperatureData, UFM01_BUFFER_TEMPERATURE_LENGTH ) / 100.0f;
}

static inline float Ufm01_InstantFlowToLitersPerHourOneWire(uint8_t* instantFlowData)
{
    uint32_t instantFlowCentiLitersPerHour = 0;
    for ( int8_t i=(UFM01_BUFFER_MEASUREMENT_ONEWIRE_LENGTH-1); i>=0; i-- )
    {
        instantFlowCentiLitersPerHour = ( instantFlowCentiLitersPerHour << 8) + instantFlowData[i];
    }

    return (float)instantFlowCentiLitersPerHour * 0.01f;
}

static inline float Ufm01_AccumulatedFlowToLitersOneWire(uint8_t* accumulatedFlowData)
{
    uint32_t accumulatedFlowDeciLiters = 0;
    for ( int8_t i=(UFM01_BUFFER_MEASUREMENT_ONEWIRE_LENGTH-1); i>=0; i-- )
    {
        accumulatedFlowDeciLiters = ( accumulatedFlowDeciLiters << 8) + accumulatedFlowData[i];
    }

    return (float)accumulatedFlowDeciLiters * 0.1f;
}

static inline float Ufm01_TemperatureToDegCOneWire(uint8_t* temperatureData)
{
    uint32_t temperatureCentiDegC = 0;
    for ( int8_t i=(UFM01_BUFFER_MEASUREMENT_ONEWIRE_LENGTH-1); i>=0; i--)
    {
        temperatureCentiDegC = ( temperatureCentiDegC << 8) + temperatureData[i];
    }

    return (float)temperatureCentiDegC * 0.01f;
}

static inline uint8_t Ufm01_CalculateCrc8_OneWire(uint8_t *val, uint8_t length)
{
    uint8_t crc = 0xFF;
    uint8_t i,t;
    for( i=0; i<length; i++ )
    {
        crc ^= val[i];
        for( t=8; t>0; --t )
        {
            if( crc & 0x80 )
            {
                crc = (crc<<1) ^ 0x31u;
            }
            else
            {
                crc = (crc<<1);
            }
        }
    }
    return crc;
}

static inline Result Ufm01_CheckValueCrcOneWire( uint8_t* measurement, uint8_t measurementCrc)
{
    Result result = RESULT_CHECKSUM_ERROR;
    uint8_t crc = Ufm01_CalculateCrc8_OneWire(measurement, UFM01_BUFFER_MEASUREMENT_ONEWIRE_LENGTH);
    
    if( crc == measurementCrc )
    {
        result = RESULT_OK;
    }

    return result;
}

static inline Result Ufm01_CheckAcknowledgeResponse( uint8_t* data, const Ufm01_CommandResponse size )
{
    Result result = RESULT_INVALID;

    if( size != UFM01_COMMAND_RESPONSE_ACKNOWLEDGE_LENGTH )
    {
        return result;
    }

    if( data[UFM01_COMMAND_RESPONSE_ACKNOWLEDGE_BYTE_ADDRESS] == UFM01_COMMAND_RESPONSE_ACKNOWLEDGE ) 
    {
        result = RESULT_OK;
    }

    return result;
}

static inline Result Ufm01_CheckSensorDataWithIdResponse( uint8_t* data, const Ufm01_CommandResponse size )
{
    Result result = RESULT_INVALID;
    
    if( size != UFM01_COMMAND_RESPONSE_PASSIVE_MODE_WITH_ID_LENGTH )
    {
        return result;
    }

    if(     (data[UFM01_COMMAND_RESPONSE_START_BYTE_ADDRESS_1           ]   != UFM01_COMMAND_RESPONSE_DATA_OUT_START_BYTE_1                 )
        ||  (data[UFM01_COMMAND_RESPONSE_START_BYTE_ADDRESS_2           ]   != UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_DATA_OUT_START_BYTE_2 )
        ||  (data[UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_STOP_ADDRESS   ]   != UFM01_COMMAND_RESPONSE_STOP_BYTE                             ) )
    {
        return result;
    }

    if( data[UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_CHECKSUM_ADDRESS] == Ufm01_CalculateChecksum_Uart( data, 0, UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_CHECKSUM_ADDRESS ) )
    {
        result = RESULT_OK;
    }
    else
    {
        result = RESULT_CHECKSUM_ERROR;
    }
    
    return result;
}

static inline Result Ufm01_CheckSensorDataNoIdResponse( uint8_t* data, const Ufm01_CommandResponse size )
{
    Result result = RESULT_INVALID;
    
    if( size != UFM01_COMMAND_RESPONSE_PASSIVE_MODE_NO_ID_LENGTH )
    {
        return result;
    }

    if(     (data[UFM01_COMMAND_RESPONSE_START_BYTE_ADDRESS_1       ]   != UFM01_COMMAND_RESPONSE_DATA_OUT_START_BYTE_1                 )
        ||  (data[UFM01_COMMAND_RESPONSE_START_BYTE_ADDRESS_2       ]   != UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_DATA_OUT_START_BYTE_2   )
        ||  (data[UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_STOP_ADDRESS ]   != UFM01_COMMAND_RESPONSE_STOP_BYTE                             ) )
    {
        return result;
    }

    if( data[UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_CHECKSUM_ADDRESS] == Ufm01_CalculateChecksum_Uart( data, 0, UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_CHECKSUM_ADDRESS ) )
    {
        result = RESULT_OK;
    }
    else
    {
        result = RESULT_CHECKSUM_ERROR;
    }
    
    return result;
}

static inline Result Ufm01_CheckSensorDataActiveResponse( uint8_t* data, const Ufm01_CommandResponse size )
{
    Result result = RESULT_INVALID;
    
    if( size != UFM01_COMMAND_RESPONSE_ACTIVE_MODE_LENGTH )
    {
        return result;
    }

    if(     (data[UFM01_COMMAND_RESPONSE_START_BYTE_ADDRESS_1   ]   != UFM01_COMMAND_RESPONSE_DATA_OUT_START_BYTE_1         )
        ||  (data[UFM01_COMMAND_RESPONSE_START_BYTE_ADDRESS_2   ]   != UFM01_COMMAND_RESPONSE_DATA_OUT_ACTIVE_START_BYTE_2  )
        ||  (data[UFM01_COMMAND_RESPONSE_ACTIVE_STOP_ADDRESS    ]   != UFM01_COMMAND_RESPONSE_STOP_BYTE                     ) )
    {
        return result;
    }

    if(data[UFM01_COMMAND_RESPONSE_ACTIVE_CHECKSUM_ADDRESS] == Ufm01_CalculateChecksum_Uart( data, 0, UFM01_COMMAND_RESPONSE_ACTIVE_CHECKSUM_ADDRESS ))
    {
        result = RESULT_OK;
    }
    else
    {
        result = RESULT_CHECKSUM_ERROR;
    }
    
    return result;
}

static inline Result Ufm01_CheckWriteConfigurationResponse( uint8_t* data, const Ufm01_CommandResponse size , Ufm01_Command configurationCommand)
{
    Result result = RESULT_INVALID;
    
    if( size != UFM01_COMMAND_RESPONSE_SET_CONFIGURATION_LENGTH )
    {
        return result;
    }

    if(     (data[UFM01_COMMAND_RESPONSE_ACKNOWLEDGE_BYTE_ADDRESS       ]   != UFM01_COMMAND_RESPONSE_ACKNOWLEDGE   )
        ||  (data[UFM01_COMMAND_RESPONSE_CONFIGURE_A0_FLAG_BYTE_ADDRESS ]   != UFM01_COMMAND_RESPONSE_FLAG_A0       )
        ||  (data[UFM01_COMMAND_RESPONSE_CONFIGURE_A1_FLAG_BYTE_ADDRESS ]   != UFM01_COMMAND_RESPONSE_FLAG_A1       )
        ||  (data[UFM01_COMMAND_RESPONSE_CONFIGURE_A2_FLAG_BYTE_ADDRESS ]   != UFM01_COMMAND_RESPONSE_FLAG_A2       )
        ||  (data[UFM01_COMMAND_RESPONSE_CONFIGURE_A3_FLAG_BYTE_ADDRESS ]   != UFM01_COMMAND_RESPONSE_FLAG_A3       )
        ||  (data[UFM01_COMMAND_RESPONSE_CONFIGURE_A4_FLAG_BYTE_ADDRESS ]   != UFM01_COMMAND_RESPONSE_FLAG_A4       )
        ||  (data[UFM01_COMMAND_RESPONSE_CONFIGURE_STOP_BYTE_ADDRESS    ]   != UFM01_COMMAND_RESPONSE_STOP_BYTE     ) )
    {
        return result;
    }

    if( data[UFM01_COMMAND_RESPONSE_CONFIGURE_CHECKSUM_BYTE_ADDRESS] != Ufm01_CalculateChecksum_Uart( data, UFM01_COMMAND_RESPONSE_CONFIGURE_A0_FLAG_BYTE_ADDRESS, UFM01_COMMAND_RESPONSE_CONFIGURE_CHECKSUM_BYTE_ADDRESS) )
    {
        result = RESULT_CHECKSUM_ERROR;
    }

    if(     (Ufm01_CompareArrays( data + UFM01_COMMAND_RESPONSE_CONFIGURE_A0_BYTE_ADDRESS,  configurationCommand + UFM01_COMMAND_A0_START_BYTE, UFM01_COMMAND_A0_LENGTH ) )
        &&  (Ufm01_CompareArrays( data + UFM01_COMMAND_RESPONSE_CONFIGURE_A1_BYTE_ADDRESS,  configurationCommand + UFM01_COMMAND_A1_START_BYTE, UFM01_COMMAND_A1_LENGTH ) )
        &&  (Ufm01_CompareArrays( data + UFM01_COMMAND_RESPONSE_CONFIGURE_A2_BYTE_ADDRESS,  configurationCommand + UFM01_COMMAND_A2_START_BYTE, UFM01_COMMAND_A2_LENGTH ) )
        &&  (Ufm01_CompareArrays( data + UFM01_COMMAND_RESPONSE_CONFIGURE_A3_BYTE_ADDRESS,  configurationCommand + UFM01_COMMAND_A3_START_BYTE, UFM01_COMMAND_A3_LENGTH ) )
        &&  (Ufm01_CompareArrays( data + UFM01_COMMAND_RESPONSE_CONFIGURE_A4_BYTE_ADDRESS,  configurationCommand + UFM01_COMMAND_A4_START_BYTE, UFM01_COMMAND_A4_LENGTH ) )
        &&  ( data[UFM01_COMMAND_RESPONSE_CONFIGURE_A0_FLAG_BYTE_ADDRESS] == UFM01_COMMAND_RESPONSE_FLAG_A0 )
        &&  ( data[UFM01_COMMAND_RESPONSE_CONFIGURE_A1_FLAG_BYTE_ADDRESS] == UFM01_COMMAND_RESPONSE_FLAG_A1 )
        &&  ( data[UFM01_COMMAND_RESPONSE_CONFIGURE_A2_FLAG_BYTE_ADDRESS] == UFM01_COMMAND_RESPONSE_FLAG_A2 )
        &&  ( data[UFM01_COMMAND_RESPONSE_CONFIGURE_A3_FLAG_BYTE_ADDRESS] == UFM01_COMMAND_RESPONSE_FLAG_A3 )
        &&  ( data[UFM01_COMMAND_RESPONSE_CONFIGURE_A4_FLAG_BYTE_ADDRESS] == UFM01_COMMAND_RESPONSE_FLAG_A4 ) )
    {
        result = RESULT_OK;
    }
    
    return result;
}

static inline Result Ufm01_CheckReadSoftwareVersionResponse( uint8_t* data, const Ufm01_CommandResponse size )
{
    Result result = RESULT_INVALID;
    
    if( size != UFM01_COMMAND_RESPONSE_GET_SOFTWARE_VERSION_LENGTH )
    {
        return result;
    }

    if(     (data[UFM01_COMMAND_RESPONSE_START_BYTE_ADDRESS_1       ]   != UFM01_COMMAND_RESPONSE_ACKNOWLEDGE   )
        ||  (data[UFM01_COMMAND_RESPONSE_SOFTWARE_STOP_BYTE_ADDRESS ]   != UFM01_COMMAND_RESPONSE_STOP_BYTE     ) )
    {
        return result;
    }

    if( data[UFM01_COMMAND_RESPONSE_SOFTWARE_CHECKSUM_BYTE_ADDRESS] == Ufm01_CalculateChecksum_Uart( data, UFM01_COMMAND_RESPONSE_SOFTWARE_VERSION_START_BYTE_ADDRESS, UFM01_COMMAND_RESPONSE_SOFTWARE_CHECKSUM_BYTE_ADDRESS ) )
    {
        result = RESULT_OK;
    }
    else
    {
        result = RESULT_CHECKSUM_ERROR;
    }
    
    return result;
}

#undef wait
#undef clear
#undef hasAnyFlag
#undef hasFlag
#undef memset
#undef memcpy

#endif // SCIOSENSE_UFM01_C_INL