#include "Nextion.h"
SoftwareSerial HMISerial(10,11);

//Insulin Value
NexText textInsulin = NexText(1,6, "insulin");

// Add insulin by 1
NexButton buttonAdd = NexButton(1,3,"plus");

//Subtract insulin by 1
NexButton buttonMinus = NexButton(1,4,"minus");

int number = 50;
char buffer[10] = {0};

//Registering button to the touch event list
NexTouch *nex_listen_list[] ={&textInsulin,&buttonAdd,&buttonMinus,NULL};

//button insulin add pop callback function
void buttonAddPopCallback(void *ptr)
{
  dbSerialPrintln("buttonAddPopCallback");

  if (number<100) number += 1;

  memset(buffer, 0, sizeof(buffer)); //clear buffer
  itoa(number, buffer, 10);
  textInsulin.setText(buffer);
}

// button insulin minus push callback function
void buttonMinusPushCallback(void *ptr)
{
  dbSerialPrintln("buttonMinusPushCallback");

  if (number>0) number -= 1;

  memset(buffer, 0, sizeof(buffer)); //clear buffer
  itoa(number, buffer, 10);
  textInsulin.setText(buffer);
}

void setup(void)
{
  nexInit();

  Serial.begin(115200);

  buttonAdd.attachPop(buttonAddPopCallback);

  buttonMinus.attachPush(buttonMinusPushCallback);

  dbSerialPrintln("Setup Done");
}

void loop(void)
{
  nexLoop(nex_listen_list);
}
