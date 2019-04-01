void setup() 
{
 Serial.begin(9600);
 attachInterrupt(0,test,RISING); 
}

void loop() {
  Serial.println("I'm in Loop");
  delay(1000);
}
void test()
{
  Serial.println("Interrupt");
}
