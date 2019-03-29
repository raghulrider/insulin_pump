const int stepPin = 3; 
const int dirPin = 4; 
long pulse;
void setup() {
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  Serial.begin(9600);
  
}
void loop() 
{
  int count =0;
  Serial.println("Enter the pulse rate:");
  while (Serial.available()==0){}
  pulse = Serial.parseInt();
  Serial.println("Current Pulse Rate:");
  Serial.println(pulse);
  if (pulse == 64)
  {
    while (count<=31)
    {
    
    digitalWrite(dirPin,HIGH);
    for(int x = 0; x < pulse; x++)
    {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500); 
    }
  count++;
  Serial.println("Completed Step Count:");
  Serial.println(count);
  delay(2000);
    }
  }
   else if (pulse == 128)
  {
    while (count<=15)
    {
    digitalWrite(dirPin,HIGH);
    for(int x = 0; x < pulse; x++)
    {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500); 
    }
  count++;
  Serial.println("Completed Step Count:");
  Serial.println(count);
  delay(2000);
    }
  }
     else if (pulse == 256)
  {
    while (count<=7)
    {
    digitalWrite(dirPin,HIGH);
    for(int x = 0; x < pulse; x++)
    {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500); 
    }
  count++;
  Serial.println("Completed Step Count:");
  Serial.println(count);
  delay(2000);
    }
  }
     else if (pulse == 512)
  {
    while (count<=3)
    {
    digitalWrite(dirPin,HIGH);
    for(int x = 0; x < pulse; x++)
    {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500); 
    }
  count++;
  Serial.println("Completed Step Count:");
  Serial.println(count);
  delay(2000);
    }
  }
bolus_interrupt:

}
