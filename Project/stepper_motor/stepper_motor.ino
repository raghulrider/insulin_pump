/* Welcome to Smart Flow - Smart Flow is an Insulin Pump, an attempt ot build an insulin pump at low cost making it affordable for common people.
   I'm Raghul Prasad Venkatachalam 3rd Year student studying Electronics and Communication in Kumaraguru College of Technology.
   We are a group of 4 students currently training as Graduate Engineer @Forge under a program named ProtoSem situated @Coimbatore
   Innovation/Startup fellowship program is a 20-week, full-semester course called ProtoSem during which students are nurtured & guided
   to develop core technical concepts and key engineering skills and are transformed into 'Innovation Engineers'.
   We work as a team with students from different engineering/technical/science streams, towards designing, developing
   and testing an innovative tech-enabled solution to solve a real-world problem
   (the Minimum Usable Prototype to test the technical feasibility and the commercial viability of the product innovation) sponsored by the industry, government, or social sector etc.
   We solve real-world challenges using the tools enabled by hardware, software and computing technologies,
   and evolve as Graduate Innovation Engineers and fully rounded professionals with more balanced skills in the Innovation, Technology, and Engineering talent dimensions.
   Graduate Innovation Engineers are creative problem-solvers, capable of building deployable/usable prototypes, selecting and applying technology to create cost-effective solutions,
   and are aware of the tools and techniques of managing the process of innovation to mitigate and reduce risks.
   They innovate and design solutions by setting and achieving metrics in
   customer development & discovery, value-proposition validation, market definition & sizing, customer acquisition, revenue model & competition analysis. */

/***********************************************************Library Section - Required Libraries will be entered in this section***********************************************************/


#include <LiquidCrystal_I2C.h>
#include <DS3231.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>

/***********************************************************Variable Section - Global and other initialization will be mentioned here***********************************************************/
DS3231  rtc(SDA, SCL);
LiquidCrystal_I2C lcd(0x3F, 16, 2);

const int stepPin = 5;
const int dirPin = 4;
long pulse, pulse_bol;
int max_step, max_step_bol;
int bolus_insulin, basal_insulin;
long delay_time, delay_time_bol;
double basal_insulin_ph;
volatile int flag = false;
unsigned long time_now = 0;
int volume_completed = 0;
int count = 0, count_bol = 0;
const int beep = 7;
const int numOfInputs = 4;
const int inputPins[numOfInputs] = {8, 9, 0, 1};
int inputState[numOfInputs];
int lastInputState[numOfInputs] = {LOW, LOW, LOW, LOW};
bool inputFlags[numOfInputs] = {LOW, LOW, LOW, LOW};
long lastDebounceTime[numOfInputs] = {0, 0, 0, 0};
long debounceDelay = 5;
volatile int set_flag = false;
const int numOfScreens = 2;
int currentScreen = 0;
String screens[numOfScreens][2] = {{"Basal Menu", "Units"}, {"Bolus Menu", "Units"}};
int parameters[numOfScreens];


/*********************************************************************** Setup section **********************************************************************************************/
void setup() {
  pinMode(beep, OUTPUT);                //alarm output
  lcd.init();                           //LCD display initialization
  lcd.backlight();                      //Enabling Backight comment this line if you dont want backight
  rtc.begin();                          //RTC begin
  //rtc.setDOW(MONDAY);                 //Set Day. This is set at time of doing my project. Change it according to your current data and Time.
  //rtc.setTime(15, 49, 30);            //Set Hour-Minutes-Seconds(24hr format)
  //rtc.setDate(4, 9, 2019);            //Set Month-Date-Year
  lcd.setCursor(15, 0);                 //See basics of LCD programming to understand what it is.
  char welcome[] = ("Welcome to Smart Flow!                ");
  for ( int i = 0; i < 37; i++)
  {
    lcd.scrollDisplayLeft();             //Welcome message
    lcd.print(welcome[i]);
    delay(350);
  }
  for (int i = 0; i < numOfInputs; i++) {
    pinMode(inputPins[i], INPUT);                //Declaring input pins for navigation and increment/decrement buttons
    digitalWrite(inputPins[i], HIGH);            // pull-up 20k
  }
  attachInterrupt(digitalPinToInterrupt(2), set_value, RISING);                 //Interrupt for come out from data entry page
  attachInterrupt(digitalPinToInterrupt(3), bolus_interrupt_flag, RISING);      //This Interrupt is to enter bolus as well as act as confirm in many cases
  Serial.begin(9600);                            // Serial begin
}

