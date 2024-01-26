#define BLYNK_PRINT Serial    
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
WidgetLED FAN(V0); 
#include "DHT.h"
#include <SimpleTimer.h>
#define DHTPIN D7     // pin  DATA with D4
#define FAN_PIN D2    // FAN RELAY
#define DHTTYPE DHT11   
#define COLD_TEMP     31
#define HOT_TEMP      33
#define TIME_FAN_ON  10
#define READ_AIR_DATA_TM  1L  
#define SEND_UP_DATA_TM   1L 
#define AUTO_CTRL_TM      1L 


//#include <Adafruit_NeoPixel.h>
#include <SPI.h>


#define PIN D1
#define NUMPIXELS 12
#define BLYNK_PRINT Serial
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);



char auth[] = "gHZzDS7KwWJADfyqhEtBsWH0paQxKdO-";
char ssid[] = "LAB_TESTING";
char pass[] = "XXXXXXXX;

float humDHT = 0;
float tempDHT = 0;
boolean fanStatus = 0;
int timeFanOn = 1; 
long sampleTimingSeconds = 20; 
long startTiming = 0;
long elapsedTime = 0;
SimpleTimer timer;
DHT dht(DHTPIN, DHTTYPE);



void setup() {
    
    pinMode(D5,OUTPUT);
         digitalWrite(D5,HIGH);
    pinMode(D6,OUTPUT);
         digitalWrite(D6,HIGH);
    pinMode(FAN_PIN, OUTPUT);
         aplyCmd();
  
  Serial.begin(115200);
  dht.begin();
  
//  pixels.begin(); 
     
  Blynk.begin( auth, ssid , pass );
  FAN.off();
 startTimers();
}


BLYNK_WRITE(V1)
{

int R = param[0].asInt();
int G = param[1].asInt();
int B = param[2].asInt();
Serial.println(R);
Serial.println(G);
Serial.println(B);
for(int i=0;i<NUMPIXELS;i++){

//pixels.setPixelColor(i, pixels.Color(R,G,B));

//pixels.show();
}
}




void loop() {
  timer.run(); 
  Blynk.run();
}
BLYNK_WRITE(3) 
{
  int i = param.asInt();
  if (i == 1)
  {
    fanStatus = !fanStatus;
    aplyCmd();
  }
}

void getDhtData(void)
{
tempDHT = dht.readTemperature();
  humDHT = dht.readHumidity();
  if (isnan(humDHT) || isnan(tempDHT))   
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
}
void aplyCmd()
{
  if (fanStatus == 1)
  {
    Blynk.notify("fan ON");
    digitalWrite(FAN_PIN, LOW);
    FAN.on();
  }

  else {
    digitalWrite(FAN_PIN, HIGH);
    FAN.off();
  }
}
void autoControlPlantation(void)

{
  if (tempDHT > HOT_TEMP)
  {
    turnFanOn();
  }
}

void turnFanOn()
{
  fanStatus = 1;
  aplyCmd();
  delay (TIME_FAN_ON * 1000);
  fanStatus = 0;
  aplyCmd();
}
void startTimers(void)
{
  timer.setInterval(READ_AIR_DATA_TM * 1000, getDhtData);
  timer.setInterval(SEND_UP_DATA_TM * 1000, sendUptime);
  timer.setInterval(AUTO_CTRL_TM * 1000, autoControlPlantation);
}
void sendUptime()
{
  Blynk.virtualWrite(2, tempDHT); 
  Blynk.virtualWrite(3, humDHT); 
  
}
