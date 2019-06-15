/***************************************************HEADER SECTION***********************************************/                 //WiFi Header
#include <ESP8266WiFi.h>    //Firebase Header
#include <FirebaseArduino.h>
#include "Nextion.h"                 //Nextion Header
#include <Wire.h>
#include <RtcDS3231.h>               //RTC Library

/*************************************************INITIALIZATION PART********************************************/
RtcDS3231<TwoWire> Rtc(Wire);

#define stepPin D6
#define dirPin D7                              //Initializing Stepper motor
int stepCount = 0;

#define FIREBASE_HOST "spero-a0ff0.firebaseio.com"                   //Firebase HOST ID
#define FIREBASE_AUTH "VnvTaOh7HYy5Q97ra3qx0kecYoNrbdwxJuxHcGuC"     //Firebase Auth Key 
#define WIFI_SSID "HWjunction"                                       //WiFi Name
#define WIFI_PASSWORD "forged@forge"                                 //WiFi PassWord

const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASSWORD;

NexButton basaladd = NexButton(2, 3, "basaladd");
NexButton basalsub = NexButton(2, 4, "basalsub");
NexButton startbasal = NexButton(4, 3, "startbasal");
NexButton suspendbasal = NexButton(5, 3, "suspendbasal");
NexText basal = NexText(2, 5, "basal");

NexText pageonetime = NexText(1, 12, "pageonetime");
NexText pageonedate = NexText(1, 13, "pageonedate");
NexText reshome = NexText(1, 6, "reshome");
NexText bathome = NexText(1, 7, "bathome");
NexText injectedhome = NexText(1, 7, "injectedhome");

NexText infotop = NexText(9, 5, "infotop");
NexText infobottom = NexText(9, 6, "infobottom");
NexText resinfo = NexText(9, 3, "resinfo");
NexText batinfo = NexText(9, 4, "batinfo");

NexText lowspeed = NexText(10, 2, "lowspeed");
NexText midspeed = NexText(10, 3, "midspeed");
NexText highspeed = NexText(10, 4, "highspeed");
NexText rewindmotor = NexText(18, 2, "rewindmotor");

NexPage page1 = NexPage(1, 0, "page1");
NexPage page12 = NexPage(12, 0, "page12");
NexPage page13 = NexPage(13, 0, "page13");
NexPage page14 = NexPage(14, 0, "page14");
NexPage page15 = NexPage(15, 0, "page15");
NexPage page16 = NexPage(16, 0, "page16");

NexButton bolusadd = NexButton(3, 3, "bolusadd");
NexButton bolussub = NexButton(3, 4, "bolussub");
NexButton givebolus = NexButton(7, 3, "givebolus");
NexText bolus = NexText(3, 5, "bolus");
NexTouch *nex_listen_list[] = {&basaladd, &bolusadd, &basalsub, &bolussub, &startbasal, &suspendbasal, &givebolus, &lowspeed, &midspeed, &highspeed, &page1, &page12,&page13, &page14, &page15,&page16, &rewindmotor, NULL};

const int maxInsulin = 300;
double pumpedInsulin = 0.00;
int basalData = 0;
int bolusData = 0;
const int maximumstep = 2200;
const int minimumstep = 0;
String currentdate, currenttime, topinfodata = "Nothing", bottominfodata = " to show", hometime, homedate;

char buffer[25] = {0};
unsigned long startMillis, currentMillis;
const unsigned long BasalPeriod = 3600000;                                // 1 Hour interval in MilliSeconds
bool startBasalFlag = false;
unsigned long timed;
unsigned long curtimed;
/*******************************************************SETUP****************************************************/
void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  nexInit();
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
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
  lowspeed.attachPop(lowspeedPopCallback, &lowspeed);
  midspeed.attachPop(midspeedPopCallback, &midspeed);
  highspeed.attachPop(highspeedPopCallback, &highspeed);
  rewindmotor.attachPop(rewindMotorPopCallback, &rewindmotor);
  startMillis = millis();
  timed = millis();
  Rtc.Begin();
  //initializeStepper();
  //updateRTCtime();
}

/***************************************************LOOP SECTION*************************************************/
void loop() {
  nexLoop(nex_listen_list);
  curtimed = millis();
  if (startBasalFlag == true) {
    currentMillis = millis();
    if (currentMillis - startMillis >= BasalPeriod) {
      basalFunction();
      startMillis = currentMillis;
    }
  }
  /*if (firecurrent - firestart >= FirebaseUpdatePeriod) {
    //updateFirebase();
    firestart = firecurrent;
    }*/
  if (abs(curtimed - timed) >= 2000) {
    infoUpdate();
    timed = millis();
  }
}

