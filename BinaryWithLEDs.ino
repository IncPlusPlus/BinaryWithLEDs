#include "Settings.h"
#include <math.h>

/*
 * Enable to slow program steps and get detailed output through serial.
 */
boolean debug = false;
int NUM_LEDS = 8;       //Total number of LEDs connected to pins
/*
 * START_PIN is what pin to count up from when connecting LEDs.
 * START_PIN will be the MOST significant bit.
 * This number will be counted up to from the most significant bit to the least.
 * Order your pins so that the pin number count is inversely proportional to the bit significance.
 */
int START_PIN = 2;
/*
 * Until calibration is implemented, this is a messy workaround due to the imperfections of potentiometers.
 */
int LOWEST_POTENTIOMETER_READING = 40;
int POTENTIOMETER_PIN = 0;
int serialTimeoutCounter = NUM_LEDS;
boolean timedOut = false;
boolean calibrationMessageDelivered = false;
void setup()
{
  Serial.begin(9600); //Open serial monitor with a baud rate of 9600
  displaySetup();
  Serial.println("Welcome to the decimal to binary converter.\nType a number to display it in binary or wait to use the dial.\n");
  delay(750);
  pinMode(10, INPUT_PULLUP);
}

void loop()
{
  while(!Serial.available())
  {
    if(!timedOut)
    {
      if(serialTimeoutCounter == NUM_LEDS)
      {
        for(int i = START_PIN; i < NUM_LEDS + START_PIN; i++)
        {
          digitalWrite(i, HIGH);
          delay(200);
        }
      }
      Serial.print("Timing out and using potentiometer in ");
      Serial.print(serialTimeoutCounter);
      Serial.println(" seconds...");
      delay(1000);
      digitalWrite(serialTimeoutCounter +   1, LOW);
      serialTimeoutCounter--;
      if(serialTimeoutCounter == 0)
      {
        potentiometer();
        break;
      }
    }
  }
  timedOut = true;
  displayReset();
  int workingNum = 0;
  workingNum = (int)Serial.parseInt();
  if(workingNum == 0)
  {
    menu();
  }
  else if(workingNum < 0)
  {
    Serial.println("The number you entered is either too big or negative.");
    Serial.println("Please enter a number that is neither of these.");
    workingNum = 0;
  }
  else if(NUM_LEDS < floor(log10(workingNum)/log10(2) + 1))
  {
    Serial.print("Not enough LEDs to represent ");
    Serial.print(workingNum);
    Serial.println(".\nTry another number.");
  }
  else
  {
    outputBinaryNumber(workingNum);
  }

}
void menu()
{
  Serial.println("[1] Clear\n[2] Use Potentiometer\n[3] Cycle");
  while(!Serial.available())
  {
  }
  switch(Serial.parseInt())
  {
    case 1: clearSerial();
    break;
    case 2: potentiometer();
    break;
    case 3: cycle();
    break;
  }
}
void potentiometer()
{
  while(!Serial.available())
  {
    calibrationMessageDelivered = false;
    while(digitalRead(10) == 0)
    {
      if(!calibrationMessageDelivered)
      {
        Serial.println("Calibrating. Turn potentiometer down all the way, let go, and flip switch back");
        calibrationMessageDelivered = true;
      }
      LOWEST_POTENTIOMETER_READING = analogRead(POTENTIOMETER_PIN);
      for(int i = START_PIN; i < NUM_LEDS + START_PIN; i++)
      {
        digitalWrite(i, !digitalRead(i)); //Set each LED pin to an output
      }
      delay(30);
    }
    int potenVal = analogRead(POTENTIOMETER_PIN);
    Serial.print("Reading: ");
    Serial.println(potenVal);
    int mappedVal = map(potenVal,LOWEST_POTENTIOMETER_READING, 1023, 0, pow(2, NUM_LEDS)-1);
    Serial.print("Mapped to: ");
    Serial.println(mappedVal);
    int constrainedVal = constrain(mappedVal, 0, 1023);
    Serial.print("Sending: ");
    Serial.println(constrainedVal);
    Serial.println("\n");
    outputBinaryNumber(constrainedVal);
    delay(50);
  }
}
void cycle()
{
  for(int i = 0; NUM_LEDS >= floor(log10(i)/log10(2) + 1); i++)
  {
    if(Serial.available())
    {
      break;
    }
    Serial.print("Outputting: ");
    Serial.println(i);
    outputBinaryNumber(i);
    delay(250);
  }
}
void outputBinaryNumber(int num)
{
  int workingLED = START_PIN + NUM_LEDS -1;
  int currentNum = num;
  displayReset();
  if(debug)
  {
    delay(500);
    Serial.println("Beginning while loop");
  }
  while(currentNum != 0)
  {
    currentNum % 2 == 0 ? digitalWrite(workingLED, LOW) : digitalWrite(workingLED, HIGH);
    if(debug)
    {
      Serial.print(currentNum);
      Serial.print(" % 2 = ");
      Serial.println(currentNum % 2);
      delay(3000);
    }
    currentNum /= 2;
    if(debug)
    {
      Serial.print("currentNum is now: ");
      Serial.println(currentNum);
      delay(3000);
    }
    workingLED--;
    if(debug)
    {
      Serial.print("workingLED is now: ");
      Serial.println(workingLED);
      delay(3000);
      Serial.println("\nLOOPING\n");
      delay(2000);
    }
  }
  if(debug)
  {
    Serial.println("END OF WHILE LOOP");
    delay(10000);    
  }
}
void clearSerial()
{
  Serial.println("Clearing");
  delay(3000);
  for(int i = 0; i < 200; i++)
  {    
    Serial.println();
  }
}
