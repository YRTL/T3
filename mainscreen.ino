void mainScreen(){
  // print title
  Tft.fillRectangle(0,0,240,20, BLUE);
  char myName[10];
  known_rx[MY_ADDRESS].toCharArray(myName, 10);
  
  Tft.drawString(myName, 5,2,2,GREEN);
  
  // wipe previous info from screen
  Tft.fillRectangle(0,21,240,299,BLACK);
  
  // draw Connect button
  Tft.drawRectangle(0,21,230,25,WHITE);
  Tft.drawString("Connect->", 5, 23, 3, BLUE);
  
  // draw Scan button
  Tft.drawRectangle(0,48, 230,25,WHITE);
  Tft.drawString("Scan->", 5, 50, 3, BLUE);
  
  // draw [*] options/settings button in lower right corner
  // FIXME - this should be a "real" button
  Tft.drawString("*", 221,301,2,GRAY1);
  Tft.drawRectangle(220,300,15,15,GRAY1);
  
  // set the done flag so's not to keep repainting the same thing
  donePainting = 1;
}
