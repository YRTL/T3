void connectScreen(){
  connButtonCount = 0;

  // title
  Tft.fillRectangle(0,0,240,20, BLUE);
  Tft.drawString("Connect", 5,2,2,GREEN);

  // close button
  Tft.drawRectangle(220,0,20,19,GRAY1);
  Tft.drawString("X", 220,2,2,GRAY1);

  // wipe previous info from screen
  Tft.fillRectangle(0,21,240,299,BLACK);

  int newY = 21;
  
  // loop through known_rx[] and build a list o' connect buttons
  for(int i=0;i<MAX_KNOWN_RX;i++){
    if(i != MY_ADDRESS && known_rx[i].length()){
      char theName[10];
      known_rx[i].toCharArray(theName, 10);      
      Tft.drawRectangle(0,newY,230,25,WHITE);
      Tft.drawString(theName, 5, newY+2, 3, GREEN);
      connButtons[connButtonCount][0] = newY;
      newY += 28;
      connButtons[connButtonCount][1] = newY;
      connButtons[connButtonCount][2] = i;
      connButtonCount++;
    }
  }
  if(connButtonCount == 0){
    Tft.drawString("Nothing", 5,21,3, RED);
    Tft.drawString("Found", 5, 46, 3, RED);
    connButtonCount = 1;
    newY = 70;
  }

  // draw a scan button to refresh list
  Tft.drawRectangle(0,newY,230,25,WHITE);
  Tft.drawString("Scan", 5, newY+2, 3, GREEN);
  connButtons[connButtonCount][0] = newY;
  connButtons[connButtonCount][1] = newY+28;
  connButtons[connButtonCount][2] = 255;

  action = 2;
  donePainting = 1;
}

// this sets cur_rx when a connect button is pushed
// FIXME needs to display a per-rx screen showing
// um, something...
void doConnect(int inAddr){
  cur_rx = inAddr;
  char addrString[3];
  String(inAddr).toCharArray(addrString, 3);
  Tft.fillRectangle(0,300,50,20,BLACK);
  Tft.drawString(addrString, 2,300,2,GREEN);
  action = 0;
  donePainting = 0;
}

void disConnect(){
  // disconnect
  // first blank the existing packet
  memset(dataPacket,0,sizeof(dataPacket));
  dataPacket[0] = 'c';
  dataPacket[1] = 'x';
  dataPacket[2] = 'x';
  dataPacket[3] = 'x';
  manager.sendtoWait(dataPacket,sizeof(dataPacket),cur_rx);
  cur_rx = MY_ADDRESS;
}

void connectedScreen(){
  // title
  Tft.fillRectangle(0,0,240,20, BLUE);
  char nameString[10];
  String(known_rx[cur_rx]).toCharArray(nameString, 10);
  Tft.drawString(nameString, 5,2,2,GREEN);

  // close button
  Tft.drawRectangle(220,0,20,19,GRAY1);
  Tft.drawString("X", 220,2,2,GRAY1);

  // wipe previous info from screen
  Tft.fillRectangle(0,21,240,299,BLACK);
  
  donePainting = 1;
}
