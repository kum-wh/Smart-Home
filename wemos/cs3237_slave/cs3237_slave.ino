#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Ticker.h>
#include <WiFiClient.h>
//#include <AsyncMqttClient.h>
//#include <SPI.h>

#define MAIN_WEMOS 1
#define echoPin D4
#define trigPin D5
#define MAX_PPL 2
const char* WIFI_SSID = "POW";    //hotstop username and pw
const char* WIFI_PASSWORD = "heuc4212";
String serverName = "http://ec2-54-254-251-57.ap-southeast-1.compute.amazonaws.com:8000/";
//#define MQTT_HOST IPAddress(192, 168, 1, XXX)
#define MQTT_HOST "broker.emqx.io"  // for cloud broker
#define MQTT_PORT 1883
#define MQTT_PUB_ALARM "cs3237/wemos/intruder"  //topic

#define RED_LED D1
#define GREEN_LED D2
#define BLUE_LED D3

//AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

unsigned long previousMillis = 0;   // Stores last time published
const long interval = 10000;        // Interval to publish sensor readings

long duration;
int distance;

//int ppl_in_room = 0;
bool ppl_in_room = false;
bool hasChanged = false;
bool hasPosted = false;

void setup() {
  Serial.begin(115200);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  off_led();

//  Wire.begin(9); //set self address to 9
//  Wire.onReceive(receiveEvent);

  init_wifi();

//  init_mqtt();
}


void loop() {
  detect_ppl();
  Serial.print("People in room? ");
  Serial.println(ppl_in_room);
  
  if (ppl_in_room) {
    post_to_server("1");
//    send_mqtt("intruder detected");
    digitalWrite(LED_BUILTIN, HIGH);
    on_led();
  } else {
    post_to_server("0");
    digitalWrite(LED_BUILTIN, LOW);
    off_led();
  }

  delay(100); // 10 hz 
}

void receiveEvent(int bytes) {
  // nothing cause master won't be sending
}

void detect_ppl() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  Serial.print("Distance: ");
  Serial.println(distance);
  if (distance < 100 && !hasChanged) {   // cm close to the sensor for a ppl to be detected
    ppl_in_room = !ppl_in_room;
    hasPosted = false;
    hasChanged = true;
  }
  if (distance >= 100 && hasChanged) {
    hasChanged = false;
  }
}

/* sends 1 to main wemos if motion detected */
//void send_to_master(int msg) {
//  Wire.beginTransmission(MAIN_WEMOS);
//  Wire.write(msg);
//  Wire.endTransmission();
//}

void off_led() {
  analogWrite(RED_LED, 0);
  analogWrite(GREEN_LED, 0);
  analogWrite(BLUE_LED, 0);
}

void on_led() {
  analogWrite(RED_LED, 255);
  analogWrite(GREEN_LED, 255);
  analogWrite(BLUE_LED, 255);
}

void init_wifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
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

void post_to_server(String message) {
  if ((WiFi.status() == WL_CONNECTED)) {

    if (!hasPosted) {
        WiFiClient client;
        HTTPClient http;
    
        String serverPath = serverName + "slave";
        
        http.begin(client, serverPath.c_str());
        http.addHeader("Content-Type", "text/plain");
    
        int httpCode = http.POST(message);
    
        if (httpCode > 0) {
          Serial.print("Response code: ");
          Serial.println(httpCode);
          
          if (httpCode == HTTP_CODE_OK) {
            const String& payload = http.getString();
            Serial.print("Server received payload: ");
            Serial.println(payload);
    
            hasPosted = true;
          }
        } else {
          Serial.print("Error code: ");
          Serial.println(httpCode);
        }
        http.end();
    }
  } else {
    Serial.println("WiFi Disconnected");
  }
}

//void init_mqtt() {
//  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
//  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
//  
//  mqttClient.onConnect(onMqttConnect);
//  mqttClient.onDisconnect(onMqttDisconnect);
//  //mqttClient.onSubscribe(onMqttSubscribe);
//  //mqttClient.onUnsubscribe(onMqttUnsubscribe);
//  mqttClient.onPublish(onMqttPublish);
//  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
//  // If your broker requires authentication (username and password), set them below
//  //mqttClient.setCredentials("REPlACE_WITH_YOUR_USER", "REPLACE_WITH_YOUR_PASSWORD");
//  connectToWifi();
//}
//
//
//void connectToWifi() {
//  Serial.println("Connecting to Wi-Fi...");
//  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//}
//
//void onWifiConnect(const WiFiEventStationModeGotIP& event) {
//  Serial.println("Connected to Wi-Fi.");
//  connectToMqtt();
//}
//
//void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
//  Serial.println("Disconnected from Wi-Fi.");
//  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
//  wifiReconnectTimer.once(2, connectToWifi);
//}
//
//void connectToMqtt() {
//  Serial.println("Connecting to MQTT...");
//  mqttClient.connect();
//}
//
//void onMqttConnect(bool sessionPresent) {
//  Serial.println("Connected to MQTT.");
//  Serial.print("Session present: ");
//  Serial.println(sessionPresent);
//}
//
//void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
//  Serial.println("Disconnected from MQTT.");
//
//  if (WiFi.isConnected()) {
//    mqttReconnectTimer.once(2, connectToMqtt);
//  }
//}

/*void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}*/

//void onMqttPublish(uint16_t packetId) {
//  Serial.print("Publish acknowledged.");
//  Serial.print("  packetId: ");
//  Serial.println(packetId);
//}
//
///* send the msg to mqtt */
//void send_mqtt(String msg) {
//  uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_ALARM, 1, true, String(msg).c_str());
//}
