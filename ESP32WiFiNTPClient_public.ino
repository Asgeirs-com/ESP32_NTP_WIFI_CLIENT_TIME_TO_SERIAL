/*
 *  This simple sketch gets NTP time from a WEB NTP server,
 *  and sends time and day to serial line.
 *  You need to edit ssid and password to fit your wifi access point 
 *  below.  
 *  This code was based on:
 *     https://github.com/marcudanf/esp-ntp/tree/master/NTPtimeESP-master
 *     https://github.com/marcudanf/espclock/blob/master/esp/esp.ino
 *     Arduino buildt in WIFI CLIENT example
 */

#include <WiFi.h>


const char* ssid     = "your-ssid";
const char* password = "your-password";

#include <NTPtimeESP.h>

#define threshold 35
 


NTPtime NTPch("ch.pool.ntp.org");


float timezone=2.0;

byte err=0;
byte lastMin=-1;
bool state=0;
int prev=45;

strDateTime dateTime;
const char Day[7][10]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
const char Month[12][6]={"Jan.","Feb.","March","Apr.","May","June","July","Aug.","Sept.","Oct","Nov","Dec."};

void setup()
{
    Serial.begin(115200);
    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
      retry:
  dateTime=NTPch.getNTPtime(timezone, 1);
  if(!dateTime.valid)
  goto retry;
  String time;
  if(dateTime.hour<10)
    time+='0';
  time+=String(dateTime.hour)+':';
  if(dateTime.minute<10)
    time+='0';
  time+=String(dateTime.minute);
     Serial.print(time);
  uint64_t started_at=millis();
}

int value = 0;

void loop()
{
  if(WiFi.status()!=WL_CONNECTED){
    WiFi.begin(ssid,password);
 
    Serial.print("No WiFiconnection\nretrying...");
     
  }
  while(WiFi.status()!=WL_CONNECTED)
  delay(1);
  dateTime=NTPch.getNTPtime(timezone, 1);
  if(dateTime.valid==0){
    err++;
    if(err>1500){
 
          Serial.println("Could not get\ndate and time,\nreset router");
 
      ESP.deepSleep(1000000*60*24*10);
    }
    return;
  }
  if(dateTime.valid&&dateTime.minute!=lastMin){
    err=0;
    lastMin=dateTime.minute;
    String time;
    if(dateTime.hour<10)
      time+='0';
    time+=String(dateTime.hour)+':';
    if(dateTime.minute<10)
      time+='0';
    time+=String(dateTime.minute);
        Serial.println(time);
  }
  uint64_t started_at=millis();
  for(int i=started_at;i<started_at+1000*(60-dateTime.second);i=millis()){
    delay(100);
     
      state=!state;
      if(!state){
        String time;
        if(dateTime.hour<10)
          time+='0';
        time+=String(dateTime.hour)+':';
        if(dateTime.minute<10)
          time+='0';
        time+=String(dateTime.minute);
        Serial.println(time);
      }else{
        String date;
        if(dateTime.day<10)
        date+='0';
        date+=dateTime.day;
        date+=Month[dateTime.month-1];
        date+=dateTime.year;
        String line = String(Day[dateTime.dayofWeek-1]);
        Serial.println(line);
      }
    }
}
