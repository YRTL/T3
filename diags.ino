unsigned long s2millis;

void sticks2screen(){
  if(millis() - s2millis < 5000)
    return;
  
  s2millis = millis();
  
  // title
  Tft.fillRectangle(0,0,240,20, BLUE);
  Tft.drawString("Diagnostics", 5,2,2,GREEN);
  
  // close button
  Tft.drawRectangle(220,0,20,19,GRAY1);
  Tft.drawString("X", 220,2,2,GRAY1);
  
  // wipe previous info from screen
  Tft.fillRectangle(0,21,240,299,BLACK);
  
  char leftXstring[14];
  String leftX = "Left X: " + String(analogRead(lStickX));
  leftX.toCharArray(leftXstring, 14);
  
  char leftYstring[14];
  String leftY = "Left Y: " + String(analogRead(lStickY));
  leftY.toCharArray(leftYstring, 14);
  
  char rightXstring[14];
  String rightX = "Right X: " + String(analogRead(rStickX));
  rightX.toCharArray(rightXstring, 14);
  
  char rightYstring[14];
  String rightY = "Right Y: " + String(analogRead(rStickY));
  rightY.toCharArray(rightYstring, 14);
  
  Tft.drawString("Raw stick values", 5,22,1,BLUE);
  Tft.fillRectangle(0,40,240,80,BLACK);
  Tft.drawString(leftXstring, 5,42,2,BLUE);
  Tft.drawString(leftYstring, 5,62,2,BLUE);
  Tft.drawString(rightXstring, 5,82,2,BLUE);
  Tft.drawString(rightYstring, 5,102,2,BLUE);
}
