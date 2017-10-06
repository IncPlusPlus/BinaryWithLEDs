void displaySetup()
{
  for(int i = START_PIN; i < NUM_LEDS + START_PIN; i++)
  {
    pinMode(i, OUTPUT); //Set each LED pin to an output
  }
}
void displayReset()
{
  for(int i = START_PIN; i < NUM_LEDS + START_PIN; i++)
  {
    digitalWrite(i, LOW); //Set each LED pin to an output
  }
}
