#include <magneticpewpew.h>

#define STATUS_UPDATE_TIME 1000

// Variables used for Button-Debouncing
volatile bool lock_load = 1;
volatile bool lock_trigger = 1;

// flags
volatile bool updateSettings = 1; // set when settings shall be updated via I2C
volatile bool LOAD_flag;          // set from ISR if the LOAD-button is pressed
volatile bool TRIGGER_flag;       // set from ISR if the TRIGGER-button is pressed
volatile bool ioEvent_flag = 0;
static bool allOnLoad = 0;         // set when all stages are on load
static bool allMsDataReceived = 0; // set when all measurement-data is received

// variables
long sendstatus;
String ioEvent_payload;

MPP mpp; // A MPP(magnetic-pew-pew) object is created

// Socket.IO
SocketIOclient socketIO;
char host[] = "www.pmpp.info"; // Socket.IO Server Address
int port = 3000;
char path[] = "/socket.io/?EIO=4"; // Socket.IO Port Address

// Prototypes
String getmsData(void); // provides json-file with data for get-request
String getStatus(void); // provides json-file with data for get-request
void socketIOEvent(socketIOmessageType_t type, uint8_t *payload, size_t length);
void ISR_LOAD(void);
void ISR_TRIGGER(void);

void setup()
{
  // put your setup code here, to run once:
  addrSlaves();

  Serial.begin(115200); // start serial for output
  Serial.println("");
  Wire.begin(SDA_PIN, SCL_PIN); // join i2c bus

  connectToWiFi(); // connect to WiFi
  socketIO.begin(host, port, path);
  socketIO.onEvent(socketIOEvent);

  while (!mpp.n_stage)
  {

    for (int i = 1; i <= MAX_SLAVES_ON_BUS; i++)
    {
      // requests a package for every possible slave
      WireSlaveRequest slaveReq(Wire, i, MAX_SLAVE_RESPONSE_LENGTH);
      slaveReq.setRetryDelay(1);
      slaveReq.setAttempts(5);
      bool success = slaveReq.request();
      if (success)
      {
        while (slaveReq.available())
        {
          slaveReq.read(); // receive byte as an integer
          mpp.n_stage = i; // the highest slave-address is also the quantity of stages(=slaves) on the bus
        }
        Serial.println(i);
      }
      else
      {
        // if something went wrong, print the reason
        Serial.println(slaveReq.lastStatusToString());
      }
    }
  }
  Serial.println((String)mpp.n_stage + " slaves registered on bus");

  oled_Wire.begin(18, 5);
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("SSD1306 allocation failed");
  }
  oled.display();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);

  pinMode(LOAD_BUTTON, INPUT_PULLUP);                                           // sets the LOAD-button pin as an input with pull-up
  attachInterrupt(digitalPinToInterrupt(LOAD_BUTTON), ISR_LOAD, FALLING);       // attaches ext-interrupt to LOAD-button
  pinMode(TRIGGER_BUTTON, INPUT_PULLUP);                                        // sets the TRIGGER-button pin as an input with pull-up
  attachInterrupt(digitalPinToInterrupt(TRIGGER_BUTTON), ISR_TRIGGER, FALLING); // attaches ext-interrupt to TRIGGER-button

  mpp.MPP_init();
  sendstatus = millis();
  lock_load = 0; // LOAD-button gets unlocked

  String status_json = getStatus();
  socketIO.sendEVENT(status_json);
}

