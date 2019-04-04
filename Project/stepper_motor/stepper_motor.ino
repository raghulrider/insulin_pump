/*Every data mentioned is decided based on current situation. It will vary when we decide motors and attachments*/

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


//setup menu
void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(3),bolus_interrupt_flag,RISING);
 }


//Main loop for recieving basal or bolus option
void loop()
{
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
  Serial.println(flag);                                      //uncomment to see the value of flag
  Serial.println("You have entered basal menu");
  Serial.println("Enter the basal insulin:");
  while (Serial.available() == 0) {}
  basal_insulin = Serial.parseInt();
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
  else if (basal_insulin <=30 && basal_insulin > 20)
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
  for (i=0; i<=23 ; i++)
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
      Serial.println("Basal Insulin Units Injected:");
      Serial.println(count*0.1);
      delay(delay_time);
      if (flag == true)
      {
        bolus_function();
      }
    }
    delay(3600000-max_step*(1+delay_time));
   }
  else if (pulse == 128)
  {
    while (count < max_step)
    {
      if (flag == true)
      {
        bolus_function();
      }
      digitalWrite(dirPin, HIGH);
      for (int x = 0; x < pulse; x++)
      {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(500);
      }
      count++;
      Serial.println("Basal Insulin Units Injected:");
      Serial.println(count*0.2);
      delay(delay_time);
    }
    delay(3600000-max_step*(1+delay_time));
  }
  else if (pulse == 256)
  {
    while (count < max_step)
    {
      if (flag == true)
      {
        bolus_function();
      }
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
      Serial.println("Basal Insulin Units Injected:");
      Serial.println(count*0.5);
      delay(delay_time);
    }
    delay(3600000-max_step*(1+delay_time));
  }
  else if (pulse == 512)
  {
    while (count < max_step)
    {
      if (flag == true)
      {
        bolus_function();
      }
      digitalWrite(dirPin, HIGH);
      for (int x = 0; x < pulse; x++)
      {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(500);
      }
      count++;
      Serial.println("Basal Insulin Units Injected:");
      Serial.println(count*0.8);
      delay(delay_time);
    }
    delay(3600000-max_step*(1+delay_time));
  }
  }
}


//void loop for bolus menu
void bolus_function()
{
  Serial.println("You have entered bolus menu");
  Serial.println("Enter the bolus insulin:");
  while (Serial.available() == 0) {}
  bolus_insulin = Serial.parseInt();
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
      Serial.println("Bolus Insulin Units Injected:");
      Serial.println(count*0.2);
      delay(delay_time);
    }
     flag == false;
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
      Serial.println("Bolus Insulin Units Injected:");
      Serial.println(count*0.5);
      delay(delay_time);
    }
     flag == false;
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
      Serial.println("Bolus Insulin Units Injected:");
      Serial.println(count*0.8);
      delay(delay_time);
    }
     flag == false;
  }
}
