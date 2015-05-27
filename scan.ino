unsigned long scanMillis;

void scanScreen(){
  // run the scan on first loop
  if(scanCount == 0){
    //dataPacket[0] = 'Q';
    //dataPacket[1] = '?';
    //dataPacket[2] = (char)0;
    dataPacket[0] = (char)'q';
    dataPacket[1] = (char)0;
    if(!manager.sendtoWait(dataPacket, sizeof(dataPacket), RH_BROADCAST_ADDRESS)){ //RH_BROADCAST_ADDRESS);
      Tft.drawString("0", 20,300,2,RED);
    }else{
      Tft.drawString("1", 20,300,2,GREEN);
    }
  }
  
  if(manager.available()){
    Tft.drawString("1", 30,300,2,GREEN);
    uint8_t len = sizeof(buf);
    uint8_t from;
    
    // we'll take anything that answers while "scan" screen is running
    if(manager.recvfromAck(buf, &len, &from)){
      //Tft.drawString((char*)buf, 10,300,2,GREEN);
      known_rx[from] = (char*)buf;
    }
  }
  
  if(millis() - scanMillis < 3000)
    return;
  scanMillis = millis();

  if(scanCount > 5){
    scanCount = 0;
    action = 1;
    return;
  }
  
  // title
  Tft.fillRectangle(0,0,240,20, BLUE);
  Tft.drawString("Scan", 5,2,2,GREEN);
  
  // close button
  Tft.drawRectangle(220,0,20,19,GRAY1);
  Tft.drawString("X", 220,2,2,GRAY1);
  
  // wipe previous info from screen
  Tft.fillRectangle(0,21,240,299,BLACK);
  
  Tft.drawString("Scanning", 5, 23, 3, GREEN);
  char scanCountString[3];
  String(scanCount).toCharArray(scanCountString, 3);
  Tft.drawString(scanCountString, 2,300,2,GREEN);
  
  Tft.drawRectangle(0,48,230,25,WHITE);
  Tft.drawString("Cancel", 5, 50, 3, GREEN);
  
  scanCount++;

}