/**************************************************Methods Section**********************************************/
void basalAddPopCallback(void *ptr)                                 //OnClick Basal Increment Button
{
  if (basalData < 25) {
    basalData++;
  }
  itoa(basalData, buffer, 10);                                      // Int to String
  basal.setText(buffer);                                            // Display Data

}

void basalSubPopCallback(void *ptr)                                //OnClick Basal Decrement Button
{
  if (basalData > 0) {
    basalData--;
  }
  itoa(basalData, buffer, 10);                                     // Int to String
  basal.setText(buffer);                                           // Display Data
}

void bolusAddPopCallback(void *ptr)                               //OnClick Bolus Increment Button
{
  if (bolusData < 25) {
    bolusData++;
  }
  itoa(bolusData, buffer, 10);                                    // Int to String
  bolus.setText(buffer);                                          // Display Data
}

void bolusSubPopCallback(void *ptr)                              //OnClick Bolus Decrement Button
{
  if (bolusData > 0) {
    bolusData--;
  }
  itoa(bolusData, buffer, 10);                                   // Int to String
  bolus.setText(buffer);                                         // Display Data
}

void startBasalPopCallback(void *ptr)                              //OnClick Start Basal Button
{
  page15.show();
  RtcDateTime now = Rtc.GetDateTime();
  updateDateTime(now);
  topinfodata = "Basal Given at";
  bottominfodata = currenttime + " " + currentdate;
  startBasalFlag = true;
}

void suspendBasalPopCallback(void *ptr)                             //OnClick Suspend Basal Button
{
  page16.show();
  RtcDateTime now = Rtc.GetDateTime();
  updateDateTime(now);
  topinfodata = "Basal Suspended at";
  bottominfodata = currenttime + " " + currentdate;
  startBasalFlag = false;
}

void giveBolusPopCallback(void *ptr)                                 //OnClick Basal Increment Button
{
  RtcDateTime now = Rtc.GetDateTime();
  updateDateTime(now);
  topinfodata = "Bolus Given at";
  bottominfodata = currenttime + " " + currentdate;
  bolusFunction();
}

