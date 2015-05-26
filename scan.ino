unsigned long scanMillis;

void scanScreen(){
  scanButtonCount = 0;
  if(millis() - scanMillis < 5000)
    return;
  // title
  Tft.fillRectangle(0,0,240,20, BLUE);
  Tft.drawString("Scan", 5,2,2,GREEN);
  
  // close button
  Tft.drawRectangle(220,0,20,19,GRAY1);
  Tft.drawString("X", 220,2,2,GRAY1);
  
  // wipe previous info from screen
  Tft.fillRectangle(0,21,240,299,BLACK);
  
  int newY = 21;
  for(int i=0;i<MAX_KNOWN_RX;i++){
    if(i != MY_ADDRESS && known_rx[i].length()){
      char theName[10];
      known_rx[i].toCharArray(theName, 10);
      Tft.drawRectangle(0,newY,230,25,WHITE);
      Tft.drawString(theName, 5, newY+2, 3, GREEN);
      scanButtons[scanButtonCount][0] = newY;
      newY += 28;
      scanButtons[scanButtonCount][1] = newY;
      scanButtons[scanButtonCount][2] = i;
      scanButtonCount++;
    }
  }
  if(scanButtonCount == 0){
    Tft.drawString("Nothing", 5,21,3, RED);
    Tft.drawString("Found", 5, 46, 3, RED);
  }
  scanButtonCount = 1;
  newY = 70;
  Tft.drawRectangle(0,newY,230,25,WHITE);
  Tft.drawString("Refresh", 5, newY+2, 3, GREEN);
  scanButtons[scanButtonCount][0] = newY;
  scanButtons[scanButtonCount][1] = newY+28;
  scanButtons[scanButtonCount][2] = 255;

  

  
  action = 2;
  donePainting = 1;
}

void doConnect(int inAddr){
  char addrString[3];
  String(inAddr).toCharArray(addrString, 3);
  Tft.fillRectangle(0,300,50,20,BLACK);
  Tft.drawString(addrString, 2,300,2,GREEN); 
}
