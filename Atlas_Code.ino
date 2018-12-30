//#include <HCSR04.h>
#include <SoftwareSerial.h>
//#include <ArduinoBlue.h>
#include <Servo.h>

Servo servo1;
Servo servo2;

int leftFor = 9;
int leftBack = 10;
int rightFor = 6;
int rightBack = 5;

int wheelsServo = 8;
int distServo = 3;

int trigger = 13;
int echo = 12;
long duration = 0;
int distance = 0;
int x = 0;

int SERVO1CENTER = 80;
int SERVO2CENTER = 87;

int startTime = 0;
int stopTime = 0;
        
SoftwareSerial bluetooth(2, 3);
//ArduinoBlue phone(bluetooth);

int prevThrottle = 49;
int prevSteering = 49;

void setup() 
{
  Serial.begin(9600);
  pinMode(leftFor, OUTPUT);
  pinMode(leftBack, OUTPUT);
  pinMode(rightFor, OUTPUT);
  pinMode(rightBack, OUTPUT);
  
  servo1.attach(wheelsServo);
  servo2.attach(distServo);
  
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);

  bluetooth.begin(9600);
  delay(100);

}

void loop() 
{
  
  phone.checkBluetooth();
  int throttle = phone.getThrottle();
  int steering = phone.getSteering();

   if (mySerial.available()) 
   {
     c = mySerial.read();  
     Serial.println(c);

    moveCar(throttle, steering);

    if (prevThrottle != throttle || prevSteering != steering) 
    {
        Serial.print("Throttle: "); Serial.print(throttle); Serial.print("\tSteering: "); Serial.println(steering);
        prevThrottle = throttle;
        prevSteering = steering;
    }

   //smartNavigation();
   
}
  

void moveCar(int throttle, int steering)
{
  if (throttle >= 80 && steering < 40)
  {
    int leftMap = map(steering, 5, 40, 15, 110);
    servo1.write(leftMap);
    moveForward();
  }
  else if (throttle >= 80 && steering > 60)
  {
    int rightMap = map(steering, 60, 99, 120, 220);
    servo1.write(rightMap);
    moveForward();
  }
  else if (throttle < 25 && steering < 40)
  {
    int rightMap = map(steering, 60, 99, 120, 220);
    servo1.write(rightMap);
    moveBackward();
  }
  else if (throttle < 25 && steering > 60)
  {
    int leftMap = map(steering, 5, 40, 15, 110);
    servo1.write(leftMap);
    moveBackward();
  }
  else if (throttle > 90)
  {
    moveForward();
    servo1.write(115);
  }
  else if (throttle < 20)
  {
    moveBackward();
    servo1.write(115);
  }
  else
    fullStop();

}

void moveForward()
{
  analogWrite(leftFor, 255);
  analogWrite(leftBack, 0);
  analogWrite(rightBack, 0);
  analogWrite(rightFor,255);
}

void moveForward(int speed)
{
  analogWrite(leftFor, speed);
  analogWrite(leftBack, 0);
  analogWrite(rightBack, 0);
  analogWrite(rightFor,speed);
}

void moveBackward()
{
  analogWrite(leftFor, 0);
  analogWrite(leftBack, 255);
  analogWrite(rightBack, 255);
  analogWrite(rightFor, 0);
}

void moveBackwardSpeed(int speed)
{
  analogWrite(leftFor, 0);
  analogWrite(leftBack, speed);
  analogWrite(rightBack, speed);
  analogWrite(rightFor, 0);
}

void moveBackward(int setMillis)
{
  if(stopTime - startTime <= setMillis) 
  {
    moveBackwardSpeed(100);
    stopTime=millis();
    //Serial.println(mills);
  }
}

void fullStop()
{
  analogWrite(leftFor, 0);
  analogWrite(leftBack, 0);
  analogWrite(rightBack, 0);
  analogWrite(rightFor, 0);
}

void moveServo(int angle)
{
  servo1.write(angle);
}

void moveServo2(int angle)
{
  servo2.write(angle);
}

void centerServos()
{
  moveServo(SERVO1CENTER);
  moveServo2(SERVO2CENTER);
}

void turnLeft()
{
  moveServo(25);
  moveForward();
}

void turnRight()
{
  moveServo(145);
  moveForward();
}

void lookLeft()
{
  moveServo2(160);
}

void lookRight()
{
  moveServo2(14);
}

void calcDist()
{
    digitalWrite(trigger, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echo, HIGH);
    // Calculating the distance
    distance= duration*0.034/2;
    // Prints the distance on the Serial Monitor
    Serial.print("Distance: ");
    Serial.println(distance);
}

int calcDist2()
{
  int dist = 0;
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echo, HIGH);
  // Calculating the distance
  distance= duration*0.034/2;
  dist = distance;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  return dist;
}

void objectAvoidance()
{
   int dist = calcDist2();
   
   if (dist > 20)
   {
       moveForward();
       centerServos();
   }
   else
   {  
      moveServo(15);   
      moveBackward();   
   } 
}

void smartNavigation()
{
  int dist = calcDist2();
   
   if (dist > 20)
   {
       moveForward();
       centerServos();
   }
   else
   {
      lookLeft();
      int leftDist = calcDist2();
      delay(1000);

      int leftRight[2];
      leftRight[0] = leftDist;
      
      lookRight();
      int rightDist = calcDist2();
      leftRight[1] = rightDist;

      if (leftRight[0] > leftRight[1])
      {
         turnLeft();
      }
      else 
      {
        turnRight();
      }
   } 
}