void loop()
{
  // put your main code here, to run repeatedly:
  socketIO.loop();

  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.println(WiFi.localIP());
  oled.println("LOADED: " + (String)allOnLoad);
  oled.display();

  if ((millis() - sendstatus) > STATUS_UPDATE_TIME)
  {
    String status_json = getStatus();
    socketIO.sendEVENT(status_json);
    sendstatus = millis();
  }

  if (updateSettings)
  {
    // everytime updatesettings is set, data containing all settings is sent to all slaves
    updateSettings = 0;
    for (int i = 0; i < mpp.n_stage; i++)
    {
      WirePacker packer;
      packer.write((byte)'S');
      packer.write(mpp.stage[i].lsSelect);
      byte bytearray[4];
      bytearray[0] = (mpp.stage[i].dtSelect >> 24) & 0xFF;
      bytearray[1] = (mpp.stage[i].dtSelect >> 16) & 0xFF;
      bytearray[2] = (mpp.stage[i].dtSelect >> 8) & 0xFF;
      bytearray[3] = mpp.stage[i].dtSelect & 0xFF;
      packer.write(bytearray, 4);
      packer.end();

      Wire.beginTransmission(i + 1);
      while (packer.available())
      {
        Wire.write(packer.read());
      }
      Wire.endTransmission();
    }
  }

  if (ioEvent_flag)
  {
    ioEvent_flag = 0;
    if ((ioEvent_payload.indexOf("settings")) != -1)
    {
      Serial.println("payload: " + ioEvent_payload);
      ioEvent_payload.remove(0, 20);

      for (int i = 0; i < ioEvent_payload.length(); i++)
      {
        if (ioEvent_payload[i] == 92)
          ioEvent_payload.remove(i, 1);
      }

      DynamicJsonDocument settings_json(16384);
      DeserializationError error = deserializeJson(settings_json, ioEvent_payload);

      // Test if parsing succeeds.
      if (error)
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      String buffer;
      serializeJson(settings_json, buffer);
      Serial.println("Buffer: " + buffer);
      int test1 = settings_json["stage"][0]["dtSelect"];
      int test2 = settings_json["stage"][0]["lsSelect"];
      Serial.println("Test1: " + (String)test1);
      Serial.println("Test2: " + (String)test2);
      if (settings_json["n_stage"] == mpp.n_stage)
      {
        for (int i = 0; i < mpp.n_stage; i++)
        {
          mpp.stage[i].lsSelect = settings_json["stage"][i]["lsSelect"];
          mpp.stage[i].dtSelect = settings_json["stage"][i]["dtSelect"];
        }
      }
      updateSettings = 1;
    }
  }

  if (LOAD_flag)
  {
    LOAD_flag = 0;
    if (!allOnLoad)
    {
      for (int i = 0; i <= mpp.n_stage; i++)
      {
        WirePacker packer;
        packer.write((byte)'L');
        packer.end();
        Wire.beginTransmission(i + 1);
        while (packer.available())
        {
          Wire.write(packer.read());
        }
        Wire.endTransmission();

        do
        {
          WireSlaveRequest slaveReq(Wire, i + 1, MAX_SLAVE_RESPONSE_LENGTH);
          slaveReq.setRetryDelay(100);
          slaveReq.setAttempts(10);
          bool success = slaveReq.request();
          if (success)
          {
            char temp = slaveReq.read(); // receive byte as a char
            if (temp == 'L')
            {
              mpp.stage[i].onLoad = 1;
            }
          }
          else
          {
            // if something went wrong, print the reason
            Serial.println(slaveReq.lastStatusToString());
          }
        } while (!mpp.stage[i].onLoad);
      }
      bool areAllOnLoad = 1;
      for (int k = 0; k < mpp.n_stage; k++)
      {
        areAllOnLoad &= mpp.stage[k].onLoad;
      }
      if (areAllOnLoad)
      {
        allOnLoad = 1;
        Serial.println("LOADED");
        lock_trigger = 0;
      }
    }
  }
  if (TRIGGER_flag)
  {
    TRIGGER_flag = 0;
    if (allOnLoad)
    {
      allOnLoad = 0;
      for (int i = 0; i < mpp.n_stage; i++)
        mpp.stage[i].onLoad = 0;

      allMsDataReceived = 0;
      for (int i = 0; i < mpp.n_stage; i++)
      {
        mpp.stage[i].msData = 0;
      }
      delay(1000);
      while (!allMsDataReceived)
      {
        for (int i = 0; i < mpp.n_stage; i++)
        {
          WireSlaveRequest slaveReq(Wire, i + 1, MAX_SLAVE_RESPONSE_LENGTH);
          slaveReq.setRetryDelay(1);
          slaveReq.setAttempts(5);
          bool success = slaveReq.request();
          if (success)
          {
            byte a, b, c, d;
            a = slaveReq.read();
            b = slaveReq.read();
            c = slaveReq.read();
            d = slaveReq.read();

            mpp.stage[i].msData = a;
            mpp.stage[i].msData = (mpp.stage[i].msData << 8) | b;
            mpp.stage[i].msData = (mpp.stage[i].msData << 8) | c;
            mpp.stage[i].msData = (mpp.stage[i].msData << 8) | d;

            bool areAllMsDataReceived = 1;
            for (int k = 0; k < mpp.n_stage; k++)
            {
              bool isfull = (mpp.stage[k].msData != 0) ? 1 : 0;
              areAllMsDataReceived &= isfull;
            }
            if (areAllMsDataReceived)
            {
              allMsDataReceived = 1;
              lock_load = 0;
              for (int k = 1; k <= mpp.n_stage; k++)
              {
                Serial.print("Slave" + (String)k + ": msData = ");
                Serial.println(mpp.stage[k - 1].msData);
              }
              String msData_json = getmsData();
              http.post("/mppdata", "application/json", msData_json);

              int statusCode = http.responseStatusCode();
              String response = http.responseBody();

              Serial.print("Status code: ");
              Serial.println(statusCode);
              Serial.print("Response: ");
              Serial.println(response);
            }
          }
          else
          {
            // if something went wrong, print the reason
            Serial.println(slaveReq.lastStatusToString());
          }
        }
      }
    }
  }
}
String getmsData(void) // provides json-file with data for get-request
{
  DynamicJsonDocument doc(16384);

  doc["n_stage"] = mpp.n_stage;
  doc["max_velocity"] = mpp.stage[mpp.n_stage - 1].msData;
  for (int i = 0; i < mpp.n_stage; i++)
  {
    doc["msdata"][i]["lsSelect"] = mpp.stage[i].lsSelect;
    doc["msdata"][i]["dtSelect"] = mpp.stage[i].dtSelect;
    doc["msdata"][i]["msData"] = mpp.stage[i].msData;
  }

  String output;
  serializeJson(doc, output);
  return output;
}

