#include <Arduino.h>
#include <Wire.h>
#include <WireSlave.h>
#include <FunctionalInterrupt.h>
#include <Stage_Functions.h>
#include <FastLED.h>

#define LS 4
#define LS_ENABLE 15
#define LS_PREV 17
#define CHARGE 25
#define RELAY_TEST 39
#define ADC_CAP 33
#define ADC_LV 32
#define FIRE 26
#define LS_TWO 16

//LED Defines
#define LED_PIN     5
#define CHIPSET     WS2812B
#define NUM_LEDS    4
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];


#define SDA_PIN 21
#define SCL_PIN 22
int slaveaddr = 0;


void requestEvent();
void receiveEvent(int howMany);


static bool isloaded = 0;
volatile bool charge_flag=false;
volatile static uint32_t measurement_data = 0;


void ISR_LS(void);



void setup()
{
  init_timer();

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); 
  FastLED.setBrightness(20);
  fill_solid(leds, NUM_LEDS, CRGB(0,0,50));
  FastLED.show();
  fire=FIRE;
  //Ein- und Ausgänge setzen
  pinMode(CHARGE, OUTPUT);
  pinMode(FIRE, OUTPUT);
  pinMode(LS_ENABLE, OUTPUT);
  pinMode(ADC_CAP, INPUT);
  pinMode(ADC_LV, INPUT);
  pinMode(RELAY_TEST, INPUT);
  pinMode(LS_TWO, INPUT_PULLDOWN);
  
  
  /* Adressvergabe  */
  Serial.begin(115200);
  Serial.println("Waiting for Adress");
  
  slaveaddr = get_address_send_address(LS_PREV, LS); 
  Serial.println("Got Adress");
  //Lichtschranke aktivieren, nachdem die Verbindung zum Adressieren verwendet wurde
  pinMode(LS, INPUT_PULLDOWN);
  digitalWrite(LS_ENABLE,HIGH);
  pinMode(LS_PREV, INPUT);
 //I2C initalisieren
  bool res = WireSlave.begin(SDA_PIN, SCL_PIN, slaveaddr);
  if (!res)
  { fill_solid(leds, NUM_LEDS, CRGB(200,0,0)); //Alle LEDs auf rot setzen um einen Error zu symbolisieren
    Serial.println("I2C slave init failed");
    while (1)
      delay(100);
  }
  nr_to_led(slaveaddr,leds,CRGB(0,50,100));
  requestselect = 1;
  WireSlave.onRequest(requestEvent);
  WireSlave.onReceive(receiveEvent);
  Serial.printf("Slave joined I2C bus with addr #%d\n", slaveaddr);


init_timer(); //Timer für verzögertes beschleunigen initalisieren
 
}

void loop()
{ 

  
  if(charge_flag)//Laden der Kondensatoren
  {charge_flag=0;
    isloaded =load_capacitors(ADC_LV,ADC_CAP,CHARGE,leds,NUM_LEDS);
  if(isloaded) //wenn erfolgreich geladen wurde
    { digitalWrite(LS_ENABLE,HIGH);
      pinMode(LS, INPUT_PULLDOWN);
      pinMode(LS_PREV, INPUT);
      attachInterrupt(LS,ls_isr,FALLING);
      attachInterrupt(LS_PREV,prev_ls_isr,FALLING);
      percent_to_led(100,leds,NUM_LEDS);	//Fertig geladen anzeigen 
    }
    else // wenn der ladevorgang abgebrochen wurde
    {
       fill_solid(leds, NUM_LEDS, CRGB(200,0,0)); //Alle LEDs auf rot setzen um einen Error zu symbolisieren
       FastLED.show();
    }
    
  }
 if(lsflag) // nach durchschreiten der Lichtschranken
 {
   //Zeit ausgeben 
  Serial.println("Time=");
  Serial.println(deltat);
  lsflag=0;
  //Statusanzeige auf weiß
  fill_solid(leds, NUM_LEDS, CRGB(200,200,200));  
  FastLED.show();
 }
  WireSlave.update();

}








//Communication function


void requestEvent()
{
  switch (requestselect)
  {
  case 1:
    if (isloaded)
    {
      WireSlave.write((byte)'L');
    }
    else
    {
      WireSlave.write((byte)'N');
    }
    break;
  case 2:
   //Gemessene Zeit zurückgeben
    byte bytearray[4];
    bytearray[0] = (deltat >> 24) & 0xFF;
    bytearray[1] = (deltat >> 16) & 0xFF;
    bytearray[2] = (deltat >> 8) & 0xFF;
    bytearray[3] = deltat & 0xFF;
    WireSlave.write(bytearray, 4);
    requestselect = 0;
    in=1; 
    break;
  default: 
    WireSlave.write((byte)'E');
    break;
    

  }

  
}

void receiveEvent(int howMany)
{
  Serial.println("Event received");
  char c = WireSlave.read();
  Serial.println(c);
  switch (c)
  {
  case 'S': //Wartezeit und Lichtschranke vor dem Beschleunigen einstellen
    
    if (!isloaded)
    {
      lsSelect = (bool)WireSlave.read();
      Serial.print("lsSelect = ");
      Serial.println(lsSelect);
      byte a, b, c, d;
      a = WireSlave.read();
      b = WireSlave.read();
      c = WireSlave.read();
      d = WireSlave.read();

      dtSelect = a;
      dtSelect = (dtSelect << 8) | b;
      dtSelect = (dtSelect << 8) | c;
      dtSelect = (dtSelect << 8) | d;
      Serial.print("dtSelect = ");
      Serial.println(dtSelect);
      }
      
  break;
    
  case 'L': //Befehl zum Laden geben 
    digitalWrite(FIRE, LOW);
    /*Laden*/
    charge_flag=true;
    requestselect = 1;
  break;

  case 'F': //Beschleunigen
  digitalWrite(FIRE, HIGH);
  break;

  case 'I': // Alle LEDs beschreiben 
      byte R,G,B; 
      R = WireSlave.read();
      G = WireSlave.read();
      B = WireSlave.read();
      fill_solid(leds,4,CRGB(R,G,B)); 
      FastLED.show();
  
  break;
  
  case 'i': //LED einzeln umstellen
  byte lednr;
  lednr = WireSlave.read();
  R = WireSlave.read();
  G = WireSlave.read();
  B = WireSlave.read();
  leds[lednr] = CRGB(R,G,B);
  FastLED.show();
  break;
  
  }
}

