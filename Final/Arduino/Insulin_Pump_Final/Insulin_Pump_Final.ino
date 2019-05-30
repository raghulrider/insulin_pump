/***************************************************HEADER SECTION***********************************************/
#include <Stepper.h>                 //Stepper Header
#include <WiFi.h>                    //WiFi Header
#include <IOXhop_FirebaseESP32.h>    //Firebase Header
#include "Nextion.h"                 //Nextion Header

/*************************************************INITIALIZATION PART********************************************/
Stepper motor(200, 4, 0, 2, 15);                                     //Initializing Stepper Library with 200SPR on Pins 0,2,4,15
int stepCount = 0;

#define FIREBASE_HOST "thesmartflow-6b7c4.firebaseio.com"            //Firebase HOST ID
#define FIREBASE_AUTH "gpMsnrvHBcBU3r6NvEzlVHJKMMWm6vCuJ9Plp6wr"     //Firebase Auth Key 
#define WIFI_SSID "HWjunction"                                       //WiFi Name
#define WIFI_PASSWORD "forged@forge"                                 //WiFi PassWord

NexButton basaladd = NexButton(2, 3, "basaladd");
NexButton basalsub = NexButton(2, 4, "basalsub");
NexButton startbasal = NexButton(2, 6, "startbasal");
NexButton suspendbasal = NexButton(2, 7, "suspendbasal");
NexText basal = NexText(2, 5, "basal");

NexButton bolusadd = NexButton(3, 3, "bolusadd");
NexButton bolussub = NexButton(3, 4, "bolussub");
NexText bolus = NexText(3, 5, "bolus");
NexTouch *nex_listen_list[] = {&basaladd, &bolusadd, &basalsub, &bolussub, &startbasal, &suspendbasal, NULL};

int basalData = 0;
int bolusData = 0;

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 3600000;                                // 1 Hour interval in MilliSeconds
bool startBasalFlag = false;
/*******************************************************SETUP****************************************************/
void setup() {
  Serial.begin(115200);
  motor.setSpeed(60);                                                //setting motor at 60RPM
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                              //Attempts to Connect TO WiFi SSID and PassWord
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {                            //Waits Until WiFi connection established
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected with WiFi - SUCCESS");
  delay(300);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                      //Establishes Connection with Firebase
  Serial.println("Connection Established with Firebase Success");
  nexInit();
  basaladd.attachPop(basalAddPopCallback, &basaladd);                //PopCallBack Function for Basal Increment
  basalsub.attachPop(basalSubPopCallback, &basalsub);                //PopCallBack Function for Basal Decrement
  bolusadd.attachPop(bolusAddPopCallback, &bolusadd);                //PopCallBack Function for Bolus Increment
  bolussub.attachPop(bolusSubPopCallback, &bolussub);                //PopCallBack Function for Bolus Decrement
  startbasal.attachPop(startBasalPopCallback, &startbasal);                //PopCallBack Function for Bolus Decrement
  suspendbasal.attachPop(suspendBasalPopCallback, &suspendbasal);                //PopCallBack Function for Bolus Decrement
  startMillis = millis();
}

/***************************************************LOOP SECTION*************************************************/
void loop() {
  nexLoop(nex_listen_list);
  if (startBasalFlag == true) {
    currentMillis = millis();
    if (currentMillis - startMillis >= period){
      basalFunction();
      startMillis = currentMillis;
    }
  }
}

/**************************************************Methods Section**********************************************/
void basalAddPopCallback(void *ptr)                                 //OnClick Basal Increment Button
{
  static char buffer[6];
  itoa(basalData, buffer, 10);                                      // Int to String
  basal.setText(buffer);                                            // Display Data
  if (basalData < 25) {
    basalData++;
    Serial.println("Basal Incremented");
  }
}

void basalSubPopCallback(void *ptr)                                //OnClick Basal Decrement Button
{
  static char buffer[6];
  itoa(basalData, buffer, 10);                                     // Int to String
  basal.setText(buffer);                                           // Display Data
  if (basalData > 0) {
    basalData--;
    Serial.println("Basal Decremented");
  }
}

void bolusAddPopCallback(void *ptr)                               //OnClick Bolus Increment Button
{
  static char buffer[6];
  itoa(bolusData, buffer, 10);                                    // Int to String
  bolus.setText(buffer);                                          // Display Data
  if (bolusData < 25) {
    bolusData++;
    Serial.println("Bolus Incremented");
  }
}

void bolusSubPopCallback(void *ptr)                              //OnClick Bolus Decrement Button
{
  static char buffer[6];
  itoa(bolusData, buffer, 10);                                   // Int to String
  bolus.setText(buffer);                                         // Display Data
  if (bolusData > 0) {
    bolusData--;
    Serial.println("Bolus Decremented");
  }
}

void startBasalPopCallback(void *ptr)                              //OnClick Start Basal Button
{
  Serial.println("Basal Start Button is Pressed");
  startBasalFlag = true;
}

void suspendBasalPopCallback(void *ptr)                             //OnClick Suspend Basal Button
{
  Serial.println("Basal Suspend Button is Pressed");
  startBasalFlag = false;
}

void basalFunction() {
  double basalData_ph;
  int maxStep;
  if(basalData <=10){
    basalData_ph = basalData / 24.00;
    maxStep = basalData_ph / 0.1;
    startBasal(32, maxStep); 
  }
  else if(basalData <= 20 && basalData >10){
    basalData_ph = basalData / 24.00;
    maxStep = basalData_ph / 0.2;
    startBasal(64, maxStep); 
  }
  else if(basalData <= 30 && basalData >20){
    basalData_ph = basalData / 24.00;
    maxStep = basalData_ph / 0.5;
    startBasal(128, maxStep); 
  }
  else if(basalData <= 30 && basalData >20){
    basalData_ph = basalData / 24.00;
    maxStep = basalData_ph / 0.8;
    startBasal(256, maxStep); 
  }
}

void bolusFunction() {
  int maxStep;
  if(basalData <=10){
    maxStep = basalData / 0.1;
    startBolus(32, maxStep); 
  }
  else if(basalData <= 20 && basalData >10){
    maxStep = basalData / 0.2;
    startBolus(64, maxStep); 
  }
  else if(basalData <= 30 && basalData >20){
    maxStep = basalData / 0.5;
    startBolus(128, maxStep); 
  }
  else if(basalData <= 30 && basalData >20){
    maxStep = basalData / 0.8;
    startBolus(256, maxStep); 
  }
}

void startBasal(int steps, float maxStep) {
  int count =0;
  while(count < maxStep){
    motor.step(steps);
    delay(2000);
    count++;  
  }
}

void startBolus(int steps, float maxStep) {
  int count =0;
  while(count < maxStep){
    motor.step(steps);
    delay(2000);
    count++;  
  }
}
