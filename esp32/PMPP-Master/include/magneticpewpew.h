#ifndef magneticpewpew
#define magneticpewpew

#include <Arduino.h>
#include <string.h>
#include <Wire.h>
#include <WireSlaveRequest.h>
#include <WiFi.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include <SocketIOclient.h>
#include <Hash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)

#define SDA_PIN 21                   // serial data pin for I2C-connection
#define SCL_PIN 22                   // serial clock pin for I2C-connection
#define MAX_SLAVE_RESPONSE_LENGTH 32 // max number of bytes the slave will send
#define MAX_SLAVES_ON_BUS 10         // absolute maximum value is 12

#define ADDR_PIN 4 // Pin where the Address-Signal is sent from

#define LOAD_BUTTON 12     // Pin where the LOAD-button is located
#define TRIGGER_BUTTON 14 // Pin where the TRIGGER-button is located
#define TRIGGER_PIN 0     // Pin where the trigger-signal is sent from
#define SOLINOID_PIN 33 // Pin from which the Solinoid is triggered

//* WiFi
WiFiClient client;
HttpClient http = HttpClient(client, "pmpp.info", 3000);
const char *ssid[3] = {"Jakob", "iPhone von Jakob","Jonathlan"};
const char *password[3] = {"JakobWLAN123", "godsavethequeen","Iamtheoneandonlyadmin"};

/**
 * Scans for WiFi networks.
 *
 * @returns The index of the WiFi network in the list of found networks.
 * If no WiFi networks are found, returns -1.
 */
int scanForWiFi()
{
  int n = WiFi.scanNetworks();

  if (n)
  {
    Serial.println("List of WiFi-networks:");
    for (int i = 0; i < n; i++)
    {
      Serial.println(WiFi.SSID(i));
    }
    for (int i = 0; i < n; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        if (WiFi.SSID(i) == ssid[j])
        {
          return j;
        }
      }
    }
  }
  else
  {
    Serial.println("No WiFi-networks found");
  }
  return -1;
}

/**
 * Connects to the WiFi network.
 *
 * @returns None
 */
void connectToWiFi() // connects the ESP to a WiFi
{
  int x = scanForWiFi();
  if (x == -1)
    while (1)
    {
    }

  WiFi.begin(ssid[x], password[x]);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("connected to WiFi " + WiFi.SSID());
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//* I2C
/**
 * Initializes the OLED display.
 *
 * @returns None
 */
TwoWire oled_Wire = TwoWire(1);                                             // creates new TwoWire-object on Interface 2
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &oled_Wire, OLED_RESET); // initializes the oled-display

/**
 * Sends a signal to the slaves to give them their addres.
 *
 * @returns None
 */
void addrSlaves()
{
  pinMode(ADDR_PIN, OUTPUT);
  digitalWrite(ADDR_PIN, LOW);
  delay(2000);
  digitalWrite(ADDR_PIN, HIGH);
  delayMicroseconds(150);
  digitalWrite(ADDR_PIN, LOW);
  delayMicroseconds(150);
  digitalWrite(ADDR_PIN, HIGH);
  delayMicroseconds(150);
  digitalWrite(ADDR_PIN, LOW);
  delayMicroseconds(2100);
  digitalWrite(ADDR_PIN, HIGH);
  delayMicroseconds(150);
  digitalWrite(ADDR_PIN, LOW);


//Lichtschranke Aktivieren
  pinMode(ADDR_PIN, INPUT_PULLDOWN);
   pinMode(26, OUTPUT);
  digitalWrite(26, HIGH);
}

//* Classes
class Stage // class which contains data of one stage
{
public:
  // bool onBus; equal to n_stage so not used
  bool onLoad;       // indicates if the stage is loaded
  uint32_t msData;   // measurement data in us
  byte lsSelect;     // selects the previous(1) or the own (0) lightswitch
  uint32_t dtSelect; // selects the delay time in us (maximum: 2^32)
  Stage()            // Construcor sets default values
  {
    onLoad = 0;
    msData = 0;
    lsSelect = 0;
    dtSelect = 0;
  }
};

class MPP // class which contains data of all stages
{
public:
  int n_stage;  // number of of stages(=I2C-SLaves)
  Stage *stage; // pointer to an array of Stage-objects
  MPP()         // Construcor sets default values
  {
    n_stage = 0;
  }
  void MPP_init() //! call only if n_stage is set!
  // initializes the size of the array of Stage-objects
  {
    stage = new Stage[n_stage];
  }
};

//* Other functions

/**
 * Prints the contents of a memory region in hexadecimal.
 *
 * @param mem The memory region to print.
 * @param len The length of the memory region.
 * @param cols The number of bytes to print per line.
 *
 * @returns None
 */
void hexdump(const void *mem, uint32_t len, uint8_t cols = 16)
{
  const uint8_t *src = (const uint8_t *)mem;
  Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
  for (uint32_t i = 0; i < len; i++)
  {
    if (i % cols == 0)
    {
      Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
    }
    Serial.printf("%02X ", *src);
    src++;
  }
  Serial.printf("\n");
}

#endif