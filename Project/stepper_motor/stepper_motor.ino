#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);

const int stepPin = 6;
const int dirPin = 4;
long pulse;
int max_step;
long bolus_insulin;           /*---------------------------------------------------------required variables-----------------------------------------------------*/
long basal_insulin;
long delay_time;
double basal_insulin_ph;
double hours = 24.00;
volatile int flag = false;
unsigned long time_now = 0;
long volume_finished = 0;


//Input & Button Logic
const int numOfInputs = 4;
const int inputPins[numOfInputs] = {8,9,10,11};
int inputState[numOfInputs];
int lastInputState[numOfInputs] = {LOW,LOW,LOW,LOW};
bool inputFlags[numOfInputs] = {LOW,LOW,LOW,LOW};
long lastDebounceTime[numOfInputs] = {0,0,0,0};
long debounceDelay = 5;
volatile int set_flag = false;

//LCD Menu Logic
const int numOfScreens = 2;
int currentScreen = 0;
String screens[numOfScreens][2] = {{"Basal Menu","Units"}, {"Bolus Menu", "Units"}};
int parameters[numOfScreens];
char welcome[] = ("Welcome to Smart Flow!                ");

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(15, 0);
  for ( int i = 0; i < 37; i++)
  {
    lcd.scrollDisplayLeft();
    lcd.print(welcome[i]);
    delay(350);
  }
  for(int i = 0; i < numOfInputs; i++) {
    pinMode(inputPins[i], INPUT);
    digitalWrite(inputPins[i], HIGH); // pull-up 20k
  }
   attachInterrupt(digitalPinToInterrupt(3), set_value, RISING);
   attachInterrupt(digitalPinToInterrupt(2), bolus_interrupt_flag, RISING);
  //Serial.begin(9600);
}

void loop() {
  if (set_flag == false)
  {
  setInputFlags();
  resolveInputFlags();
  }
  else
  {
    basal_function();
  }
}

void set_value(){
  set_flag = true;
}

void setInputFlags() {
  for(int i = 0; i < numOfInputs; i++) {
    int reading = digitalRead(inputPins[i]);
    if (reading != lastInputState[i]) {
      lastDebounceTime[i] = millis();
    }
    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (reading != inputState[i]) {
        inputState[i] = reading;
        if (inputState[i] == HIGH) {
          inputFlags[i] = HIGH;
        }
      }
    }
    lastInputState[i] = reading;
  }
}

void resolveInputFlags() {
  for(int i = 0; i < numOfInputs; i++) {
    if(inputFlags[i] == HIGH) {
      inputAction(i);
      inputFlags[i] = LOW;
      printScreen();
    }
  }
}

void inputAction(int input) {
  if(input == 0) {
    if (currentScreen == 0) {
      currentScreen = numOfScreens-1;
    }else{
      currentScreen--;
    }
  }else if(input == 1) {
    if (currentScreen == numOfScreens-1) {
      currentScreen = 0;
    }else{
      currentScreen++;
    }
  }else if(input == 2) {
    parameterChange(0);
  }else if(input == 3) {
    parameterChange(1);
  }
}

void parameterChange(int key) {
  if(key == 0) {
    parameters[currentScreen]++;
  }else if(key == 1) {
    parameters[currentScreen]--;
  }
}

void printScreen() {
  lcd.clear();
  lcd.print(screens[currentScreen][0]);
  lcd.setCursor(0,1);
  lcd.print(parameters[currentScreen]);
  lcd.print(" ");
  lcd.print(screens[currentScreen][1]);
}
// interrupt menu for bolus interrupt

void bolus_interrupt_flag()
{
  flag = true;
}

