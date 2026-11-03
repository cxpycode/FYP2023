//  Libraries in use  //
#include <Arduino.h>  //Remove if using Arduino IDE
#include <NewPing.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <PubSubClient.h>


//  School Wifi Credentials //
const char* ssid = "hkst2023";
const char* password = "BEftsp2023#";
WiFiClient espClient;


//  MQTT Broker Setup //
// const char* mqttServer = "192.168.1.100";
const char* mqttServer = "192.168.1.10";
const int mqttPort = 1883;
const char* mqttID = "ultrasonic";
PubSubClient client(espClient);
//  MQTT Topics 
const char* quantityTopic = "sensor/quantity";


//  Front(1) and Bottom(2) LDR Setup + Variables  // 
#define pinLDR_front 32
#define pinLDR_bottom 35
int valLDR_front;
int valLDR_bottom;
const int frontLDR_rest = 2100;
const int bottomLDR_rest = 600;
const int frontLDR_covered = 3000;
const int bottomLDR_covered = 4000;


//  SG90 (Servo Motor) Setup  //
#define servoPin 13
Servo servo;      
int thisPos;  //current reading of position
int prevPos;  //records previous Position of motor
const int startPos = 100; //  Starting position of Servo motor
int delayPeriod = 15; //  Delay period to control speed


// HC-SR04 (Ultrasonic Sensor) Setup + Variables  //
#define trigPin 5 
#define echoPin 18
#define maxDist 50 // Set the maximum distance to 50cm
NewPing sonar(trigPin, echoPin, maxDist);

const float startDist = 2.0;
const float productWidth = 2.4;
const int totalProducts = 3;
const int totalDist = 12;
unsigned int distance;
float difference;
int productQuantity;
int productTaken;



//  Functions //
//  Prints all Sensor readings
void displayReadings(float(thisDist),int(LDR1),int(LDR2)){
  // Print the distance to the serial monitor

  //  Prints HC-SR04 Detected Distance
  Serial.print("Distance from back Sensor: ");
  Serial.print(round(distance));
  Serial.println(" cm");

  //  Prints LDR values
  Serial.print("Front light sensor Value: ");
  Serial.println(LDR1);
  Serial.print("Bottom light sensor Value: ");
  Serial.println(LDR2);

  Serial.println("*********************************************************");
}

//  Checks distance between HC-SR04 and product to find the quantity
void checkQuantity(float(thisDist)){
  // Calculate how many was taken and leftover
  difference = thisDist - startDist;
  productTaken = round(difference/productWidth);
  productQuantity = totalProducts - productTaken;
  
  //  If Distance detected is within Start Range
  if (0 < thisDist && thisDist <= startDist){
    Serial.print("Product left: ");
    Serial.println(productQuantity);
  }

  //  If Distance is more than Start distance
  else if(thisDist > startDist && thisDist < totalDist){
    
    // Check if the product removed is more than limit
    if(productTaken >= totalProducts){
      Serial.println("Please Restock!!!");
      Serial.print("Products Left: ");
      Serial.println(productQuantity);
    }
    else{
      Serial.print("No. Product removed: ");
      Serial.print(productTaken);
      Serial.print("| Products Left: ");
      Serial.println(productQuantity);
    }
  }

  // If there is no distance or more than max range
  else if(thisDist == 0 || thisDist >= totalDist){
    productQuantity = 0;
    Serial.println("Please Restock!!!");
    Serial.print("Products Left: ");
    Serial.println(productQuantity);
  }
}

//  Pushes products forwards when product is taken
void pushProducts(int(quantity)){
  if(quantity == 2){
    thisPos = 90;
  }
  else if(quantity == 1){
    thisPos = 120;
  }
  else if(quantity <= 1){
    thisPos = 150;
  }

  // moveServo(prevPos,thisPos);
  servo.write(thisPos);
  Serial.print("Servo Angle: ");
  Serial.println(thisPos);
  // thisPos = prevPos;
  delay(5000);
}

//  Moves Servo Backward for returning
void returnProducts(int(quantity)){
  if(quantity == 2){
    thisPos = 100;
  }
  else if(quantity <= 1){
    thisPos = 130;
  }
  // moveServo(prevPos, thisPos);
  servo.write(thisPos);
  delay(1000);

  while(true){
    valLDR_bottom = analogRead(pinLDR_bottom);
    
    if (valLDR_bottom>=bottomLDR_covered){
      Serial.println("Product has been returned");
      break;
    }
    else{
      Serial.println("Please return product");
      delay(2000);
    }
  }



}

//  Prompts ESP32 to connect to the WiFi
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

//  Reconnects ESP32 to MQTT Broker
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



//  Main Code //
void setup() {
  // Initialize serial communication
  delay(500);
  Serial.begin(115200);

  //  Connecting ESP32 to WiFi
  connectWifi();

  //  Setting up SG90
  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	servo.setPeriodHertz(50);    // standard 50 hz servo
	servo.attach(servoPin, 500, 2400); // attaches the servo on pin 13 to the servo object
  servo.write(startPos);  // Starting Position Every time
  startPos == prevPos;

  //  Setting up Server
  client.setServer(mqttServer,mqttPort);
  client.setKeepAlive(120);
}

void loop() {
  //  Connect ESP32 to Client
  if (!client.connected()){
    reconnect();
  }
  client.loop();

  // Check the quantity of products
  distance = sonar.ping_cm();
  checkQuantity(distance);
  // displayReadings(distance);

  // // If no products on shelf, request for restock 
  // if(productQuantity == 0){
  //   return;
  // }

  // else {
  //   // Check if there is no item at the front/item removed
  //   valLDR_bottom = analogRead(pinLDR_bottom);
  //   if(valLDR_bottom <= bottomLDR_rest){
  //     for (int angle = prevPos; valLDR_bottom <= bottomLDR_covered; angle++ ){
  //       servo.write(angle);
  //       valLDR_bottom = analogRead(pinLDR_bottom);
  //       delay(delayPeriod);
  //     }
  //   }
  // }

  // // Check if Anyone wants to return anything
  // if(valLDR_front >= frontLDR_covered && productQuantity<totalProducts){
  //   Serial.println("User wants to return product...");
  //   returnProducts(productQuantity);
  //   delay(500);
  // }

  //  publish quantity to MQTT Broker
  client.publish(quantityTopic,String(productQuantity).c_str(),true); 


  // // Check the quantity of products
  // distance = sonar.ping_cm();
  // checkQuantity(distance);

  // //  Check if User took out an item
  // if(valLDR_bottom <= bottomLDR_rest){
  //   //  Move Servo to push products
  //   pushProducts(productQuantity);
  // }

  // //  Check if User wants to return product (will re run if more needs to be pushed)
  // else if(valLDR_front >= frontLDR_covered && productQuantity<totalProducts){
  //   Serial.println("User wants to return product...");
  //   returnProducts(productQuantity);
  //   delay(500);
  // }

  // //  publish quantity to MQTT Broker
  // client.publish(quantityTopic,String(productQuantity).c_str(),true);   

  // Wait for a moment before taking another reading
  delay(2000);
}