/********************************************************Loop section - Code Inside this run Continuosly till power is down**********************************************************/
void loop() {
  if (set_flag == false)        /* If section runs till Interrupt in the Digital pin 2 is triggered. After the interrupt else section will execute ie. Pump will start providing insulin at basal Rate */
  {
    setInputFlags();                     //calls setInputFlags function
    resolveInputFlags();                 //calls resolveInputFlags
  }
  else
  {
    basal_function();                    //calls basal function
  }
}

/*******************************************************************set_value function - User Created Function************************************************************************/
void set_value() {
  set_flag = true;                       //sets the set_flag to HIGH when the Interrupt at pin 2 is triggered
}


// interrupt menu for bolus interrupt

void bolus_interrupt_flag()
{
  flag = true;
}

//void loop for basal menu
void basal_function()
{
  Serial.println("Basal Started at:");
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");
  Serial.println(rtc.getTimeStr());
  flag = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press again");
  lcd.setCursor(0, 1);
  lcd.print("to Confirm");
  while ( flag == false) {
  }
  flag = false;
  lcd.clear();
  lcd.print("Basal Set to:");
  lcd.setCursor(0, 1);
  lcd.print(parameters[0]);
  lcd.print(" ");
  lcd.print("Units");
  delay(1500);
  const double hours = 24.00;
  basal_insulin = parameters[0];
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

    if (pulse == 64)
    {
      while (count < max_step)
      {
        if (volume_completed != 300)
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
          lcd.setCursor(5, 1);
          lcd.print("Units");
          delay(delay_time);
          if (flag == true)
          {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Wait till ");
            lcd.setCursor(0, 1);
            lcd.print("basal finishes");
            flag = false;
            delay(1000);
          }
        }
        else if (volume_completed >= 300)
        {
          reservoir_completed();
        }

      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Basal Injected:");
      lcd.setCursor(0, 1);
      lcd.print(count * 0.1);
      lcd.setCursor(5, 1);
      lcd.print("Units");
      time_now = millis();
      while (millis() < time_now  + (3600000 - max_step * (1 + delay_time)))
      {
        if (flag == true)
        {
          delay(200);
          flag = false;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Are you sure, ");
          lcd.setCursor(0, 1);
          lcd.print("suspend basal?");
          while (flag == false) {
          }
          delay(1000);
          flag = false;
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
        if (volume_completed != 300)
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
          lcd.setCursor(5, 1);
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
            delay(1000);
            flag = false;
            //Serial.println("");
            //Serial.println("Wait till the basal finishes");
            //Serial.println("");
          }
        }
        else if (volume_completed >= 300)
        {
          reservoir_completed();
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
        if (volume_completed != 300)
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
          lcd.setCursor(5, 1);
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
            delay(1000);
            flag = false;
            //Serial.println("");
            //Serial.println("Wait till the basal finishes");
            //Serial.println("");
          }
        }
        else if (volume_completed >= 300)
        {
          reservoir_completed();
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
        if (volume_completed != 300)
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
          lcd.setCursor(5, 1);
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
            delay(1000);
            flag  = false;
            //Serial.println("");
            //Serial.println("Wait till the basal finishes");
            //Serial.println("");
          }
        }
        else if (volume_completed >= 300)
        {
          reservoir_completed();
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
  Serial.println("Bolus Started at:");
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");
  Serial.println(rtc.getTimeStr());
  bolus_insulin = parameters[1];
  lcd.clear();
  lcd.print("Basal Suspended");
  delay(1000);
  lcd.clear();
  lcd.print("Bolus Set to:");
  lcd.setCursor(0, 1);
  lcd.print(bolus_insulin);
  lcd.print(" ");
  lcd.print("Units");
  delay(2000);
  if (bolus_insulin <= 10)
  {
    pulse_bol = 128;                                               //taking pulse as 256 ie., 16 steps per revolution for bolus injection
    max_step_bol  = bolus_insulin / 0.2;                           //considering it can give 0.2 units per step
    delay_time_bol = 3000;                                         //for example alone, its not for final
  }
  else if (bolus_insulin <= 15 && bolus_insulin > 10)
  {
    pulse_bol = 256;                                                 //taking pulse as 256 ie., 8 steps per revolution for bolus injection
    max_step_bol = bolus_insulin / 0.5;                              //considering it can give 0.5 units per step
    delay_time_bol = 1500;                                           //for example alone. its not for final
  }
  else if (bolus_insulin <= 20 && bolus_insulin > 15)
  {
    pulse_bol = 512;                                                 //taking pulse as 256 ie., 8 steps per revolution for bolus injection
    max_step_bol = bolus_insulin / 0.8;                              //considering it can give 0.5 units per step
    delay_time_bol = 1000;                                           //for example alone. its not for final
  }
  pulse_function_bolus();
}




