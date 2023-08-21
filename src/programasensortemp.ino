#include <UbiConstants.h>
#include <UbidotsEsp32Mqtt.h>
#include <UbiTypes.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "DHT.h"
#define DHTPIN 27 //pin 27 lee 
#define DHTYPE DHT11 

TFT_eSPI tft = TFT_eSPI(); 
DHT dht(DHTPIN, DHTYPE);

const char *UBIDOTS_TOKEN = "BBFF-a9TZcQUiyg19IVwSRkiTpqsXTwOWbq";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "UPBWiFi";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "esp32";   // Put here your Device label to which data  will be published
const char *HUMEDAD_VARIABLE_LABEL = "Humedad"; //  humedad
const char *TEMPERATURA_VARIABLE_LABEL = "Temperatura"; //  temperatura
const int PUBLISH_FREQUENCY = 5000; // Update rate in milliseconds
unsigned long timer;
uint8_t analogPin = 27; // Pin used to read data from GPIO34 ADC_CH6.
Ubidots ubidots(UBIDOTS_TOKEN);


//definicion 
// callback ubidots 
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

//setup 
void setup(){  
  Serial.begin(115200);
  // ubidots.setDebug(true);  // uncomment this to make debug messages available
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  Serial.println(F("DHTxx test!"));
  dht.begin();
  timer = millis();

  float Humedad = dht.readHumidity(); 
  float Temperatura = dht.readTemperature(); 
  tft.init(); 
  tft.fillScreen(TFT_BLACK); //mirar el color 
  tft.drawString("Humedad", 10, 10, 2);
  tft.drawString("Temperatura", 10, 60, 2);
   if (isnan(Humedad)|| isnan(Temperatura)){
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
}



void loop(){

    if (!ubidots.connected())
  {
    ubidots.reconnect();
  }
  float Humedad = dht.readHumidity(); 
  float Temperatura = dht.readTemperature(); 

  if ((millis() - timer) > PUBLISH_FREQUENCY) 
  {
    Serial.print("Humedad: ");
    Serial.print(Humedad);
    Serial.print("Temperatura: ");
    Serial.print(Temperatura);
    tft.drawString(String(Humedad), 10, 30, 2);
    tft.drawString(String(Temperatura), 10, 80, 2);
    ubidots.add(HUMEDAD_VARIABLE_LABEL,Humedad); 
    ubidots.add(TEMPERATURA_VARIABLE_LABEL,Temperatura); 
    ubidots.publish(DEVICE_LABEL);
    timer = millis();      
  }
  ubidots.loop();
}

  
