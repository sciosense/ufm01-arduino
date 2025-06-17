#include "ufm01.h"


UFM01::~UFM01() { }

UFM01::UFM01()
{
    io                                  = { 0 };

    deviceIdData[0]                     = 0;
    softwareVersionData[0]              = 0;
    statusData[0]                       = 0;

    accumulatedFlowLiters               = 0;
    instantFlowDataLitersPerHour        = 0;
    temperatureDataDegC                 = 0;
    startupFlowRateDeciLitersPerHour    = 0;
    acquisitionFrequencyHz              = 0;
    measurementMode                     = UFM01_MEASUREMENT_MODE_ACTIVE;
}

void UFM01::begin(Stream* serial)
{
    serialConfig        = { 0 };
    serialConfig.serial = serial;

    io.read             = ScioSense_Arduino_Serial_Read;
    io.write            = ScioSense_Arduino_Serial_Write;
    io.wait             = ScioSense_Arduino_Serial_Wait;
    io.clear            = ScioSense_Arduino_Serial_Clear;
    io.protocol         = UFM01_PROTOCOL_UART;
    io.config           = &serialConfig;
}

void UFM01::begin(OneWire* oneWire)
{
    oneWireConfig           = { 0 };
    oneWireConfig.oneWire   = oneWire;

    io.read                 = ScioSense_Arduino_OneWire_Read;
    io.write                = ScioSense_Arduino_OneWire_Write;
    io.wait                 = ScioSense_Arduino_OneWire_Wait;
    io.protocol             = UFM01_PROTOCOL_ONE_WIRE;
    io.config               = &oneWireConfig;

    //there is no active mode when using the one wire io interface
    measurementMode         = UFM01_MEASUREMENT_MODE_PASSIVE;
}

inline bool UFM01::init()
{
    return Ufm01_Reset(this) == RESULT_OK;
}

void UFM01::clear()
{
    this->io.clear(this->io.config);
}

void UFM01::reset()
{
    Ufm01_Reset(this);
}

bool UFM01::isConnected()
{
    return Ufm01_IsConnected(this);
}

Result UFM01::update()
{
    return Ufm01_Update(this);
}

bool UFM01::setOperatingMode(const Ufm01_OperatingMode mode)
{
    return Ufm01_SetOperatingMode(this, mode) == RESULT_OK;
}

Result UFM01::writeConfiguration(Ufm01_Frequency ufm01FrequencyHz, float startupFlowRateLitresPerHour)
{
    return Ufm01_WriteConfiguration(this, ufm01FrequencyHz, startupFlowRateLitresPerHour);
}

float UFM01::getAccumulatedFlowLiters()
{
    return Ufm01_GetAccumulatedFlowLiters(this);
}

float UFM01::getAccumulatedFlowM3()
{
    return Ufm01_GetAccumulatedFlowLiters(this) / 1000.0f;
}

float UFM01::getInstantFlowLitersPerHour()
{
    return Ufm01_GetInstantFlowLitersPerHour(this);
}

Result UFM01::clearAccumulatedFlow()
{
    return Ufm01_ClearAccumulatedFlow(this);
}

float UFM01::getTempKelvin()
{
    return Ufm01_GetTemperatureDegC(this) + 273.15f;
}

float UFM01::getTempCelsius()
{
    return Ufm01_GetTemperatureDegC(this);
}

float UFM01::getTempFahrenheit()
{
    return (1.8f * Ufm01_GetTemperatureDegC(this)) + 32.f;
}

uint32_t UFM01::getSoftwareVersion()
{
    return Ufm01_GetSoftwareVersion(this);
}

uint64_t UFM01::getDeviceId()
{
    return Ufm01_GetDeviceId(this);
}

char* UFM01::getDeviceIdString()
{
    return Ufm01_GetDeviceIdString(this);
}

uint32_t UFM01::getManufacturingDate()
{
    return Ufm01_GetManufacturingDate(this);
}

uint16_t UFM01::getSerialNumber()
{
    return Ufm01_GetSerialNumber(this);
}

Ufm01_Frequency UFM01::getAcquisitionFrequency()
{
    return Ufm01_GetAcquisitionFrequency(this);
}

float UFM01::getStartupFlowRateLitersPerHour()
{
    return Ufm01_GetStartupFlowRate(this);
}

Ufm01_ErrorCode UFM01::getError()
{
    return Ufm01_GetStatus(this);
}