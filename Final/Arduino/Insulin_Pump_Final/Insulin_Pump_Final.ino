/***************************************************HEADER SECTION***********************************************/
#include <Stepper.h>                 //Stepper Header
#include <WiFi.h>                    //WiFi Header
#include <IOXhop_FirebaseESP32.h>    //Firebase Header
#include "Nextion.h"                 //Nextion Header
#include <Wire.h>
#include <RtcDS3231.h>               //RTC Library

/*************************************************INITIALIZATION PART********************************************/
RtcDS3231<TwoWire> Rtc(Wire);

Stepper motor(200, 15, 2, 0, 4);                                     //Initializing Stepper Library with 200SPR on Pins 0,2,4,15
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
NexButton givebolus = NexButton(1, 8, "givebolus");
NexText bolus = NexText(3, 5, "bolus");
NexTouch *nex_listen_list[] = {&basaladd, &bolusadd, &basalsub, &bolussub, &startbasal, &suspendbasal, &givebolus, NULL};

int basalData = 0;
int bolusData = 0;
String currentdate, currenttime;

unsigned long startMillis, currentMillis, firestart, firecurrent;
const unsigned long BasalPeriod = 3600000;                                // 1 Hour interval in MilliSeconds
const unsigned long FirebaseUpdatePeriod = 60000;
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
  startbasal.attachPop(startBasalPopCallback, &startbasal);          //PopCallBack Function for Bolus Decrement
  suspendbasal.attachPop(suspendBasalPopCallback, &suspendbasal);    //PopCallBack Function for Bolus Decrement
  givebolus.attachPop(giveBolusPopCallback, &givebolus);             //PopCallBack Function for Giving Bolus
  startMillis = millis();
  firestart = millis();

  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  if (!Rtc.IsDateTimeValid())
  {
    if (Rtc.LastError() != 0)
    {
      Serial.println(Rtc.LastError());
    }
    else
    {
      Rtc.SetDateTime(compiled);
    }
  }

  if (!Rtc.GetIsRunning())
  {
    Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled)
  {
    Rtc.SetDateTime(compiled);
  }
  else if (now > compiled)
  {
    Rtc.SetDateTime(compiled);
  }
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);
}

/***************************************************LOOP SECTION*************************************************/
void loop() {
  nexLoop(nex_listen_list);
  if (startBasalFlag == true) {
    currentMillis = millis();
    if (currentMillis - startMillis >= BasalPeriod) {
      basalFunction();
      startMillis = currentMillis;
    }
  }
  firecurrent = millis();
  if (firecurrent - firestart >= FirebaseUpdatePeriod) {
    updateFirebase();
    firestart = firecurrent;
  }
}

/**************************************************Methods Section**********************************************/
void basalAddPopCallback(void *ptr)                                 //OnClick Basal Increment Button
{
  static char buffer[6];
  if (basalData < 25) {
    basalData++;
  }
  itoa(basalData, buffer, 10);                                      // Int to String
  basal.setText(buffer);                                            // Display Data

}

void basalSubPopCallback(void *ptr)                                //OnClick Basal Decrement Button
{
  static char buffer[6];
  if (basalData > 0) {
    basalData--;
  }
  itoa(basalData, buffer, 10);                                     // Int to String
  basal.setText(buffer);                                           // Display Data
}

void bolusAddPopCallback(void *ptr)                               //OnClick Bolus Increment Button
{
  static char buffer[6];
  if (bolusData < 25) {
    bolusData++;
  }
  itoa(bolusData, buffer, 10);                                    // Int to String
  bolus.setText(buffer);                                          // Display Data
}

