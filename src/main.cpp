#include <Arduino.h>

#define BTN1 3 // PIN BUTON1 - start charging
#define BTN2 2 // PIN BUTON2 - stop charging

#define BLED 4  // LED RGB - ALBASTRU - (nefolosit in cod)
#define GLED 5  // LED RGB - VERDE - statie libera
#define RLED 6  // LED RGB - ROSU - statie ocupata
#define LED1 7  // LED ALBASTRU - incarcare 25%
#define LED2 8  // LED ALBASTRU - incarcare 50%
#define LED3 9  // LED ALBASTRU - incarcare 75%
#define LED4 10 // LED ALBASTRU - incarcare 100%

// VARIABILE
int button1Status = 0, lastButton1Status = 0; // stari buton1
int button2Status = 0, lastButton2Status = 0; // stari buton2
int isCharging = 0, lastChargeState = 0, stopCharging = 0; // stari incarcare
unsigned long lastDebounceTime1 = 0, debounceDelay = 50, button2Start = 0; // stari debouncing

// functie pentru afisarea statiei libere
void displayFreeStation()
{
  digitalWrite(RLED, LOW); // LED ROSU stins
  digitalWrite(GLED, HIGH); // LED VERDE aprins
}

// functie pentru verificarea butonului de stop incarcare
void checkStopCharging()
{
  // citim starea butonului 2
  button2Status = !digitalRead(BTN2);

  // daca starea s-a schimbat, incepem procesul de debounce
  if (button2Status != lastButton2Status)
  {
    lastDebounceTime1 = millis();
    if (button2Status)
      button2Start = millis();
  }

  if (button2Status && (millis() - button2Start >= 3000))
    stopCharging = 1;

  lastButton2Status = button2Status;
}

// functie pentru incarcare
void charge()
{
  stopCharging = 0; // resetam variabila de stop incarcare
  digitalWrite(GLED, LOW); // LED VERDE stins
  digitalWrite(RLED, HIGH); // LED ROSU aprins

  // iteram prin cele 4 LED-uri albastre pentru animatia blink
  for (int led = LED1; led <= LED4 && !stopCharging; led++)
  { 
    for (int i = 1; i <= 3 && !stopCharging; i++)
    {
      digitalWrite(led, HIGH); // blink aprins

      // folosim un loop pentru a verifica mai des oprirea incarcarii
      for (int j = 0; j < 6 && !stopCharging; j += 1)
      {
        delay(100);
        checkStopCharging();
      }
      digitalWrite(led, LOW); // blink stins

      // folosim un loop pentru a verifica mai des oprirea incarcarii
      for (int j = 0; j < 6 && !stopCharging; j += 1)
      {
        delay(100);
        checkStopCharging();
      }
    }
    digitalWrite(led, HIGH); // blink aprins
  }

  // daca incarcarea nu a fost oprita, facem blink la toate cele 4 LED-uri
  if (!stopCharging)
  {
    for (int i = 0; i < 3 && !stopCharging; i++)
    {
      for (int led = LED1; led <= LED4 && !stopCharging; led++)
        digitalWrite(led, LOW);
      delay(600);
      for (int led = LED1; led <= LED4 && !stopCharging; led++)
        digitalWrite(led, HIGH);
      delay(600);
    }

    // la final afisam ca statia a devenit libera
    displayFreeStation();
  }

  // la final stingem cele 4 LED-uri albastre
  for (int led = LED1; led <= LED4; led++)
    digitalWrite(led, LOW);
}

void setup()
{
  // initializam LED-urile pentru OUTPUT mode
  for (int pin = BLED; pin <= LED4; pin++)
    pinMode(pin, OUTPUT);
  // initializam butoanele pentru INPUT mode
  for (int pin = BTN1; pin <= BTN2; pin++)
    pinMode(pin, INPUT);

  Serial.begin(9600);

  // afisam ca statia este libera
  displayFreeStation();
}

void loop()
{
  // citim starea butonului 1
  button1Status = !digitalRead(BTN1);

  // Serial.println(button1Status);

  // daca starea difera de ultima stare, incepem procesul de deboucing
  if (button1Status != lastButton1Status)
    lastDebounceTime1 = millis();
  if ((millis() - lastDebounceTime1) > debounceDelay)
  {
    if (button1Status)
      isCharging = 1;
    else
      isCharging = 0;
  }
  lastButton1Status = button1Status;

  // daca starea de incarcare este diferita de ultima stare, afisam statia ca fiind libera
  if (!isCharging && lastChargeState)
    displayFreeStation();
  else if (isCharging && !lastChargeState)
    charge();
  lastChargeState = isCharging;
}