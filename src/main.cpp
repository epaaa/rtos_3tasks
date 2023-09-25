#include <Arduino.h>
/*import libraries*/
#include <Ticker.h>
#include "DHTesp.h"
#include <Wire.h>
#include "BH1750.h"
#include "device.h"

DHTesp dht; //define the dht
BH1750 lightMeter; //define the lux meter

/*declare the tasks*/
void taskTemperature(void *arg); 
void taskHumidity(void *arg);
void taskLight(void *arg);


void setup() {
  Serial.begin(115200);
  delay(100);

  /*setup the pinouts*/
  pinMode(LED_BUILTIN, OUTPUT);
  #if defined(LED_FLASH)
    pinMode(LED_FLASH, OUTPUT);
  #endif
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(PIN_SW, INPUT);
  
  xTaskCreatePinnedToCore(taskTemperature, "Temperature", 2048, NULL, 1, NULL, 0); //pin a specific task to a specific core
  xTaskCreatePinnedToCore(taskHumidity, "Humidity", 2048, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(taskLight, "Light", 2048, NULL, 3, NULL, 1);
  
  /*setup the dht*/
  dht.setup(PIN_DHT, DHTesp::DHT11); 
  Serial.printf("Board: %s\n", ARDUINO_BOARD);
  Serial.printf("DHT Sensor ready, sampling period: %d ms\n", dht.getMinimumSamplingPeriod());

  /*setup the lux meter*/
  Wire.begin(PIN_SDA, PIN_SCL); 
  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23, &Wire);

  
}

void loop() {
  vTaskDelay(0); //inform the void loop to do nothing
}

void taskTemperature(void *arg) {
  for (;;)
  {
    TickType_t nTickStart = xTaskGetTickCount(); //store the count of starting tick
    vTaskDelay(8000 / portTICK_PERIOD_MS); //to give delay
    TickType_t nElapaseTick = xTaskGetTickCount() - nTickStart; //to store the time interval
    float temperature = dht.getTemperature();
    if (dht.getStatus() == DHTesp::ERROR_NONE) //check if there's an error
    {
    Serial.printf("[%u] Temperature: %.2f C\n", nElapaseTick, temperature); //print the temperature to terminal
    }
  }
}

void taskHumidity(void *arg)
{
  for (;;)
  {
    TickType_t nTickStart = xTaskGetTickCount();//store the count of starting tick
    delay(8000);
    TickType_t nElapaseTick = xTaskGetTickCount() - nTickStart; //to store the time interval
    float humidity = dht.getHumidity();
    if (dht.getStatus() == DHTesp::ERROR_NONE) //check if there's an error
    {
    Serial.printf("[%u] Humidity: %.2f RH\n", nElapaseTick, humidity); //print the humidity to terminal
    }
  }
}

void taskLight(void *arg)
{
  for (;;)
  {
    TickType_t nTickStart = xTaskGetTickCount();//store the count of starting tick
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    float lux = lightMeter.readLightLevel();
    TickType_t nElapaseTick = xTaskGetTickCount() - nTickStart; //to store the time interval
    Serial.printf("[%u] Light: %.2f lux\n", nElapaseTick, lux); //print the lux to terminal
  }
}



