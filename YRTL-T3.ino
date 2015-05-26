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

int scanButtonCount;
int scanButtons[10][3];


void setup() {
  Tft.init();
  Tft.setDisplayDirect(LEFT2RIGHT);
  if (!manager.init()){
    Tft.drawString("Manager init failed", 5,22,1, RED);
    donePainting = 1;
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
    
    // if Scan button is pressed on main screen
    if(p.x > 0 && (p.y >= 21 && p.y <= 46) && action == 0){
      // Scanning screen
      action = 1;
      donePainting = 0;
    }
    
    // use scanButtons array to build connect buttons
    if(action == 2 && scanButtonCount > 0){
      int lastY;
      for(int i=0; i<=scanButtonCount; i++){
        if(p.x > 0 && (p.y >= scanButtons[i][0] && p.y <= scanButtons[i][1])){
          if(scanButtons[i][2] == 255){
            action = 1;
            donePainting = 0;
          }else{
            doConnect(scanButtons[i][2]);
          }
        }
        lastY = scanButtons[i][1];
      }
      
      // handle Refresh button
      if(p.x > 0 && (p.y >= lastY && p.y <= lastY+25)){
        action = 1;
        donePainting = 0;
      }
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
