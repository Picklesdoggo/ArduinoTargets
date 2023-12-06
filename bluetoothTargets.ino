#include <FastLED.h>
#include <SoftTimers.h>

#define LED_PIN 13
#define NUM_LEDS 108
#define NUM_TARGETS 5

CRGB leds[NUM_LEDS];
SoftTimer defaultTimer;

struct target
{
  int targetID;
  int sensorPin;
  int sensorReading;
  int threshold;
  int startLED;
  int endLED;
  bool isHit;
};

target targets[NUM_TARGETS]{
  { 0, A0, 0, 350, 7, 15, false },
  { 1, A4, 0, 350, 27, 35, false },
  { 2, A8, 0, 350, 49, 57, false },
  { 3, A12, 0, 350, 72, 80, false },
  { 4, A1, 0, 350, 95, 103, false }
};

int hitCount = 0;

void changeLED(int led, int red, int green, int blue)
{
  leds[led] = CRGB(red, green, blue);
}

void turnAllLEDsOff()
{
  // turn all off
  for (int i = 0; i <= NUM_LEDS; i++)
  {
    changeLED(i, 0, 0, 0);
    FastLED.show();
  }
}

void initialLEDSetup(String color)
{

  for (int i = 0; i < NUM_TARGETS; i++)
  {
    targets[i].isHit = false;
    for (int ii = targets[i].startLED; ii <= targets[i].endLED; ii++)
    {

      if (color == "b")
      {
        changeLED(ii, 0, 0, 255);
      }
      else if (color == "r")
      {
        changeLED(ii, 255, 0, 0);
      }
      else if (color == "g")
      {
        changeLED(ii, 0, 255, 0);
      }
      else if (color == "o")
      {
        changeLED(ii, 0, 0, 0);
      }

      //delay(100);
      FastLED.show();
    }
  }
}

void targetHit(target hitTarget)
{
  Serial.print("Target ");
  Serial.print(hitTarget.targetID);
  Serial.print(" : ");
  Serial.println(hitTarget.sensorReading);

  hitCount++;
  for (int i = hitTarget.startLED; i <= hitTarget.endLED; i++)
  {
    changeLED(i, 0, 255, 0);
    //delay(100);
    FastLED.show();
  }
}

void setup()
{

  // setup LEDs
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(10);
  turnAllLEDsOff();
  //
  Serial.begin(9600);  // use the serial port
  defaultTimer.setTimeOutTime(5000);
  defaultTimer.reset();
}

void allTargets()
{
  initialLEDSetup("b");
  while (true)
  {
    for (int i = 0; i < 5; i++)
    {
      if (targets[i].isHit == false)
      {
        targets[i].sensorReading = analogRead(targets[i].sensorPin);
        if (targets[i].sensorReading > targets[i].threshold)
        {
          targetHit(targets[i]);
          targets[i].isHit = true;
        }
      }
    }

    if (hitCount == NUM_TARGETS)
    {
      Serial.println("WINNER");
      hitCount = 0;
      delay(3000);
      initialLEDSetup("b");
    }

    String data = Serial.readString();  //read until timeout
    data.trim();                        // remove any \r \n whitespace at the end of the String

    if (data == "r")
    {
      String targetStatus = "";
      for (int i = 0; i < NUM_TARGETS; i++)
      {
        targetStatus = targetStatus + targets[i].isHit;
      }
      Serial.println(targetStatus);
    }
  }
}

void randomTarget()
{
  while (true)
  {
    randomSeed(analogRead(A1));
    // pick a random target
    int targetNumber = random(0, NUM_TARGETS);

    // Make sure it is not hit
    while (targets[targetNumber].isHit)
    {
      targetNumber = random(0, NUM_TARGETS);
    }

    // Light up chosen target

    for (int i = targets[targetNumber].startLED; i <= targets[targetNumber].endLED; i++)
    {
      changeLED(i, 0, 0, 255);
      //delay(100);
      FastLED.show();
    }

    // Wait until it gets hit
    while (!targets[targetNumber].isHit)
    {
      targets[targetNumber].sensorReading = analogRead(targets[targetNumber].sensorPin);
      if (targets[targetNumber].sensorReading > targets[targetNumber].threshold)
      {
        targetHit(targets[targetNumber]);
        targets[targetNumber].isHit = true;
      }
    }

    // Have all targets been hit
    if (hitCount == NUM_TARGETS)
    {
      Serial.println("WINNER");
      hitCount = 0;
      delay(3000);
      initialLEDSetup("o");
    }
  }
}

void loop()
{
  String mode;
  if (!defaultTimer.hasTimedOut())
  {
    mode = Serial.readString();  //read until timeout
    mode.trim();                        // remove any \r \n whitespace at the end of the String
  }
  else
  {
    mode = "2";
  }

  if (mode == "1")
  {
    allTargets();
  }
  else if (mode == "2")
  {
    randomTarget();
  }
}
