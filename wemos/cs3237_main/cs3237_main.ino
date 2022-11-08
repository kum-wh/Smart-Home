/**
 * ---------------------------------------
 * 
 * This is for cs32327
 * Isabella
 * 
 * ---------------------------------------
 */
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Wire.h>

// ------ WiFi ---------------
const char* ssid = "POW";    //hotstop username and pw
const char* password = "heuc4212";
String serverName = "http://ec2-54-254-251-57.ap-southeast-1.compute.amazonaws.com:8000/";

// ------ mood -----------
#define ANGRY 0
#define HAPPY 1
#define NEUTRAL 2

// ------- led ---------------
// you are suppose to connect 180ohm to R 
// and 110ohm to G and B 
// but I dont have 110ohm so i just use 120

#define lampR  D0
#define lampG  D1
#define lampB  D2

#define mainR  D3
#define mainG  D4
#define mainB  D5

#define hueR  D6
#define hueG  D7
#define hueB  D8

int white[3] = {250, 250, 250};
int warm[3] = {250, 60, 0} ;
int pink[3] = {40, 250, 200};
int blue[3] = {15, 250, 250};
int purple[3] = {30, 100, 250};

int x;
unsigned long moodLastTime = 0;
unsigned long slaveLastTime = 0;
unsigned long timerDelay = 5000; // set time to 5s

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(F("Hello! This is CS3237!"));
  Serial.println(F("Setting up WeMOS ......"));

  init_rgb();
  init_wifi();
  init_wire();
}

int mood = 0 ; 
int slave = 0;
void loop() {
  delay(100);

  /* test point for Serial input */
//  while(Serial.available() > 0) {
//    mood = Serial.parseInt(); // test cases for serial input 
//    set_lighting(mood);
//  }
  /* get mood from web and set ambient lighting based on mood*/
//  mood = get_mood();
//  set_lighting(mood);

  /* get intruder alert from second wemos and alarm through web */
  slave = get_slave();
  if (slave) {
    off_light();
  } else {
    mood = get_mood();
    set_lighting(mood);
  }
//  if (x == 1) { 
//    // turn off light
//    off_light();
//    Serial.println("lights off!");
//  }
}

void off_light(){
  set_led_colour(lampR, lampG, lampB, 0,0,0); // off lamp
  set_led_colour(mainR, mainG, mainB, 0,0,0); // off main
  set_led_colour(hueR, hueG, hueB, 0,0,0);    // off hue
}

void intruder_alarm() {
  Serial.println("intruder detected! ");
}

void set_lighting(int mood){
  switch(mood) {
    case HAPPY:
//      set_led_colour(lampR, lampG, lampB, 0,0,0); // off lamp
      // set to hue light
      set_led_colour(lampR, lampG, lampB, purple[0], purple[1], purple[2]);
      set_led_colour(mainR, mainG, mainB, pink[0], pink[1],pink[2]);  
      set_led_colour(hueR, hueG, hueB, blue[0], blue[1], blue[2]);
      Serial.println(F("lighting to suit happy mood"));
      break;
    case ANGRY:
      // dim lights and set to warm tone
      set_led_colour(lampR, lampG, lampB, 0,0,0);
      set_led_colour(mainR, mainG, mainB, 60, 250, 0);  
      set_led_colour(hueR, hueG, hueB, warm[0], warm[1], warm[2]);
      Serial.println(F("lighting to suit angry mood"));
      break;
    case NEUTRAL:
      // light up to suit working modes
      set_led_colour(lampR, lampG, lampB, white[0],white[1],white[2]);
      set_led_colour(mainR, mainG, mainB, white[0],white[1],white[2]);
      set_led_colour(hueR, hueG, hueB, warm[0],warm[1],warm[2]);
      Serial.println(F("lighting to suit neutral mood"));
      break;
    default:
      break;
  }
}

void receiveEvent(int bytes) {
  x = Wire.read(); // read one character from the I2C
  Wire.endTransmission(); // stop transmitting
}

void init_wire(){
  Wire.begin(1);  // address set to 1
  Wire.onReceive(receiveEvent); // function to trigger when something received 
}

void init_wifi() {
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Timer set to 5 seconds, it will take 5 seconds before publishing the first reading.");
}

int get_slave() {
  if ((millis() - slaveLastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      String serverPath = serverName + "slave";  // the address we getting from 
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverPath.c_str());
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
        return payload.toInt();
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    slaveLastTime = millis();
  }
}

/* Send an HTTP POST request depending on timerDelay */
int get_mood() {
  if ((millis() - moodLastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      String serverPath = serverName + "getemo";  // the address we getting from 
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverPath.c_str());
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
        return payload.toInt();
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    moodLastTime = millis();
  }
}

void init_rgb() {
  pinMode(lampR, OUTPUT);
  pinMode(lampG, OUTPUT);
  pinMode(lampB, OUTPUT);

  pinMode(mainR, OUTPUT);
  pinMode(mainG, OUTPUT);
  pinMode(mainB, OUTPUT);

  pinMode(hueR, OUTPUT);
  pinMode(hueG, OUTPUT);
  pinMode(hueB, OUTPUT);
}

void set_led_colour(int pinR, int pinG, int pinB, int R, int G, int B) {
  analogWrite(pinR, R);  
  analogWrite(pinG, G); 
  analogWrite(pinB, B); 
}
