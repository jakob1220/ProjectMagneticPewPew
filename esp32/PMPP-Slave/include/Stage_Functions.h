
#ifndef Stage_Functions
#define Stage_Functions

#include <Arduino.h>
#include <FastLED.h>
#include <FunctionalInterrupt.h>
//File kan und sollte nicht ohne die passenden konfigurationen im main verwendet werden
//Das hier ist keine modulare library
//Slaves adressieren

volatile int requestselect = 0;

volatile int cnt = 0;
volatile unsigned long t = 0;
volatile int counting = 1;

volatile unsigned long start; //Startzeit zum Geschwindigkeit messen
volatile unsigned long deltat; // Zeit zwischen den Lichtschranken
volatile bool in=1,out=1; 
volatile int lsflag=0; 
static bool lsSelect=0;
static uint16_t dtSelect=0;

int fire=26;

hw_timer_t * timer = NULL;



void IRAM_ATTR acc() //Interrupt funktion für den Beschleunigermagneten
{
  timerAlarmDisable(timer); // weitere Timer interrupts verhindern
  digitalWrite(fire,HIGH); // den Magneten einschalten 
  timerWrite(timer, 0);  //timer zurücksetzen 
}

void init_timer() //Timer Initalisieren
{
  timer = timerBegin(0, 80, true); // Timer mit einer Taktrake von 1uS setzen 
  timerAttachInterrupt(timer, &acc, true); // Interrupt hinzufügen ( ist noch nicht enabled)  
}
void IRAM_ATTR prev_ls_isr() // ISR für die Lichtschranke der vorherigen Stage
{ unsigned long t=micros(); // Sistemzeit zwischenspeichen 
  if(in){ //wenn die ISR noch nicht ausgeführt worden ist (seit dem aufruf der ISR ls_isr() )
  start = t; //start der messung ist t
  if(lsSelect) // wenn ein schalten relativ zu der vorherigen Lichtschranke konfiguriert wurde
  { if(dtSelect>2) //wenn die wartezeit vor dem Schalten >2us ist; kleinere Zeiten können zu Errors führen
    {timerWrite(timer, 0); //  timer zurücksetzen
    timerAlarmWrite(timer, dtSelect, true); // Interrupt nach dtselect zeitschritten 
    timerAlarmEnable(timer);} //Interrupt aktivieren 
    else{digitalWrite(fire,HIGH);} // wenn die zeit zu kurz ist wird gleich der Magnet aktiviert 
    }
    /*richtige reihenfolge der Interrupts sicherstellen*/
  in=0;
  out=1;
  }
}
void IRAM_ATTR ls_isr()
{
  unsigned long t=micros();//Systemzeit zwischenspeichern
  if(out){ // Wenn der Interrupt der vorherigen Lichtschranke schon ausgeführt wurde
  if(t<start)              //verhindern dass ein overflow der systemzeit(2^32us ~71min) während dem messvorgang zu einem Problem wird
  {deltat=t+(4294967296-start);
  }
  else 
  {deltat = t-start;
  }
  if(!lsSelect) //wenn die Interne lichtschranke zum schalten ausgewählt ist 
  { //Siehe prev_ls_isr zeilen 48-52
    if(dtSelect>2) 
    {timerWrite(timer, 0);
    timerAlarmWrite(timer, dtSelect, true);
    timerAlarmEnable(timer);}
    else{digitalWrite(fire,HIGH);}
    }
    out=0;
    lsflag=1;// Flag, damit die LEDs gesetzt werden 
  }
  
  requestselect=2; // Ermöglicht ein abfragen des gemessenen wertes
}


void count() //zählt adressierungspulse
{   
    unsigned long x = micros(); //Systemzeit zwischenspeichern
    if (t == 0){ //wenn noch kein puls empfangen wurde
        t = x; //zeit speichen 

    }
    else if ( (x - t) < 500) // wenn die zeit seit der letzten flanke kürzer als 500us ist
    {            
                cnt++; // Adresse hinaufzählen
                t = x;   // neue referenz für nächste flanke speichern      
    }
    else if (((x - t) > 2000)&& (cnt!=0)) // Wenn schon gezählt worden ist und die letzte flanke länger als 2ms her ist 
    {
                counting = 0; // Zählvorgang beenden
                t=0; //zeit zurücksetzen 
    }
}

    int get_address_send_address(int adress_pin, int next_adress_pin) //adressieren der stage
    {   //ein und ausgänge setzen 
        
        pinMode(adress_pin,INPUT_PULLDOWN);
        pinMode(next_adress_pin,OUTPUT);
        digitalWrite(next_adress_pin,LOW);
        delay(5);
        
        attachInterrupt(adress_pin,count,RISING);//Zählvorgang starten
        while(counting){} // warten bis das zählen abgeschlossen ist
        int adress = cnt; // Adresse auslesen 
        

        for(int x=0; x<=(adress+1); x++) //nächste Adresse weitergeben
        {
          digitalWrite(next_adress_pin,HIGH);
          delayMicroseconds(150);
          digitalWrite(next_adress_pin,LOW);  
          delayMicroseconds(150);  
        }
          delayMicroseconds(2100);
          digitalWrite(next_adress_pin,HIGH);

        return adress;//Eigene Adresse zurückgeben
    }




