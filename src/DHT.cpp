#ifdef SENSORS
#include "DHT.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <WiFiSettings.h>
#include <AsyncMqttClient.h>
#include "string_utils.h"

#include <DHTesp.h>
#include <Ticker.h>

namespace DHT
{
    uint8_t dht11Pin;
    uint8_t dht22Pin;
    float dhtTempOffset;

    /** Initialize DHT sensor 1 */
    DHTesp dhtSensor;

    /** Task handle for the light value read task */
    TaskHandle_t dhtTempTaskHandle = NULL;

    /** Ticker for temperature reading */
    Ticker tempTicker;

    /** Flags for temperature readings finished */
    bool gotNewTemperature = false;

    /** Data from dht sensor 1 */
    TempAndHumidity dhtSensorData;

    /* Flag if main loop is running */
    bool dhtTasksEnabled = false;

    /* update time */
    int dhtUpdateTime = 10; //ToDo: maybe make this a user choise via settings menu

    /**
     * Task to reads temperature from DHT11 sensor
     * @param pvParameters
     *		pointer to task parameters
     */
    void tempTask(void *pvParameters)
    {
        Serial.println("tempTask loop started");
        while (1) // tempTask loop
        {
            if (dhtTasksEnabled && !gotNewTemperature)
            {
                // Read temperature only if old data was processed already
                // Reading temperature for humidity takes about 250 milliseconds!
                // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
                dhtSensorData = dhtSensor.getTempAndHumidity(); // Read values from sensor 1
                gotNewTemperature = true;
            }
            vTaskSuspend(NULL);
        }
    }

    /**
     * triggerGetTemp
     * Sets flag dhtUpdated to true for handling in loop()
     * called by Ticker tempTicker
     */
    void triggerGetTemp()
    {
        if (dhtTempTaskHandle != NULL)
        {
            xTaskResumeFromISR(dhtTempTaskHandle);
        }
    }

    void Setup()
    {
        if (dht11Pin) dhtSensor.setup(dht11Pin, DHTesp::DHT11);
        if (dht22Pin) dhtSensor.setup(dht22Pin, DHTesp::DHT22); //(AM2302)

        if (dht11Pin || dht22Pin)
        {
            // Start task to get temperature
            xTaskCreatePinnedToCore(
                    tempTask,           /* Function to implement the task */
                    "tempTask ",        /* Name of the task */
                    4000,               /* Stack size in words */
                    NULL,               /* Task input parameter */
                    5,                  /* Priority of the task */
                    &dhtTempTaskHandle, /* Task handle. */
                    1);                 /* Core where the task should run */

            if (dhtTempTaskHandle == NULL)
            {
                Serial.println("[ERROR] Failed to start task for temperature update");
            }
            else
            {
                // Start update of environment data every 10 seconds
                tempTicker.attach(dhtUpdateTime, triggerGetTemp);
            }

            // Signal end of setup() to tasks
            dhtTasksEnabled = true;
        }
    }

    void ConnectToWifi()
    {
        dht11Pin = WiFiSettings.integer("dht11_pin", 0, "DHT11 sensor pin (0 for disable)");
        dht22Pin = WiFiSettings.integer("dht22_pin", 0, "DHT22 sensor pin (0 for disable)");
        dhtTempOffset = WiFiSettings.floating("dhtTemp_offset", -40, 125, 0.0, "DHT temperature offset");
    }

    void SerialReport()
    {
        Serial.print("DHT11 Sensor: ");
        Serial.println(dht11Pin ? "enabled" : "disabled");
        Serial.print("DHT22 Sensor: ");
        Serial.println(dht22Pin ? "enabled" : "disabled");
        Serial.print("DHT Temp Offset: ");
        Serial.println(dhtTempOffset ? "enabled" : "disabled");
    }

    void Loop()
    {
        if (!dht11Pin && !dht22Pin) return;

        if (gotNewTemperature)
        {
            float humidity = dhtSensorData.humidity;
            float temperature = dhtSensorData.temperature + dhtTempOffset;
            Serial.println("Temp: " + String(temperature, 1) + "'C Humidity: " + String(humidity, 1) + "%");

            mqttClient.publish((roomsTopic + "/humidity").c_str(), 0, 1, String(humidity, 1).c_str());
            mqttClient.publish((roomsTopic + "/temperature").c_str(), 0, 1, String(temperature, 1).c_str());

            gotNewTemperature = false;
        }
    }

    bool SendDiscovery()
    {
        if (!dht11Pin && !dht22Pin) return true;

        return sendSensorDiscovery("Temperature", "", "temperature", "°C") && sendSensorDiscovery("Humidity", "", "humidity", "%");
    }

    bool Command(String& command, String& pay)
    {
        return false;
    }

    bool SendOnline()
    {
        return true;
    }
}
#endif
