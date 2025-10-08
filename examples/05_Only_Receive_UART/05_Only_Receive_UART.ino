/* **************************************************
*
*   Example Code for running ScioSense UFM01 on UART
*       tested with Arduino MEGA and ESP32
*
*  **************************************************
*/

#include <Arduino.h>

#include <ScioSense_UFM01.h>

#ifdef ESP32
    #define esp32RxPin      16
    #define esp32TxPin      17
#endif
#define uartBaudrateUfm01   2400
#define uartConfigUfm01     SERIAL_8E1

UFM01 ufm01;

void PrintUfm01Error(uint16_t error);

void setup()
{
    Serial.begin(9600);
    Serial.println("");
    Serial.println("Starting UFM01 example 05_Only_Receive_UART");

    #ifdef ESP32
      Serial2.begin(uartBaudrateUfm01, uartConfigUfm01, esp32RxPin, esp32TxPin);
    #endif
    #ifdef __AVR_ATmega2560__
        Serial2.begin(uartBaudrateUfm01, uartConfigUfm01);
    #endif
    ufm01.begin(&Serial2);
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