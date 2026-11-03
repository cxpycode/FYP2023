#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <WiFi.h>

#define DHT_SENSOR_PIN  21 // ESP32 pin GIOP21 connected to DHT11 sensor
#define DHT_SENSOR_TYPE DHT11 //Accepts DHT11 or DHT22
DHT dht(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

//  School Wifi Credentials
const char* ssid = "hkst2023";
const char* password = "BEftsp2023#";

//  Home WiFi Credentials - INPUT OWN WIFI CREDENTIALS
//const char* ssid = "XXXXXXXXX";
//const char* password = "XXXXXXXXX";

//  Web Server 
WebServer server(80);

//  Variables
float temperature, humidity;
unsigned long measureDelay = 5000;                //    NOT LESS THAN 2000!!!!!   
unsigned long lastTimeRan;

StaticJsonDocument<1024> jsonDocument;
char buffer[1024];


//  Functions
void connectWifi(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected");  
  Serial.println(WiFi.localIP());
}

void createJson(char *name, float value, char *unit) {  
  jsonDocument.clear();
  jsonDocument["name"] = name;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, buffer);  
}
 
void addJsonObject(char *name, float value, char *unit) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj["name"] = name;
  obj["value"] = value;
  obj["unit"] = unit; 
}

void getValues() {
  Serial.println("Get all values");
  jsonDocument.clear(); // Clear json buffer
  addJsonObject("Temperature", temperature,"Deg");
  
  addJsonObject("Humidity", humidity,"%");
  
  serializeJson(jsonDocument, buffer);
  server.send(200, "application/json", buffer);
}

void setupApi() {
  server.on("/getValues", getValues);
  //server.on("/setStatus", HTTP_POST, handlePost);
  delay(500);

  // start server
  server.begin();
}

//  Main Code
void setup() {
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(500); 
  Serial.println("Serial Begin");
  Serial.begin(115200);

  connectWifi();
  setupApi();
}

void loop() {
  delay(1000);
  server.handleClient();
  // measurements every measureDelay ms
  if (millis() > lastTimeRan + measureDelay)  {
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
    lastTimeRan = millis();
    Serial.println(WiFi.localIP());
    Serial.print("Temperature: ");
    Serial.print(temperature,1);
    Serial.print("°C");
    Serial.print(", Humidity: ");
    Serial.print(humidity,1);
    Serial.println("%\n");
   }
}