//Bunte lichter für mehr speed

DEFINE_GRADIENT_PALETTE( charge_PL ) { //Palette für die Umwandlung eines Prozentwertes in eine LED visualisierung 
100,   255,  0,  0,   //rot
190,   255,255,  0, //gelb
230,   0,255,0 ,
255,   0,255,2 }; //grün

 void percent_to_led(int percent, CRGB led[4], int ledcnt)
    {
        CRGBPalette16 pl = charge_PL;
        uint8_t pipl = percent * 0.94 * 2.55;                                    //Prozent in in den bereich 0-255 umwandeln für die Palette( *94 weil auf den letzten 6% die Palette einen Bug hat)
        int ledson = (((ledcnt - 1) * percent) / 100) + 1;                       //Eine Led leuchtet immer, wenn mehrere die letzte nur  bei 100%
        float fade = 255 - (2.55 * (percent - ((100 / ledcnt) * (ledson - 1)))); // berechnet um wie viel die letzt zugeschaltene Led abgedunkelt wird bevor sie 100% gefüllt ist

        FastLED.clear(); // Anzeige zurücksetzen 
        for (int x = 0; x < ledson; x++) // alle nötigen LEDs in der richtigen Farbe einschalten 
        {
            led[x] = ColorFromPalette(pl, pipl); 
        }
        fadeToBlackBy(&(led[ledson - 1]), 1, fade); //letzte LED dimmen
        FastLED.show(); //Anzeige aktualisieren 
        
    }

 void nr_to_led(int number, CRGB led[4], CRGB color) //zahl in binär für die anzeige umwandeln
 {  
   for(int x=0; x<=3; x++) 
   {
     if(number&(0x01<<x)) //für jede stelle vergleich mit maske
     {led[x]=color; // wenn die stelle eins ist, led setzen 
     }
     else
     {led[x]=CRGB(0,0,0); //wenn die setelle null ist led auf schwarz setzen
     }
     FastLED.show(); //anzeige aktualisieren
   }


 }



/*laden der kondensatoren*/


//unabsichtlich auf englisch kommentiert
int load_capacitors(int V_charge_pin, int V_cap_pin, int charge_cont)
{ int charge_dc=512; //charging duty cycle set to 50% at the beginning 
  int ref; //reference reading prior to charging
  int cap_v=0,chr_v;
  digitalWrite(26,LOW);
  ledcSetup(15, 10000, 10);//initialize PWM with Channel, Frequency and Resolution 
  ref=analogRead(V_charge_pin); // get a voltage reference
  delay(150); // wait shortly, so all Stages had the chance to get a reference reading
  ledcAttachPin(charge_cont, 15);
  Serial.println(ref);  
  while(cap_v <= ref*0.95)
  { 

    cap_v=analogRead(V_cap_pin);
    chr_v=analogRead(V_charge_pin); 
    if(chr_v>=0.95*ref)
    {charge_dc+=2;}   
    /*using different size to count up and down to prevent oscillation
    and ganin accuracy, but not using 1 so the feddback loop works faster */ 
    else if(chr_v<0.70*ref)
    {charge_dc-=3;}
    if(charge_dc<=0)        //Abort charging in case of uncontrollable voltage drop 
    { ledcWrite(15,0);
      return 0;}
    else if(charge_dc>=900) // off time is needed to recharge cap
    {charge_dc-=5;
     }
    ledcWrite(15, charge_dc); // make duty cycle adjustment

   
  }   
  ledcWrite(15,0);
  return 1;
}

// Same function as above but with fancy lights for the charge progress
int load_capacitors(int V_charge_pin, int V_cap_pin, int charge_cont, CRGB led[4], int ledcnt)
{ int charge_dc=512; //charging duty cycle set to 50% at the beginning 
  int ref; //reference reading prior to charging
  int cap_v=0,chr_v;
  ledcSetup(15, 10000, 10);//initialize PWM with Channel, Frequency and Resolution 
  ref=analogRead(V_charge_pin); // get a voltage reference
  delay(150); // wait shortly, so all Stages had the chance to get a reference reading
  ledcAttachPin(charge_cont, 15);
  
  while(cap_v <= ref*0.95)
  {
    cap_v=analogRead(V_cap_pin);
    chr_v=analogRead(V_charge_pin); 
    //add some RGB  :)
    percent_to_led((cap_v*100)/ref, led, ledcnt);
    if(chr_v>=0.95*ref) //increase duty cycle in case of low voltage drop
    {charge_dc+=1;}   
  
    else if(chr_v<0.85*ref) // decrease duty cycle in case of high voltage drop
    {charge_dc-=1;}
    if(charge_dc<=0)         
    { 
      charge_dc=1;    //setting the duty cycle to a minimum

      /*alternative precedure:*/
      //ledcWrite(15,0);     //Abort charging in case of uncontrollable voltage drop
      //return 0;
    }
    else if(charge_dc>=900) // off time is needed to recharge cap
    {charge_dc-=5;
     }
    ledcWrite(15, charge_dc); // make duty cycle adjustment 
  }   
  ledcWrite(15,0); // end PWM
  return 1; 
}



#endif