//void loop for basal menu
void basal_function()
{
  basal_insulin = parameters[0];
  lcd.clear();
  lcd.print("Basal Set to:");
  lcd.setCursor(0,1);
  lcd.print(basal_insulin);
  lcd.print("");
  lcd.print("Units");
  if (basal_insulin <= 10)
  {
    basal_insulin_ph = basal_insulin / hours;                     //diving total total insulin units for per hour basal delievery
    pulse = 64;                                                //taking pulse as 64 ie., 32 steps per revolution for bolus injection
    max_step  = basal_insulin_ph / 0.1;                        //considering it can give 0.1 units per step
    delay_time = 3500;                                         //for example alone. its not for final
  }
  else if (basal_insulin <= 20 && basal_insulin > 10)
  {
    basal_insulin_ph = basal_insulin / hours;                     //diving total total insulin units for per hour basal delievery
    pulse = 128;                                               //taking pulse as 128 ie., 16 steps per revolution for bolus injection
    max_step = basal_insulin_ph / 0.2;                         //considering it can give 0.2 units per step
    delay_time = 3000;                                         //for example alone. its not for final
  }
  else if (basal_insulin <= 30 && basal_insulin > 20)
  {
    basal_insulin_ph = basal_insulin / hours;                     //diving total total insulin units for per hour basal delievery
    pulse = 256;                                               //taking pulse as 256 ie., 8 steps per revolution for bolus injection
    max_step = basal_insulin_ph / 0.5;                         //considering it can give 0.5 units per step
    delay_time = 2500;                                         //for example alone. its not for final
  }
  else if (basal_insulin > 30)
  {
    basal_insulin_ph = basal_insulin / hours;                     //diving total total insulin units for per hour basal delievery
    pulse = 512;                                               //taking pulse as 512 ie., 5 steps per revolution for bolus injection
    max_step = basal_insulin_ph / 0.8;                         //considering it can give 0.5 units per step
    delay_time = 2000;                                         //for example alone. its not for final
  }
  pulse_function_basal();                                      // this will call the pulse_function_basal() function
}


