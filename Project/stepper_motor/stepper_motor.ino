//Every data mentioned is decided based on current situation. It will vary when we decide motors and attachments

const int stepPin = 3;
const int dirPin = 4;
long pulse;
long basbol;
int max_step;
long bolus_insulin;
long basal_insulin;
//setup menu

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  Serial.begin(9600);
}

//Main loop for recieving basal or bolus option

void loop()
{
  Serial.println("Enter the pulse rate:");
  while (Serial.available() == 0) {}
  basbol = Serial.parseInt();
  if (basbol == 1142)                    //giving unique ID for basal as 1142 - Just for Test purpose
  {
    void basal_function();
  }     
  else if (basbol == 1152)               //giving unique ID for bolus as 1152 - Just for Test purpose
  { 
    void bolus_function();
  }
}

//void loop for basal menu


void basal_function()
{
  Serial.println("You have entered basal menu");
  Serial.println("Enter the basal insulin:");
  while (Serial.available() == 0) {}
  basal_insulin = Serial.parseInt();
  if (basal_insulin <= xxx)
  {
    basal_insulin_ph = basal_insulin/24;  //diving total total insulin units for per hour basal delievery  
    pulse = 64;                         //taking pulse as 64 ie., 32 steps per revolution for bolus injection
    max_step  = basal_insulin_ph / 0.1;    //considering it can give 0.1 units per step
    delay_time = 3500                  //for example alone. its not for final
  }
  else if (basal_insulin <= xxx && basal_insulin >= xxx)
  {
    basal_insulin_ph = basal_insulin/24;  //diving total total insulin units for per hour basal delievery
    pulse = 128;                         //taking pulse as 128 ie., 16 steps per revolution for bolus injection
    max_step = basal_insulin_ph / 0.2;      //considering it can give 0.2 units per step
    delay_time = 3000                    //for example alone. its not for final
  }
  else if (basal_insulin <= xxx && basal_insulin >= xxx)
  {
    basal_insulin_ph = basal_insulin/24;  //diving total total insulin units for per hour basal delievery
    pulse = 256;                         //taking pulse as 256 ie., 8 steps per revolution for bolus injection
    max_step = basal_insulin_ph / 0.5;      //considering it can give 0.5 units per step
    delay_time = 2500                    //for example alone. its not for final
  }
  else if (basal_insulin <= xxx && basal_insulin >= xxx)
  {
    basal_insulin_ph = basal_insulin/24;  //diving total total insulin units for per hour basal delievery
    pulse = 512;                         //taking pulse as 512 ie., 5 steps per revolution for bolus injection
    max_step = basal_insulin_ph / 0.8;      //considering it can give 0.5 units per step
    delay_time = 2000                    //for example alone. its not for final
  } 
  void pulse_function();
}


//void loop for bolus menu

void bolus_function()

{
  Serial.println("You have entered bolus menu");
  Serial.println("Enter the bolus insulin:");
  while (Serial.available() == 0) {}
  bolus_insulin = Serial.parseInt();
  if (bolus_insulin <= xxx)2000)
  {
    pulse = 128;                        //taking pulse as 256 ie., 16 steps per revolution for bolus injection
    max_step  = bolus_insulin / 0.2;    //considering it can give 0.2 units per step
    delay_time = 3000                   //for example alone, its not for final
  }
  else if (bolus_function <= xxx && bolus_function >= xxx)
  {
    pulse = 256;                        //taking pulse as 256 ie., 8 steps per revolution for bolus injection
    max_step = bolus_insulin / 0.5;     //considering it can give 0.5 units per step
    delay_time = 1500                   //for example alone. its not for final
  }
  else if (bolus_function <= xxx && bolus_function >= xxx)
  {
    pulse = 512;                        //taking pulse as 256 ie., 8 steps per revolution for bolus injection
    max_step = bolus_insulin / 0.8;     //considering it can give 0.5 units per step
    delay_time = 1000                   //for example alone. its not for final
  }
  void pulse_function();
}



//pulse function based on different insulin level

void pulse_function()
{
  int count = 0;
  if (pulse == 64)
  {
    while (count <= max_step)
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
      Serial.println("Completed Step Count:");
      Serial.println(count);
      delay(delay_time);
    }
  }
  else if (pulse == 128)
  {
    while (count <= max_step)
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
      Serial.println("Completed Step Count:");
      Serial.println(count);
      delay(delay_time);
    }
  }
  else if (pulse == 256)
  {
    while (count <= max_step)
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
      Serial.println("Completed Step Count:");
      Serial.println(count);
      delay(delay_time);
    }
  }
  else if (pulse == 512)
  {
    while (count <= max_step)
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
      Serial.println("Completed Step Count:");
      Serial.println(count);
      delay(delay_time);
    }
  }
}