String getStatus(void) // provides json-file with data for get-request
{
  DynamicJsonDocument doc(16384);
  JsonArray array = doc.to<JsonArray>();

  array.add("status");

  JsonObject param = array.createNestedObject();
  param["n_stage"] = mpp.n_stage;
  for (int i = 0; i < mpp.n_stage; i++)
  {
    param["stage"][i]["onLoad"] = mpp.stage[i].onLoad;
  }

  String output;
  serializeJson(doc, output);
  return output;
}

void socketIOEvent(socketIOmessageType_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case sIOtype_DISCONNECT:
    Serial.printf("[IOc] Disconnected!\n");
    break;
  case sIOtype_CONNECT:
    Serial.printf("[IOc] Connected to url: %s\n", payload);
    // join default namespace (no auto join in Socket.IO V3)
    socketIO.send(sIOtype_CONNECT, "/");
    break;
  case sIOtype_EVENT:
    Serial.printf("[IOc] get event: %s\n", payload);
    ioEvent_payload = (char *)payload;
    ioEvent_flag = 1;
    break;
  case sIOtype_ACK:
    Serial.printf("[IOc] get ack: %u\n", length);
    hexdump(payload, length);
    break;
  case sIOtype_ERROR:
    Serial.printf("[IOc] get error: %u\n", length);
    hexdump(payload, length);
    break;
  case sIOtype_BINARY_EVENT:
    Serial.printf("[IOc] get binary: %u\n", length);
    hexdump(payload, length);
    break;
  case sIOtype_BINARY_ACK:
    Serial.printf("[IOc] get binary ack: %u\n", length);
    hexdump(payload, length);
    break;
  }
}

void ISR_LOAD(void)
{
  if (!lock_load)
  {
    lock_load = 1;
    LOAD_flag = 1;
  }
}

void ISR_TRIGGER(void)
{
  if (!lock_trigger)
  {
    lock_trigger = 1;
    TRIGGER_flag = 1;
  }
}