void bolusSubPopCallback(void *ptr)                              //OnClick Bolus Decrement Button
{
  static char buffer[6];
  if (bolusData > 0) {
    bolusData--;
  }
  itoa(bolusData, buffer, 10);                                   // Int to String
  bolus.setText(buffer);                                         // Display Data
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

void giveBolusPopCallback(void *ptr)                                 //OnClick Basal Increment Button
{
  Serial.println("Give Basal Button is Press000ed");
  bolusFunction();
}

void basalFunction() {
  Serial.println("I'm in Basal Function");
  double basalData_ph;
  int maxStep = 0 ;
  if (basalData <= 10) {
    basalData_ph = basalData / 24.00;
    maxStep = basalData_ph / 0.1;
    startBasal(32, maxStep);
  }
  else if (basalData <= 20 && basalData > 10) {
    basalData_ph = basalData / 24.00;
    maxStep = basalData_ph / 0.2;
    startBasal(64, maxStep);
  }
  else if (basalData <= 30 && basalData > 20) {
    basalData_ph = basalData / 24.00;
    maxStep = basalData_ph / 0.5;
    startBasal(128, maxStep);
  }
  else if (basalData <= 30 && basalData > 20) {
    basalData_ph = basalData / 24.00;
    maxStep = basalData_ph / 0.8;
    startBasal(256, maxStep);
  }
}

void bolusFunction() {
  Serial.println("I'm in Bolus Function");
  int maxStep = 0;
  if (bolusData <= 5) {
    maxStep = bolusData / 0.1;
    startBolus(32, maxStep, 500);
  }
  else if (bolusData > 5 && bolusData <= 10) {
    maxStep = bolusData / 0.2;
    startBolus(64, maxStep, 1000);
  }
  else if (bolusData > 10 && bolusData <= 15) {
    maxStep = basalData / 0.5;
    startBolus(128, maxStep, 2000);
  }
  else if (bolusData > 15) {
    maxStep = bolusData / 0.8;
    startBolus(256, maxStep, 3000);
  }
}

void startBasal(int steps, float maxStep) {
  Serial.println("I'm in Bolus Function Motor Section");
  int count = 0;
  Serial.print("Basal is Set to : ");
  Serial.println(basalData);
  while (count < maxStep) {
    motor.step(steps);
    delay(2000);
    count++;
  }
  RtcDateTime now = Rtc.GetDateTime();
  updateDateTime(now);
  String fireupdatebasal = String(basalData) + "-" +currentdate+ "-" +currenttime;  
  Firebase.setString("Patient/BQf10hHVAHTNL2ot95SZUH4IOD13/injected/data", fireupdatebasal);
}

void startBolus(int steps, float maxStep, int delayTime) {
  Serial.println("I'm in Bolus Function Motor Section");
  int count = 0;
  Serial.print("Bolus is Set to : ");
  Serial.println(bolusData);
  Serial.print("maxStep :");
  Serial.println(maxStep);
  while (count < maxStep) {
    motor.step(steps);
    Serial.println("Giving Bolus");
    Serial.print("Count :");
    Serial.println(count);
    delay(delayTime);
    count++;
  }
  RtcDateTime now = Rtc.GetDateTime();
  updateDateTime(now);
  String fireupdatebolus = currentdate+ "-" +currenttime+ "-bolus-" + String(bolusData);  
  Firebase.setString("Patient/BQf10hHVAHTNL2ot95SZUH4IOD13/injected/data", fireupdatebolus);
  Serial.println("Returning to main Loop............");
}

void updateFirebase() {
  String tempbasal = String(basalData);
  Firebase.setString("Patient/BQf10hHVAHTNL2ot95SZUH4IOD13/basal", tempbasal);
  String tempbolus = String(bolusData);
  Firebase.setString("Patient/BQf10hHVAHTNL2ot95SZUH4IOD13/bolus", tempbolus);
  Serial.println("Firebase Update Success ----- Upto Date");
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void updateDateTime(const RtcDateTime& dt)
{
  char datestring[12], timestring[10];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u"),
             dt.Month(),
             dt.Day(),
             dt.Year());
  snprintf_P(timestring,
             countof(timestring),
             PSTR("%02u:%02u:%02u"),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
  currentdate = datestring;
  currenttime = timestring;
}
