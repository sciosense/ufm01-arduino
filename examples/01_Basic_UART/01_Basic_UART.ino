/* **************************************************
*
*   Example Code for running ScioSense UFM01 on UART
*       tested with Arduino UNO and ESP32
*
*  **************************************************
*/

#include <Arduino.h>

#include <ScioSense_UFM01.h>

#define rxPin               16
#define txPin               17
#define uartBaudrateUfm01   2400
#define uartConfigUfm01     SERIAL_8E1

UFM01 ufm01;

void PrintUfm01Error(uint16_t error);

void setup()
{
    Serial.begin(9600);
    Serial.println("");
    Serial.println("Starting UFM01 example 01_Basic_UART");

    Serial2.begin(uartBaudrateUfm01, uartConfigUfm01, rxPin, txPin);
    ufm01.begin(&Serial2);

    while (ufm01.init() == false)
    {
        Serial.println("Error -- The UFM01 is not connected.");
        delay(1000);
    }

    Serial.print("Device ID: ");
    Serial.println(ufm01.getDeviceIdString());
    Serial.print("Software Version: ");
    Serial.println(ufm01.getSoftwareVersion());
    ufm01.clearAccumulatedFlow();
}

void loop()
{
    if (ufm01.update() == RESULT_OK)
    {
        uint16_t error = ufm01.getError();
        PrintUfm01Error(error);
        
        if( !error )
        {
            Serial.print("AccF[l]:");
            Serial.print(ufm01.getAccumulatedFlowLiters());
    
            Serial.print("\tInstF[l/hr]:");
            Serial.print(ufm01.getInstantFlowLitersPerHour());
    
            Serial.print("\tT[C]:");
            Serial.println(ufm01.getTempCelsius());
        }
    }

    delay(1000);
}

void PrintUfm01Error(uint16_t error)
{
    if( error & UFM01_ERROR_CODE_EMPTY_TUBE)
    {
        Serial.println("Error: No water detected in the tube");
    }
    if( error & UFM01_ERROR_CODE_UFC_CHIP_ERROR)
    {
        Serial.println("Error: UFC not detected or abnormal");
    }
    if( error & UFM01_ERROR_CODE_FLOW_DIRECTION_WRONG)
    {
        Serial.println("Error: Negative flow detected");
    }
    if( error & UFM01_ERROR_CODE_FLOW_RATE_OUT_OF_RANGE)
    {
        Serial.println("Error: Big flow rate out of range continuously detected for 5s");
    }
}