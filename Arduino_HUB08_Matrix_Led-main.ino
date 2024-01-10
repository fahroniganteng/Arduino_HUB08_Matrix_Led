/* ********************************************************************
 * THIS DEMO TESTED ON ARDUINO UNO, NANO, PRO MINI (ATMEGA328P)
 * ********************************************************************
 * HUB08 (matrix led) to ARDUINO pin connection
 * LA / A    -> Digital Pin 4
 * LB / B    -> Digital Pin 5
 * LC / C    -> Digital Pin 6
 * LD / D    -> Digital Pin 7
 * S / CLK   -> Digital Pin 13
 * R1 / R    -> Digital Pin 11
 * OE / EN   -> Digital Pin 3
 * L / LAT / STB -> Digital Pin 2
 * 
 * ********************************************************************
 * If display in matrix led invert, change in HUB08SPI.cpp 
 * Find :
 *    SPI.transfer(*ptr);
 * Change to :
 *    SPI.transfer(~*ptr);
 * Detail, check in : 
 *    https://github.com/emgoz/HUB08SPI/issues/5
 * ********************************************************************/



#include <avr/pgmspace.h>
#include <SPI.h>
#include "HUB08SPI.h"
#include "TimerOne.h"
#include "Buffer.h"

#define WIDTH   64 // width of led matrix (pixel)
#define HEIGHT  16 // height of led matrix

HUB08SPI display;
uint8_t displaybuf[WIDTH * HEIGHT / 8];
Buffer buff(displaybuf, WIDTH, HEIGHT);

//make sure to put after create buffer
#include "ronnAnimation.h" 

void refresh() {
  display.scan();  //refresh a single line of the display
}
void showTitle(String icon, String text, bool topPosition=true){
  char* topMoving[] ={"P:300", "L:27", "E:0"};
  char* midMoving[] ={"D:4","P:300", "L:27", "E:0"};
  ronn.setFont(C_ICON);
  ronn.scrollText_D(icon,27,0,9,10);
  ronn.moveTo((topPosition?topMoving:midMoving), 10, 27, 0, 9, 8);
  ronn.setFont();
  ronn.scanText_L(text, 11, (topPosition?0:4), 10);
  delay(500);
}
void changeTitle(String text){
  ronn.setFont();
  ronn.clear_L(10,0,54,8);
  ronn.printText_RC(text,11,0);
  delay(500);
}
void setup(){
  Serial.begin(9600);
  display.begin(displaybuf, WIDTH, HEIGHT);
  Timer1.initialize(100);  //800 us -> refresh rate 1250 Hz per line / 16 = 78 fps
  Timer1.attachInterrupt(refresh);
  display.setBrightness(200);     //low brightness to save energy
  buff.clear();                   //clear display led matrix
  buff.bitmap(0,0,64,16,logo,1);  //bitmap logo
  delay(2000);
  ronn.clear_A1();
}
void loop(){
  showFont();
  animationDemo();
  moveDemo();
  clearDemo();
  showTitle("4","END DEMO",false);
  delay(5000);
  ronn.clearSlice_R();
}




/*
 * RONN ANIMATION DEMO FUNCTION
 * ************************************************************************************************
 */

//CEAR DEMO
void clearDemo(){
  showTitle("2","CLEAR",false);delay(500);
  ronn.move_U(4);delay(300);
  changeTitle("Clear Left");
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clear_L(0,8,64,8,CLEAR);
  changeTitle("Scroll Left");
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clear_L(0,8,64,8,SCROLL);
  changeTitle("Clear Right");
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clear_R(0,8,64,8,CLEAR);
  changeTitle("Scroll Right");
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clear_R(0,8,64,8,SCROLL);
  changeTitle("Clear Up");
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clear_U(0,8,64,8,CLEAR);
  changeTitle("Scroll Up");
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clear_U(0,8,64,8,SCROLL);
  changeTitle("Clear Down");
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clear_D(0,8,64,8,CLEAR);
  changeTitle("Scroll Down");
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clear_D(0,8,64,8,SCROLL);

  changeTitle("Slice Left");
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clearSlice_L(0,8,64,8);
  changeTitle("Slice Right");
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clearSlice_R(0,8,64,8);
  ronn.clear_U();
  
  showTitle("2","CLEAR ALL",false);delay(500);
  ronn.move_U(4);delay(300);
  changeTitle("Clear Left");
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clear_L(CLEAR);
  showTitle("2","Scroll Left");delay(500);
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clear_L();
  showTitle("2","Clear Right");delay(500);
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clear_R(CLEAR);
  showTitle("2","Scroll Right");delay(500);
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clear_R();
  showTitle("2","Clear Down");delay(500);
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clear_D(CLEAR);
  showTitle("2","Scroll Down");delay(500);
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clear_D();
  showTitle("2","Clear Up");delay(500);
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clear_U(CLEAR);
  showTitle("2","Scroll Up");delay(500);
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clear_U();
  showTitle("2","Slice Left");delay(500);
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clearSlice_L();
  showTitle("2","Slice Right");delay(500);
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clearSlice_R();

  showTitle("2","Animation 1");delay(500);
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clear_A1();
  showTitle("2","Animation 2");delay(500);
  ronn.printText("Animation Demo",0,9);delay(500);
  ronn.clear_A2();
}

