/*Every data mentioned is decided based on current situation. It will vary when we decide motors and attachments*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);
const int stepPin = 5;
const int dirPin = 4;
long pulse;
long basbol;
int max_step;
long bolus_insulin;           /*---------------------------------------------------------required variables-----------------------------------------------------*/
long basal_insulin;
long delay_time;
double basal_insulin_ph;
double hours = 24.00;
volatile int flag = false;
unsigned long time_now = 0;
long volume_finished = 0;
char welcome[] = "Welcome to Smart Flow!";

//setup menu
void setup() {
  lcd.init(); //initialize the lcd
  lcd.backlight(); //open the backlight
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(3), bolus_interrupt_flag, RISING);
}


//Main loop for recieving basal or bolus option
void loop()
{
  lcd.setCursor(15, 0);
  for ( int i = 0; i < 26; i++)
  {
    lcd.scrollDisplayLeft();
    lcd.print(welcome[i]);
    delay(350);
  }
  delay(1000);
  basal_function(); //basal rate will provided continuously
  delay(1000);
}

// interrupt menu for bolus interrupt
void bolus_interrupt_flag()
{
  flag = true;
}

//void loop for basal menu
void basal_function()
{
  //Serial.println(flag);     //uncomment to see the value of flag
  lcd.setCursor(0, 0);
  lcd.print("You have entered");
  lcd.setCursor(0, 1);
  lcd.print("basal menu");
  //Serial.println("You have entered basal menu");
  //Serial.println("Enter the basal insulin:");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter the ");
  lcd.setCursor(0, 1);
  lcd.print("basal insulin:");
  Serial.println("Basal -->");
  while (Serial.available() == 0) {}
  basal_insulin = Serial.parseInt();
  //lcd.clear();
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
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Basal set to:");
  lcd.setCursor(0, 1);
  lcd.print(basal_insulin);
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
        //delay(1000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Basal Injected:");
        lcd.setCursor(0, 1);
        lcd.print(count * 0.1);
        //Serial.println("Basal Insulin Units Injected:");
        //Serial.println(count * 0.1);
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
  //Serial.println("");
  //Serial.println("Basal Injection is suspended, it will resume as soon as the bolus injection is provided");
  //Serial.println("");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Basal Suspended");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter the ");
  lcd.setCursor(0, 1);
  lcd.print("bolus insulin:");
  Serial.println("Bolus -->");
  //Serial.println("Enter the bolus insulin:");
  delay(10000);
  while (Serial.available() == 0) {}
  bolus_insulin = Serial.parseInt();
  delay(2000);
  //bolus_insulin = 1;
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
      //Serial.println("Bolus Insulin Units Injected:");
      //Serial.println(count * 0.8);
      delay(delay_time);
    }
  }
}
/*
  void

*/
