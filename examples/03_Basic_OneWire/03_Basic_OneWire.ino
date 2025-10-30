/* ******************************************************
*
*   Example Code for running ScioSense UFM01 on One-Wire
*           tested with Arduino MEGA and ESP32
*
*  ******************************************************
*/

#include <OneWire.h>

#include <ScioSense_UFM01.h>

#define oneWirePin      4

UFM01 ufm01;
OneWire oneWire(oneWirePin);

void setup(void)
{
    Serial.begin(9600);
    Serial.println("");
    Serial.println("Starting UFM01 example 03_Basic_OneWire");

    ufm01.begin(&oneWire);

    delay(1200);
    
    while (ufm01.init() == false)
    {
        Serial.println("Error -- The UFM01 is not connected.");
        delay(1000);
    }
    
    while (ufm01.clearAccumulatedFlow() == false)
    {
        Serial.println("Trying to clear the accumulated flow");
        delay(1000);
    }
}

void loop(void)
{
    if (ufm01.update() == RESULT_OK)
    {
        Serial.print("AccF[l]:");
        Serial.print(ufm01.getAccumulatedFlowLiters());

        Serial.print("\tInstF[l/hr]:");
        Serial.print(ufm01.getInstantFlowLitersPerHour());

        Serial.print("\tT[C]:");
        Serial.println(ufm01.getTempCelsius());
    }
    delay(1000);
}
