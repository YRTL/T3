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

// how many receivers to keep track of
#define MAX_KNOWN_RX 5
// and a space to hold their names (indexed by address)
String known_rx[MAX_KNOWN_RX];

#define MY_ADDRESS 3
#define WALLE_ADDRESS 4

#define lStickX  A8
#define lStickY  A9
#define rStickX  A15
#define rStickY  A14

// touchscreen pins on a Mega2560
#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 54   // can be a digital pin, this is A0
#define XP 57   // can be a digital pin, this is A3

RH_NRF24 driver(53,49);
RHReliableDatagram manager(driver, MY_ADDRESS);

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

int action = 0;
int donePainting = 0;

int connButtonCount;
int connButtons[10][3];

int scanCount = 0;

int cur_rx;

uint8_t dataPacket[RH_NRF24_MAX_MESSAGE_LEN];
//uint8_t dataPacket[] = "Message! :-)";
uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];

void setup() {
  Tft.init();
  Tft.setDisplayDirect(LEFT2RIGHT);
  if (!manager.init()){
    Tft.drawString("Manager init failed", 5,22,1, RED);
    donePainting = 1;
  }
  
  if(scanCount == 0){
    //dataPacket[0] = 'Q';
    //dataPacket[1] = '?';
    //dataPacket[2] = (char)0;
    
//    if(!manager.sendtoWait(dataPacket, sizeof(dataPacket), WALLE_ADDRESS)){ //RH_BROADCAST_ADDRESS
//      Tft.drawString("0", 20,300,2,RED);
//    }else{
//      Tft.drawString("1", 20,300,2,GREEN);
//    }
  }
  
  Serial.begin(9600);
  known_rx[MY_ADDRESS] = "YRTL-T3";
//  known_rx[1] = "YRTLBot";
//  known_rx[2] = "Ruprecht";
//  known_rx[4] = "Wall-E";
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
    if(p.x > 220 && p.y > 300 && action == 0){
      // Diags screen
      action = 9;
      donePainting = 0;
    }

    // if Connect button is pressed on main screen
    if(p.x > 0 && (p.y >= 21 && p.y <= 46) && action == 0){
      // Connect screen
      action = 1;
      donePainting = 0;
    }

    if(p.x > 0 && (p.y >= 48 && p.y <= 73) && action == 0){
      // Scan screen
      action = 3;
      donePainting = 0;
    }

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

      // handle Scan button
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
    if(p.x > 220 && (p.y > 0 && p.y <= 20)){
      // back to Main screen
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
    if(action == 9)
      sticks2screen();
  }

//  Serial.print("LX: ");
//  Serial.print(analogRead(A8));
//  Serial.print(" LY: ");
//  Serial.print(analogRead(A9));
//  Serial.print(" RX: ");
//  Serial.print(analogRead(A15));
//  Serial.print(" RY: ");
//  Serial.println(analogRead(A14));
}