//MOVE DEMO
void moveDemo(){
  showTitle("3","MOVE DEMO",false);delay(500);
  ronn.move_U(4);delay(300);
  changeTitle("Move All");
  ronn.printText_RC("Demo",0,9);delay(500);
  ronn.move_R(32);delay(500);
  ronn.move_U(8);delay(500);
  ronn.move_L(16);delay(500);
  ronn.move_D(16);delay(500);
  showTitle("3","Custom");
  changeTitle("Move");
  ronn.move_R(14);delay(500);
  ronn.move_D(4);delay(500);
  buff.rect(0,12,4,4,1);delay(500);
  char* moving[] ={"R:60", "P:300", "U:8", "P:300", "L:14", "E:0"};
  ronn.moveTo(moving, 20, 0, 12, 4, 4);delay(1000);
  ronn.clear_A2();
  //ronn.clearSlice_R();
}

//FONT DEMO
void showFont(){
  showTitle("0","FONT DEMO",false);delay(500);
  ronn.move_U(4);delay(300);
  changeTitle("Big Font");
  ronn.clear_R();
  ronn.setFont(B_STD);
  ronn.scrollText_LR("This is a big font demo from RONN ANIMATION - 1234567890",0,0,64,20);
  showTitle("0","Normal Font");
  ronn.scrollText_LR("Normal font example - CAPITAL - 1234567890",0,9,64,30);
  changeTitle("Small Font");
  ronn.setFont(S_STD);
  ronn.scrollText_LR("Small font example - CAPITAL - 1234567890",0,9,64,40);
  changeTitle("Custom");
  changeTitle("7 Segment");
  ronn.setFont(N_7SEGMENT);
  ronn.scrollText_LR("1234567890",0,9,64,30);
  changeTitle("Big Font");
  changeTitle("7 Segment");
  ronn.clear_L();
  ronn.setFont(B_7SEGMENT);
  ronn.scrollText_LR("1234567890",0,0,64,20);
}

