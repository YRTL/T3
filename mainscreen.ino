void mainScreen(){
  // print title
  Tft.fillRectangle(0,0,240,20, BLUE);
  char myName[10];
  known_rx[MY_ADDRESS].toCharArray(myName, 10);
  
  Tft.drawString(myName, 5,2,2,GREEN);
  
  // wipe previous info from screen
  Tft.fillRectangle(0,21,240,299,BLACK);
  
  if(cur_rx == MY_ADDRESS){
    // draw Connect button
    Tft.drawRectangle(0,21,230,25,WHITE);
    Tft.drawString("Connect->", 5, 23, 3, BLUE);
    
    // draw Scan button
    Tft.drawRectangle(0,48, 230,25,WHITE);
    Tft.drawString("Scan->", 5, 50, 3, BLUE);
  }else{
    Tft.drawString("Connected",5,23,3,BLUE);
    Tft.drawString("To",5,46,3,BLUE);
    char theName[10];
    known_rx[cur_rx].toCharArray(theName, 10);
    Tft.drawString(theName,5,69,3,GREEN);
    
    Tft.drawRectangle(0,95,239,40,WHITE);
    Tft.drawString("Disconnect", 5,100,3,GREEN);
    
  }
  
  // draw [*] options/settings button in lower right corner
  // FIXME - this should be a "real" button
  Tft.drawString("*", 221,301,2,GRAY1);
  Tft.drawRectangle(220,300,15,15,GRAY1);
  
  // set the done flag so's not to keep repainting the same thing
  donePainting = 1;
}
