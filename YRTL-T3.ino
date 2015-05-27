/*
  Copyright (c) 2015 Del Rudolph <del@rudforce.com>

  See http://rud.mit-license.org or included file LICENSE
*/

#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>
#include <stdint.h>
#include <TouchScreen.h>
#include <TFT.h>


#define MY_ADDRESS 3
//#define WALLE_ADDRESS 4  // used in testing

// how many receivers to keep track of
#define MAX_KNOWN_RX 5
// and a space to hold their names (indexed by address)
String known_rx[MAX_KNOWN_RX];
// and the one we're currently talking to (if any)
// init to self, cause 0 is a valid address
int cur_rx = MY_ADDRESS;

RH_NRF24 driver(53,49);
RHReliableDatagram manager(driver, MY_ADDRESS);

uint8_t dataPacket[RH_NRF24_MAX_MESSAGE_LEN];
//uint8_t dataPacket[] = "Message! :-)";    // used in testing
uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];

#define lStickX  A8
#define lStickY  A9
#define rStickX  A15
#define rStickY  A14

// touchscreen pins on a Mega2560
#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 54   // can be a digital pin, this is A0
#define XP 57   // can be a digital pin, this is A3

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// some vars to keep track of the stuff we do
int action = 0;
int donePainting = 0;

int connButtonCount;
int connButtons[10][3];

int scanCount = 0;

int sendFailCount = 0;

void setup() {
  Tft.init();
  Tft.setDisplayDirect(LEFT2RIGHT);
  if (!manager.init()){
    Tft.drawString("Manager init failed", 5,22,1, RED);
    donePainting = 1;
  }
  
  Serial.begin(9600);
  known_rx[MY_ADDRESS] = "YRTL-T3";
}

void loop() {
  TSPoint p = ts.getPoint();
  p.x = map(p.x, 120,880,240,0);
  p.y = map(p.y, 115,930,320,0);

  if(p.z > ts.pressureThreshhold){
    delay(20);    // do I need to debounce a touchscreen?
//    Serial.print("p.x: ");
//    Serial.print(p.x);
//    Serial.print(" p.y: ");
//    Serial.println(p.y);

    // if * button is pressed on main screen
    // shows current stick positions, updated at 5 second intervals
    if(p.x > 220 && p.y > 300 && action == 0){
      action = 9;
      donePainting = 0;
    }

    // if Connect button is pressed on main screen
    if(p.x > 0 && (p.y >= 21 && p.y <= 46) && action == 0){
      // Connect screen
      action = 1;
      donePainting = 0;
    }

    // if Scan button is pressed on main screen
    if(p.x > 0 && (p.y >= 48 && p.y <= 73) && action == 0){
      action = 3;
      donePainting = 0;
    }

    // connect page
    // use scanButtons array to build connect buttons
    if(action == 2 && connButtonCount > 0){
      int lastY;
      for(int i=0; i<=connButtonCount; i++){
        if(p.x > 0 && (p.y >= connButtons[i][0] && p.y <= connButtons[i][1])){
          if(connButtons[i][2] == 255){
            action = 3;
            donePainting = 0;
          }else{
            doConnect(connButtons[i][2]);
          }
        }
        lastY = connButtons[i][1];
      }

      // handle Scan button press on connect page
      if(p.x > 0 && (p.y >= lastY && p.y <= lastY+25)){
        action = 3;
        donePainting = 0;
      }
    }
    
    // Cancel button on Scan screen
    if(action == 3 && scanCount && p.x > 0 && (p.y >= 48 && p.y <= 73)){
      action = 0;
      donePainting = 0;
      scanCount = 0;
    }
    
    // if X (close) button is pressed on any screen
    if(p.x > 210 && (p.y > 0 && p.y <= 22)){
      
      // if it's pressed on connected screen
      // FIXME no longer used?
      if(action == 4) disConnect();
      
      // go back to Main screen
      action = 0;
      donePainting = 0;
    }
    
    if(action == 0 && p.x > 0 && (p.y >= 95 && p.y <= 135)){
      disConnect();
      action = 0;
      donePainting = 0;
    }
  }

  if(donePainting == 0){
    if(action == 0)
      mainScreen();
    if(action == 1)
      connectScreen();
    if(action == 3)
      scanScreen();
    if(action == 4)
      connectedScreen();
    if(action == 9)
      sticks2screen();
  }

  // if not talking to ourselves, send a packet
  if(cur_rx != MY_ADDRESS){
    
    // first blank the existing packet
    memset(dataPacket,0,sizeof(dataPacket));
    
    // make it a command data packet
    dataPacket[0] = 'c';

  //  Serial.print("LX: ");
  //  Serial.print(analogRead(A8));
    unsigned int LX = analogRead(A8);
    dataPacket[1] = highByte(LX);
    dataPacket[2] = lowByte(LX);
    
    //Serial.print(" LY: ");
    //Serial.print(analogRead(A9));
    unsigned int LY = analogRead(A9);
    dataPacket[3] = highByte(LY);
    dataPacket[4] = lowByte(LY);
  
  //  Serial.print(" RX: ");
  //  Serial.print(analogRead(A15));
    unsigned int RX = analogRead(A15);
    // Right X is "backwards" reading highest at left stick
    RX = map(RX, 0,1023,1023,0);
    dataPacket[5] = highByte(RX);
    dataPacket[6] = lowByte(RX);
    
  //  Serial.print(" RY: ");
  //  Serial.println(analogRead(A14));
    unsigned int RY = analogRead(A14);
    dataPacket[7] = highByte(RY);
    dataPacket[8] = lowByte(RY);
    
    // now send it
    if(!manager.sendtoWait(dataPacket, sizeof(dataPacket), cur_rx)){
      // oopsie
      Tft.drawString("X", 40,300,2,RED);
      sendFailCount++;
      
      // if failure happens 5 times in a row assume RX is gone
      // remove cur from known_rx and reset cur to MY
      if(sendFailCount >= 5){
        known_rx[cur_rx] = "";
        cur_rx = MY_ADDRESS;
      }
    }else{
      sendFailCount = 0;
    }
    
  }  // end of "if cur != MY"
}


