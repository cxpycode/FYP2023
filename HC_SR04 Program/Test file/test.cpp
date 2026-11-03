//  Libraries in use  //
#include <Arduino.h>
#include <NewPing.h>
#include <ESP32Servo.h>


//  Bottom(1) & Front(2) LDR Setup  // 
#define pinLDR = 32
int valLDR;



//  SG90 (Servo Motor) Setup  //
#define servoPin 13
Servo servo;
int thisPos;  //current reading of position


// HC-SR04 (Ultrasonic Sensor) Setup  //
#define trigPin 5 
#define echoPin 18
#define maxDist 50 // Set the maximum distance to 50cm
NewPing sonar(trigPin, echoPin, maxDist);

//  Global Varibles (cm)
const float startDist = 2.0;
const float productWidth = 2.4;
const int totalProducts = 4;
const int totalDist = 12;
unsigned int distance;
float difference;
int productQuantity;
int productTaken;



//  Functions //

//  Prints out the distance detected by HC-SR04
void detectDistance(float(thisDist)){
  // Print the distance to the serial monitor
  Serial.print("Distance from back Sensor: ");
  Serial.print(round(distance));
  Serial.println(" cm");
  Serial.println("*********************************************************");
}

//  Checks the quantity of products from distance detected
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

//  Moves Servo Forward when product taken
void moveServo(int(quantity)){
  if(quantity == 4){
    thisPos = 90;
  }
  if(quantity == 3){
    thisPos = 120;
  }
  if(quantity == 2){
    thisPos = 150;
  }
  if(quantity >= 1){
    thisPos = 180;
  }
  servo.write(thisPos);
  Serial.println()
  delay(15);
}

//  Moves Servo Backward for returning
void requestReturn(int(quantity)){
    if(quantity == 4){
        Serial.println("Shelf still full");
        continue;
    }
    if(quantity == 3){
        thisPos = 90;
    }
    if(quantity == 2){
        thisPos = 120;
    }
    if(quantity <= 1){
        thisPos = 150;
    }
    servo.write(thisPos);
    Serial.println()
    delay(15);
}

//  Main Code //
void setup() {
    // Initialize serial communication
    delay(500);
    Serial.begin(115200);

    //  Setting up SG90
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    servo.setPeriodHertz(50);    // standard 50 hz servo
    servo.attach(servoPin, 500, 2400); // attaches the servo on pin 18 to the servo object

}

void loop() {
    // Sensor readings
    distance = sonar.ping_cm();
    valLDR = analogRead(pinLDR);
    Serial.print("Light Sensor Value: ");
    Serial.print(valLDR);
    detectDistance(distance);

    //  Check if User wants to return product
    if(valLDR<100){
        Serial.println("User wants to return product...");
        requestReturn(productQuantity);
    }   
    else{
        //  Check the quantity of products
        checkQuantity(distance);

        //  Move Servo to push products
        moveServo(productQuantity);

        //  publish quantity to MQTT Broker
        client.publish(quantityTopic,String(productQuantity).c_str(),true);

        // Wait for a moment before taking another reading
        delay(2000);
    }
}