//TEXT ANIMATION DEMO
void animationDemo(){
  showTitle("1","ANIMATION",false);
  ronn.move_U(4);delay(300);
  
  //Print text
  changeTitle("Print Text");
  ronn.printText("Animation Demo",0,9);delay(500);
  
  //Print Right
  changeTitle("Print Right");
  ronn.clear_D(0,8,64,8,CLEAR);delay(300);
  ronn.printText_R("Animation Demo",0,9);delay(500);
  changeTitle("Custom");
  changeTitle("Speed");
  ronn.clear_D(0,8,64,8,CLEAR);delay(300);
  ronn.printText_R("Animation Demo",0,9,50);delay(500);
  changeTitle("With Cursor");
  ronn.clear_D(0,8,64,8,CLEAR);delay(300);
  ronn.printText_RC("Animation Demo",0,9);delay(500);
  changeTitle("Custom");
  changeTitle("Speed");
  ronn.clear_D(0,8,64,8,CLEAR);delay(300);
  ronn.printText_RC("Animation Demo",0,9,50);delay(500);
  changeTitle("Big Font");
  ronn.clear_L(CLEAR);delay(300);
  ronn.setFont(B_STD);
  ronn.printText_RC("Example",0,0);delay(1000);
  ronn.clear_L();
  
  //Print Left
  showTitle("1","Print Left");
  ronn.clear_D(0,8,64,8,CLEAR);delay(300);
  ronn.printText_L("Animation Demo",0,9);delay(500);
  changeTitle("With Cursor");
  ronn.clear_D(0,8,64,8,CLEAR);delay(300);
  ronn.printText_LC("Animation Demo",0,9);delay(500);
  changeTitle("Big Font");
  ronn.clear_R(CLEAR);delay(300);
  ronn.setFont(B_STD);
  ronn.printText_LC("Example",0,0);delay(1000);
  ronn.clear_R();
  
  //Scan Left
  showTitle("1","Scan Left");
  ronn.clear_D(0,8,64,8,CLEAR);delay(300);
  ronn.scanText_L("Animation Demo",0,9);delay(500);
  changeTitle("Custom");
  changeTitle("Speed");
  ronn.clear_L(0,8,64,8,CLEAR);delay(300);
  ronn.scanText_L("Animation Demo",0,9,50);delay(500);
  changeTitle("Big Font");
  ronn.clear_R(CLEAR);delay(300);
  ronn.setFont(B_STD);
  ronn.scanText_L("Example",0,0);delay(1000);
  ronn.clear_L(CLEAR);

  //Scan Right
  showTitle("1","Scan Right");
  ronn.clear_R(0,8,64,8,CLEAR);delay(300);
  ronn.scanText_R("Animation Demo",0,9);delay(500);
  changeTitle("Big Font");
  ronn.clear_R(CLEAR);delay(300);
  ronn.setFont(B_STD);
  ronn.scanText_R("Example",0,0);delay(1000);
  ronn.clear_R(CLEAR);
  
  //Scroll text
  showTitle("1","Scroll Left");
  ronn.scrollText_LL("Animation",0,9,64);delay(500);
  changeTitle("Run Text");
  ronn.clear_D(0,8,64,8,CLEAR);delay(300);
  ronn.scrollText_LR("Ronn Animation Demo",0,9,64);delay(500);
  changeTitle("Custom");
  changeTitle("Speed");
  ronn.scrollText_LR("Ronn Animation Demo",0,9,64,50);delay(500);
  changeTitle("Custom");
  changeTitle("Position");
  buff.rect(0,9,15,7,1);
  buff.rect(49,9,15,7,1);
  for(int i=0;i<2;i++)//run 3x
    ronn.scrollText_LR("Ronn Animation Demo - RONN ANIMATION DEMO",16,9,32,40);delay(500);
  
  //Scroll Down
  changeTitle("Scroll Down");
  ronn.scrollText_D("Animation Demo",0,9,64);delay(500);
  ronn.scrollText_D("Animation",0,9,43);delay(500);
  ronn.scrollText_D("Demo",43,9,64-43);delay(500);
  
  changeTitle("Clock Demo");
  ronn.clear_D(0,8,64,8,CLEAR);delay(300);
  ronn.scrollText_D("06:30:55",11,8,64-11);
  delay(1000-160);
  for(int i=6;i<=9;i++){
    ronn.scrollText_D(String(i),36+11,8,5);
    delay(1000-160);//animatioan delay = 20ms * 8 step =160ms
  }
  ronn.scrollText_D("00",30+11,8,11);
  ronn.scrollText_D("1",21+11,8,5);
  changeTitle("Big Clock");
  ronn.clear_D();
  ronn.setFont(B_7SEGMENT);
  ronn.scrollText_D("06:30",9,0,64-11);
  for(int i=1;i<=5;i++){
    delay(400);
    buff.fillRect(31,0,2,16,0);
    delay(600);
    ronn.printText(":",31,0);
  }
  ronn.scrollText_D("1",45,0,10);delay(500);
  ronn.clear_D();
  
  //Scroll Up
  showTitle("1","Scroll Up");
  ronn.scrollText_U("Animation",12,8,43);delay(500);
  ronn.scrollText_U("   Demo",12,8,64-23);delay(500);
  changeTitle("Clock Demo");
  ronn.clear_U(0,8,64,8,CLEAR);delay(300);
  ronn.scrollText_U("06:30:55",11,8,64-11);
  delay(1000-160);
  for(int i=6;i<=9;i++){
    ronn.scrollText_U(String(i),36+11,8,5);
    delay(1000-160);
  }
  ronn.scrollText_U("00",30+11,8,11);
  ronn.scrollText_U("1",21+11,8,5);
  ronn.clear_U();
  
  
  //ronn.clearSlice_L(0,8,64,8);
}


