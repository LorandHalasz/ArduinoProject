#include <Servo.h>

// Pinii motor 1 
#define mpin00 6 
#define mpin01 5 
// Pinii motor 2 
#define mpin10 11 
#define mpin11 10 
// Pinii pentru senzor ultrasonic
#define trigPin 9
#define echoPin 12
Servo srv;

float value=0;
volatile int rev1=0, rev2=0;
float rpm1 = 0, rpm2 = 0;
int oldtime=0;
int times;
int speed1 = 70, speed2 = 70;
int ok = 0;
 
void isr1() //interrupt service routine
{
  rev1++;
}
void isr2() //interrupt service routine
{
  rev2++;
}
 
void setup()
{
  digitalWrite(mpin00, 0); 
  digitalWrite(mpin01, 0); 
  digitalWrite(mpin10, 0); 
  digitalWrite(mpin11, 0); 
 
  pinMode(2 ,INPUT);
  pinMode(3, INPUT);  
 
  digitalWrite(2, HIGH); 
  digitalWrite(3, HIGH);
 
  pinMode (mpin00, OUTPUT); 
  pinMode (mpin01, OUTPUT); 
  pinMode (mpin10, OUTPUT); 
  pinMode (mpin11, OUTPUT); 
 
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), isr1, RISING);  //attaching the interrupt
  attachInterrupt(digitalPinToInterrupt(3), isr2, RISING);  //attaching the interrupt

  while((ok == 0 || ((int)(rpm1 * 100) != (int)(rpm2 * 100)))){
    ok = 1;
    StartMotor(mpin00, mpin01, 1, speed1);
    StartMotor(mpin10, mpin11, 1, speed2);
    calcRpm();

    if(rpm1 > rpm2)
      speed2 += 10;
    else
      speed1 += 10;

    Serial.print("Speed1 ");
    Serial.println(speed1);
    Serial.print("Speed2 ");
    Serial.println(speed2);
    Serial.println();
  }

  // senzor ultrasonic
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}
 
void StartMotor (int m1, int m2, int forward, int speed) 
{ 
  if (speed==0) 
  { 
    digitalWrite(m1, 0); 
    digitalWrite(m2, 0); 
  } 
  else  
  { 
    if (forward) 
    { 
      digitalWrite(m2, 0); 
      analogWrite (m1, speed); 
    } 
    else 
    { 
      digitalWrite(m1, 0); 
      analogWrite(m2, speed); 
    } 
  } 
} 
 
void calcRpm(){
  delay(1000);
  detachInterrupt(digitalPinToInterrupt(2));           //detaches the interrupt
  detachInterrupt(digitalPinToInterrupt(3));           //detaches the interrupt

  times = millis() - oldtime;        //finds the time 
  rpm1 = (float)rev1 / times;         //calculates rpm
  rpm2 = (float)rev2 / times;         //calculates rpm
  oldtime = millis();             //saves the current time

  Serial.print("rpm1 ");
  Serial.println(rpm1);
  Serial.print("rpm2 ");
  Serial.println(rpm2);
  Serial.println();
  
  rev1 = 0;
  rev2 = 0;
  
  attachInterrupt(digitalPinToInterrupt(2), isr1, RISING);
  attachInterrupt(digitalPinToInterrupt(3), isr2, RISING);
}
 
void loop()
{
  StartMotor(mpin00, mpin01, 1, speed1);
  StartMotor(mpin10, mpin11, 1, speed2);

  delay(1000);

  playWithServo(13);

  StartMotor(mpin00, mpin01, 1, 0);
  StartMotor(mpin10, mpin11, 1, 0);
  delay(500);
}

void playWithServo(int pin)
{
  double distFront, distLeft, distRight;
  srv.attach(pin);
  srv.write(90);
  distFront = calcDist();
  

  Serial.println();
  Serial.println(distFront);
  Serial.println();
  if(distFront < 30)
  {
    StartMotor(mpin00, mpin01, 1, 0);
    StartMotor(mpin10, mpin11, 1, 0);
    srv.write(0);
    delay(1000);
    distRight = calcDist();
    srv.write(180);
    delay(1000);

    distLeft = calcDist();
    srv.write(90);
    delay(1000);
    
    Serial.print("distLeft ");
    Serial.println(distLeft);
    Serial.print("distRight ");
    Serial.println(distRight);
    if(distLeft > distRight )
    {
           StartMotor(mpin00, mpin01, 0, 0);
      StartMotor(mpin10, mpin11, 1, speed2); 

      delay(1000);
      delayStopped(200);
    }
    else
    {

StartMotor(mpin00, mpin01, 1, speed1);
      StartMotor(mpin10, mpin11, 0, 0);
      delay(1000);
      delayStopped(200);
    }

  }
  srv.detach();
}

double calcDist() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  double duration = pulseIn(echoPin, HIGH);
  
  // Calculating the distance
  double distance = duration * 0.034 / 2;
  
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  return distance;
}

void delayStopped(int ms)
{
 StartMotor (mpin00, mpin01, 0, 0);
 StartMotor (mpin10, mpin11, 0, 0);
 delay(ms);
} 