//pulse function based on different basal levels
void pulse_function_basal()
{
  int i;
  for (i = 0; i <= 23 ; i++)
  {
    int count = 0;
    if (pulse == 64)
    {
      while (count < max_step)
      {
        digitalWrite(dirPin, HIGH);
        for (int x = 0; x < pulse; x++)
        {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(500);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(500);
        }
        count++;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Basal Injected:");
        lcd.setCursor(0, 1);
        lcd.print(count * 0.1);
        lcd.setCursor(5,1);
        lcd.print("Units");
        delay(delay_time);
        if (flag == true)
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Wait till ");
          lcd.setCursor(0, 1);
          lcd.print("basal finishes");
          delay(200);
        }
      }
      time_now = millis();
      while (millis() < time_now  + (3600000 - max_step * (1 + delay_time)))
      {
        if (flag == true)
        {
          delay(1000);
          bolus_function();
          flag = false;
          delay(1000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Bolus done");
          delay(500);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Basal Resumed");
          //Serial.println("Basal resumed");
          delay(500);
        }
      }
      //delay(3600000-max_step*(1+delay_time));
    }
    else if (pulse == 128)
    {
      while (count < max_step)
      {

        digitalWrite(dirPin, HIGH);
        for (int x = 0; x < pulse; x++)
        {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(500);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(500);
        }
        count++;
        //delay(1000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Basal Injected:");
        lcd.setCursor(0, 1);
        lcd.print(count * 0.2);
        lcd.setCursor(5,1);
        lcd.print("Units");
        //Serial.println("Basal Insulin Units Injected:");
        //Serial.println(count * 0.2);
        delay(delay_time);
        if (flag == true)
        {
          //delay(300);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Wait till ");
          lcd.setCursor(0, 1);
          lcd.print("basal finishes");
          delay(500);
          //Serial.println("");
          //Serial.println("Wait till the basal finishes");
          //Serial.println("");
        }
      }
      time_now = millis();
      while (millis() < time_now  + (3600000 - max_step * (1 + delay_time)))
      {
        if (flag == true)
        {
          delay(1000);
          bolus_function();
          flag = false;
          delay(1000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Bolus done");
          //Serial.println("Bolus Done");
          delay(1000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Basal Resumed");
          //Serial.println("Basal resumed");
          delay(1000);
        }
      }
      //delay(3600000-max_step*(1+delay_time));
    }
    else if (pulse == 256)
    {
      while (count < max_step)
      {

        digitalWrite(dirPin, HIGH);
        for (int x = 0; x < pulse; x++)
        {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(500);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(500);
        } Serial.println("Current Pulse Rate:");
        Serial.println(pulse);
        count++;
        //delay(1000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Basal Injected:");
        lcd.setCursor(0, 1);
        lcd.print(count * 0.5);
        lcd.setCursor(5,1);
        lcd.print("Units");
        //Serial.println("Basal Insulin Units Injected:");
        //Serial.println(count * 0.5);
        delay(delay_time);
        if (flag == true)
        {
          //delay(300);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Wait till ");
          lcd.setCursor(0, 1);
          lcd.print("basal finishes");
          delay(500);
          //Serial.println("");
          //Serial.println("Wait till the basal finishes");
          //Serial.println("");
        }
      }
      time_now = millis();
      while (millis() < time_now  + (3600000 - max_step * (1 + delay_time)))
      {
        if (flag == true)
        {
          delay(1000);
          bolus_function();
          flag = false;
          delay(1000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Bolus done");
          //Serial.println("Bolus Done");
          delay(1000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Basal Resumed");
          //Serial.println("Basal resumed");
          delay(1000);
        }
      }
      //delay(3600000-max_step*(1+delay_time));
    }
    else if (pulse == 512)
    {
      while (count < max_step)
      {
        digitalWrite(dirPin, HIGH);
        for (int x = 0; x < pulse; x++)
        {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(500);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(500);
        }
        count++;
        //delay(1000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Basal Injected:");
        lcd.setCursor(0, 1);
        lcd.print(count * 0.8);
        lcd.setCursor(5,1);
        lcd.print("Units");
        //Serial.println("Basal Insulin Units Injected:");
        //Serial.println(count * 0.8);
        delay(delay_time);
        if (flag == true)
        {
          //delay(300);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Wait till ");
          lcd.setCursor(0, 1);
          lcd.print("basal finishes");
          delay(500);
          //Serial.println("");
          //Serial.println("Wait till the basal finishes");
          //Serial.println("");
        }
      }
      time_now = millis();
      while (millis() < time_now  + (3600000 - max_step * (1 + delay_time)))
      {
        if (flag == true)
        {
          delay(2000);
          bolus_function();
          flag = false;
          delay(1000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Bolus done");
          //Serial.println("Bolus Done");
          delay(1000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Basal Resumed");
          //Serial.println("Basal resumed");
          delay(1000);
        }
      }
      //delay(3600000-max_step*(1+delay_time));
    }
  }
}


//void loop for bolus menu
void bolus_function()
{
  bolus_insulin = parameters[1];
  lcd.clear();
  lcd.print("Bolus Set to:");
  lcd.setCursor(0,1);
  lcd.print(bolus_insulin);
  lcd.print("");
  lcd.print("Units");
  if (bolus_insulin <= 10)
  {
    pulse = 128;                                               //taking pulse as 256 ie., 16 steps per revolution for bolus injection
    max_step  = bolus_insulin / 0.2;                           //considering it can give 0.2 units per step
    delay_time = 3000;                                         //for example alone, its not for final
  }
  else if (bolus_insulin <= 15 && bolus_insulin > 10)
  {
    pulse = 256;                                                 //taking pulse as 256 ie., 8 steps per revolution for bolus injection
    max_step = bolus_insulin / 0.5;                              //considering it can give 0.5 units per step
    delay_time = 1500;                                           //for example alone. its not for final
  }
  else if (bolus_insulin <= 20 && bolus_insulin > 15)
  {
    pulse = 512;                                                 //taking pulse as 256 ie., 8 steps per revolution for bolus injection
    max_step = bolus_insulin / 0.8;                              //considering it can give 0.5 units per step
    delay_time = 1000;                                           //for example alone. its not for final
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bolus set to:");
  lcd.setCursor(0, 1);
  lcd.print(bolus_insulin);
  pulse_function_bolus();
}




//pulse function based on different bolus level
void pulse_function_bolus()
{
  int count = 0;

  if (pulse == 128)
  {
    while (count < max_step)
    {
      digitalWrite(dirPin, HIGH);
      for (int x = 0; x < pulse; x++)
      {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(500);
      }
      count++;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bolus Injected");
      lcd.setCursor(0, 1);
      lcd.print(count * 0.2);
      lcd.setCursor(5,1);
      lcd.print("Units");
      //Serial.println("Bolus Insulin Units Injected:");
      //Serial.println(count * 0.2);
      delay(delay_time);
    }
  }
  else if (pulse == 256)
  {
    while (count < max_step)
    {
      digitalWrite(dirPin, HIGH);
      for (int x = 0; x < pulse; x++)
      {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(500);
      }
      count++;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bolus Injected");
      lcd.setCursor(0, 1);
      lcd.print(count * 0.5);
      lcd.setCursor(5,1);
      lcd.print("Units");
      //Serial.println("Bolus Insulin Units Injected:");
      //Serial.println(count * 0.5);
      delay(delay_time);
    }
  }
  else if (pulse == 512)
  {
    while (count < max_step)
    {
      digitalWrite(dirPin, HIGH);
      for (int x = 0; x < pulse; x++)
      {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(500);
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bolus Injected");
      lcd.setCursor(0, 1);
      lcd.print(count * 0.8);
      lcd.setCursor(5,1);
      lcd.print("Units");
      //Serial.println("Bolus Insulin Units Injected:");
      //Serial.println(count * 0.8);
      delay(delay_time);
    }
  }
}
