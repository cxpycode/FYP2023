//  Libraries in use  //
#include <Arduino.h> //Remove if using Arduino IDE
#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>


//  Define DHT Sensor  //
#define DHT_SENSOR_PIN  21 // ESP32 pin GIOP21 connected to DHT11 sensor
#define DHT_SENSOR_TYPE DHT11 //Accepts DHT11 or DHT22
DHT dht(DHT_SENSOR_PIN, DHT_SENSOR_TYPE); //'dht' name of DHT11 variable


//  School Wifi Credentials  //
const char* ssid = "hkst2023";
const char* password = "BEftsp2023#";
WiFiClient espClient;


//  MQTT Broker Setup  //
// const char* mqttServer = "XX.XXX.X.XXX"
const char* mqttServer = "XX.XXX.X.XXX" //Input own IP address of wifi;
const int mqttPort = 1883;
const char* mqttID = "FlaskMan";
PubSubClient client(espClient);


//  MQTT Topics
const char* humiTopic = "sensor/humidity";
const char* tempTopic = "sensor/temperature";


//  Variables
float temperature;
float humidity;



//  Functions
//  Prompts ESP32 to Connect to WiFi
void connectWifi(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to...");
    Serial.print(ssid);
  }
  Serial.println("WiFi connected");  
  Serial.println(WiFi.localIP());
}

//  Reconnects ESP32 to the MQTT Broker
void reconnect(){
    while(!client.connected()){
        Serial.print("Attempting MQTT connection...");

        if (client.connect(mqttID)){
            Serial.println("Reconnected");
        }
        else {
            //  Added for troubleshooting purposes
            Serial.println(WiFi.status());
            Serial.println(WiFi.localIP());

            //  Check return code and client state
            Serial.print("failed, rc=");
            Serial.println(client.state());
            Serial.println("(Trying again in 2.5 seconds)");
            delay(2500);
        }
    }
}

//  Prints out DHT11 Sensor Readings
void printReadings(float(temp),float(humi)){
    Serial.print("Humidity: ");
    Serial.print(humi);
    Serial.print(" %\t");
    Serial.print("| Temperature: ");
    Serial.print(temp);
    Serial.println(" Deg");
    Serial.println(" ");
}


//  Main Code
void setup() {
    //  delay gives chance for Serial Monitor to run
    delay(500);
    Serial.println("Serial begin");
    Serial.begin(9600);
    
    connectWifi();  //Connect to the wifi
    dht.begin();    //Initialize DHT11
    client.setServer(mqttServer,mqttPort); //Set Up Server
    client.setKeepAlive(120); //Timeout value increased to 120s
}

void loop() {
    if (!client.connected()){
        reconnect();
    }
    client.loop();

    //  Sensor Readings
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)){
        Serial.println("Failed to read from DHT11!");
        return;
    }

    printReadings(temperature,humidity);

    //  publish sensor readings
    client.publish(tempTopic,String(temperature).c_str(),true);
    client.publish(humiTopic,String(humidity).c_str(),true);

    //  delay before next loop
    delay(2000);
}