void lowspeedPopCallback(void *ptr)                                  //OnClick Low speed Button
{
  if(stepCount+11<=maximumstep){
  digitalWrite(dirPin, HIGH);
  for (int i = 0; i < 11; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
  stepCount += 11;
  pumpedInsulin += 1.5;
}
else
{
  page14.show();
}
}

void midspeedPopCallback(void *ptr)                                  //OnClick Medium speed Button
{
  if(stepCount+33<=maximumstep){
  digitalWrite(dirPin, HIGH);
  for (int i = 0; i < 33; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
  stepCount += 33;
  pumpedInsulin += 4.5;
}
else
{
  page14.show();
}
}

void highspeedPopCallback(void *ptr)                                 //OnClick High speed Button
{
   if(stepCount+88<=maximumstep){
  digitalWrite(dirPin, HIGH);
  for (int i = 0; i < 88; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
  stepCount += 88;
  pumpedInsulin += 12;
}
else{
  page14.show();
}
}

void rewindMotorPopCallback(void *ptr)                                 //OnClick High speed Button
{
  digitalWrite(dirPin, LOW);
  for (int i = 0; i < stepCount; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
  stepCount = 0;
  pumpedInsulin = 0;
}

void basalFunction() {
  Serial.println("I'm in Basal Function");
  double basalData_ph;
  int maxStep = 0 ;
  if (basalData <= 10 || basalData % 10 == 0) {
    basalData_ph = basalData / 24.00;
    maxStep = basalData_ph / 0.4090909;
    pumpedInsulin += basalData_ph;
    startBasal(3, maxStep);
  }
  else if (basalData > 10) {
    basalData_ph = basalData / 24.00;
    maxStep = basalData_ph / 0.618181;
    pumpedInsulin += basalData_ph;
    startBasal(5, maxStep);
  }
}

void bolusFunction() {
  //Serial.println("I'm in Bolus Function");
  int maxStep = 0;
  if (bolusData <= 15 && bolusData % 3 != 0) {
    maxStep = bolusData / 0.40909090;
    pumpedInsulin += bolusData;
    startBolus(3, maxStep, 1000);
  }
  else if (bolusData <= 30 && bolusData % 3 == 0) {
    maxStep = bolusData / 1.5;
    pumpedInsulin += bolusData;
    startBolus(11, maxStep, 1000);
  }
  else if (bolusData > 30 && bolusData % 3 == 0) {
    maxStep = bolusData / 3;
    pumpedInsulin += bolusData;
    startBolus(22, maxStep, 1000);
  }
  else if (bolusData < 25 && bolusData % 2 == 0 ) {
    maxStep = bolusData / 2.181818;
    pumpedInsulin += bolusData;
    startBolus(16, maxStep, 1000);
  }
  else {
    maxStep = bolusData / 0.4090909;
    pumpedInsulin += bolusData;
    startBolus(3, maxStep, 1000);
  }
}

void startBasal(int steps, int maxStep) {
  if(steps*maxStep < maximumstep - stepCount){
    int count = 0;
  while (count < maxStep) {
    for (int i = 0; i < steps; i++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    delay(1000);
    count++;
  }
  stepCount += (maxStep * steps);
  RtcDateTime now = Rtc.GetDateTime();
  updateDateTime(now);
  String fireupdatebasal = "Basal@" + String(basalData) + "@" + currentdate + "@" + currenttime;
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& basalfirebase = JSONbuffer.createObject();
  basalfirebase["data"] = fireupdatebasal;
  Firebase.push("sanjay/medication", basalfirebase);
  Serial.print("Total Insulin Remaining : ");
  Serial.print(maxInsulin - pumpedInsulin);
  Serial.println(" Units");
    }
    else{
      page13.show();
      }
}

void startBolus(int steps, int maxStep, int delayTime) {
  if(steps*maxStep < maximumstep - stepCount){
  page12.show();
  int count = 0;
  digitalWrite(dirPin, HIGH);
  while (count < maxStep) {
    for (int i = 0; i < steps; i++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    delay(delayTime);
    count++;
  }
  stepCount += (maxStep * steps);
  RtcDateTime now = Rtc.GetDateTime();
  updateDateTime(now);
  String fireupdatebolus = "Bolus@" + String(bolusData) + "@" + currentdate + "@" + currenttime;
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& bolusfirebase = JSONbuffer.createObject();
  bolusfirebase["data"] = fireupdatebolus;
  Firebase.push("sanjay/medication", bolusfirebase);
  Serial.print("Total Insulin Remaining : ");
  Serial.print(maxInsulin - pumpedInsulin);
  Serial.println(" Units");
  }
  else{
    page13.show();
    }
}

/*
  void updateFirebase() {
  String tempbasal = String(basalData);
  Firebase.setString("Patient/BQf10hHVAHTNL2ot95SZUH4IOD13/basal", tempbasal);
  String tempbolus = String(bolusData);
  Firebase.setString("Patient/BQf10hHVAHTNL2ot95SZUH4IOD13/bolus", tempbolus);
  Serial.println("Firebase Update Success ----- Upto Date");
  }
*/

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

void homedatetime(const RtcDateTime& dt)
{
  char hometimechar[10], homedatechar[12];
  snprintf_P(hometimechar,
             countof(hometimechar),
             PSTR("%02u:%02u"),
             dt.Hour(),
             dt.Minute());
  snprintf_P(homedatechar,
             countof(homedatechar),
             PSTR("%02u/%02u/%04u"),
             dt.Month(),
             dt.Day(),
             dt.Year());
  hometime = hometimechar;
  homedate = homedatechar;
}
void updateRTCtime()
{
  RtcDateTime compiled = RtcDateTime(__DATE__, "17:54:35");
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

void infoUpdate()
{
  RtcDateTime now = Rtc.GetDateTime();
  homedatetime(now);
  hometime.toCharArray(buffer, 10);
  pageonetime.setText(buffer);
  homedate.toCharArray(buffer, 12);
  pageonedate.setText(buffer);
  topinfodata.toCharArray(buffer, 20);
  infotop.setText(buffer);
  bottominfodata.toCharArray(buffer, 20);
  infobottom.setText(buffer);
  double temp = (pumpedInsulin / maxInsulin) * 100;
  int tempint = 100 - temp;
  itoa(tempint, buffer, 10);                                   // Int to String
  reshome.setText(buffer);
  itoa((300 - pumpedInsulin), buffer, 10);
  resinfo.setText(buffer);
  itoa(pumpedInsulin, buffer, 10);
  injectedhome.setText(buffer);
  itoa(100, buffer, 10);
  bathome.setText(buffer);
  itoa(100, buffer, 10);
  batinfo  .setText(buffer);
}
void initializeStepper()
{
  for(int x=0; x<11; x++){
  digitalWrite(dirPin, HIGH);
  for (int i = 0; i < 200; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
  delay(1000);
  }
  for(int x=0; x<16; x++){ //change it to 16 after demo
  digitalWrite(dirPin, LOW);
  for (int i = 0; i < 200; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
  delay(1000);
  }
}