//pulse function based on different bolus level
void pulse_function_bolus()
{
  count_bol = 0;
  if (pulse_bol == 128)
  {
    while (count_bol < max_step_bol)
    {
      if (volume_completed != 300)
      {
        digitalWrite(dirPin, HIGH);
        for (int x = 0; x < pulse_bol; x++)
        {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(500);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(500);
        }
        count_bol++;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Bolus Injected");
        lcd.setCursor(0, 1);
        lcd.print(count_bol * 0.2);
        lcd.setCursor(5, 1);
        lcd.print("Units");
        //Serial.println("Bolus Insulin Units Injected:");
        //Serial.println(count * 0.2);
        delay(delay_time_bol);
      }
      else if (volume_completed >= 300)
      { lcd.clear();
        lcd.print("List laye illa");
        delay(1000);
        reservoir_completed();
      }
    }
  }
  else if (pulse_bol == 256)
  {
    while (count_bol < max_step_bol)
    {
      if (volume_completed != 300)
      {
        digitalWrite(dirPin, HIGH);
        for (int x = 0; x < pulse_bol; x++)
        {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(500);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(500);
        }
        count_bol++;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Bolus Injected");
        lcd.setCursor(0, 1);
        lcd.print(count_bol * 0.5);
        lcd.setCursor(5, 1);
        lcd.print("Units");
        //Serial.println("Bolus Insulin Units Injected:");
        //Serial.println(count * 0.5);
        delay(delay_time_bol);
      }
      else if (volume_completed >= 300)
      {
        reservoir_completed();
      }
    }
  }
  else if (pulse_bol == 512)
  {
    while (count_bol < max_step_bol)
    {
      digitalWrite(dirPin, HIGH);
      for (int x = 0; x < pulse_bol; x++)
      {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(500);
      }
      count_bol++;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bolus Injected");
      lcd.setCursor(0, 1);
      lcd.print(count_bol * 0.8);
      lcd.setCursor(5, 1);
      lcd.print("Units");
      //Serial.println("Bolus Insulin Units Injected:");
      //Serial.println(count * 0.8);
      delay(delay_time_bol);
    }
  }
}

/*******************************************************************setInputFlags function - User Created Function************************************************************************/
void setInputFlags() {
  for (int i = 0; i < numOfInputs; i++) {
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
  for (int i = 0; i < numOfInputs; i++) {
    if (inputFlags[i] == HIGH) {
      inputAction(i);
      inputFlags[i] = LOW;
      printScreen();
    }
  }
}

void inputAction(int input) {
  if (input == 0) {
    if (currentScreen == 0) {
      currentScreen = numOfScreens - 1;
    } else {
      currentScreen--;
    }
  } else if (input == 1) {
    if (currentScreen == numOfScreens - 1) {
      currentScreen = 0;
    } else {
      currentScreen++;
    }
  } else if (input == 2) {
    parameterChange(0);
  } else if (input == 3) {
    parameterChange(1);
  }
}

void parameterChange(int key) {
  if (key == 0) {
    parameters[currentScreen]++;
  } else if (key == 1) {
    parameters[currentScreen]--;
  }
}

void printScreen() {
  lcd.clear();
  lcd.print(screens[currentScreen][0]);
  lcd.setCursor(0, 1);
  lcd.print(parameters[currentScreen]);
  lcd.print(" ");
  lcd.print(screens[currentScreen][1]);
}

void reservoir_completed() {
  flag = false;
  while (flag = ! true)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ALERT!");
    lcd.setCursor(0, 1);
    lcd.print("Reservoir Empty");
    tone(beep, 1000);
    delay(500);
    lcd.clear();
    noTone(beep);
    delay(500);
  }
  // Rewinding code yet to write and all this is blind build; Precision can be achieved only through encoders and some other methods
}
