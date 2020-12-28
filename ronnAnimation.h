#include "Arduino.h"
#include "ronnFont.h"

class ronnAnimation{
  //PRIVATE CLASS ///////////////////////////////////////////////////////////////
  private:
    typedef  const uint8_t fontType_t;
    fontType_t  *_fontData;
    bool big_font;
    int min_font;
    int max_font;
    int height_font;
    int x_pos = 0;
    int y_pos = 0;
    #define SCROLL 1
    #define CLEAR 0
    
    int getFontPosition(byte c){
      int pos_font = 5;
      int charWidth;
      for (uint16_t i = min_font; i < c; i++){
        charWidth = pgm_read_byte_near(_fontData + pos_font);
        pos_font += charWidth +1;
      }
      return pos_font;
    }



    /*
     * *******************************************************************************************************************
     * PRINT TEXT
     * *******************************************************************************************************************
     */
    void writeByte(int x, int y, uint8_t data, int s){
      //if (x >= WIDTH ||y >= HEIGHT || x+8<=0 || y < 0) return; //outside already check in buff.setPixel
      for (uint16_t row=0; row<height_font; row++){
      bitRead(data,row)==1?
        buff.setPixel(x,y+row,1):
        buff.setPixel(x,y+row,0);
      delay(s);
      }
    }

    void printChar(byte chr, int y ,int s, int m){ //char,Y,speed,mode(0:none;1:cursor;2:scan)
      //if( chr < min_font || chr > max_font || x_pos > WIDTH)return; //already check in buff.setPixel
      int pos_font = getFontPosition(chr);
      int pos_font_big = getFontPosition(chr + 128);
      int charWidth = pgm_read_byte_near(_fontData + pos_font);
      int charWidthBig = pgm_read_byte_near(_fontData + pos_font_big);
      pos_font++;
      pos_font_big++;
      //Serial.println(big_font);
      //write char dari kiri --> kolom 1 (atas ke bawah), kolom 2 (atas ke bawah) dst...
      for (uint16_t col = 0; col < charWidth; col++){
        if(m==1)buff.fillRect(x_pos,y,2,height_font,1);
        if(m==2)buff.fillRect(x_pos-1,y,1,height_font,0);
        if(big_font){
          writeByte(x_pos,y,pgm_read_byte_near(_fontData + pos_font_big + col),0);
          writeByte(x_pos++,y+8,pgm_read_byte_near(_fontData + pos_font + col),0);
        }
        else
          writeByte(x_pos++,y,pgm_read_byte_near(_fontData + pos_font + col),0);
        
        delay(s);
      }
      
      //print space betwen char
      if(m==2)buff.fillRect(x_pos-1,y,1,height_font,0);
      buff.lineDown(x_pos,y,height_font,0);
      x_pos++;
    }
    int getLastPositionText(char* string){// X position last text
      int x = 0;
      int pos_font;
      for(int i=0;i<strlen(string);i++){
        pos_font = getFontPosition(string[i]);
        x += pgm_read_byte_near(_fontData + pos_font) + 1;
      }
      return x - 1;
    }
    void printCharLeft(byte chr, int y ,int s, int m){ //char,Y,speed,mode(0:none;1:cursor;2:scan)
      int pos_font = getFontPosition(chr);
      int pos_font_big = getFontPosition(chr + 128);
      int charWidth = pgm_read_byte_near(_fontData + pos_font);
      int charWidthBig = pgm_read_byte_near(_fontData + pos_font_big);
      pos_font++;
      pos_font_big++;
      for (uint16_t col = charWidth; col > 0; col--){
        if(m==1)buff.fillRect(x_pos-1,y,2,height_font,1);
        if(big_font){
          writeByte(x_pos,y,pgm_read_byte_near(_fontData + pos_font_big + col-1),0);
          writeByte(x_pos--,y+8,pgm_read_byte_near(_fontData + pos_font + col-1),0);
        }
        else
          writeByte(x_pos--,y,pgm_read_byte_near(_fontData + pos_font + col-1),0);
        if(x_pos < WIDTH) delay(s); //outside already check in buff.setPixel, but delay still execute.
        if(m==2)buff.fillRect(x_pos+1,y,1,height_font,0);
      }
      buff.lineDown(x_pos,y,height_font,0);
      x_pos--;
      if(x_pos < WIDTH) delay(s); //outside already check in buff.setPixel, but delay still execute.
    }

    /* *******************************************************************************************************************
     * VERTICAL SCROLL
     * *******************************************************************************************************************
     */
    void scrollCharVertical(byte c, int x, int y, int w, int row){
      if( c < min_font || c > max_font || x_pos > WIDTH)return;
      int pos_font = getFontPosition(c);
      int pos_font_big = getFontPosition(c + 128);
      int charWidth = pgm_read_byte_near(_fontData + pos_font);
      int charWidthBig = pgm_read_byte_near(_fontData + pos_font_big);
      pos_font++;
      pos_font_big++;
      for (uint16_t col = 0; col < charWidth; col++){
        if(x_pos+col-x<w){
          int buff_chr = big_font && row<8?
            buff_chr = pgm_read_byte_near(_fontData + pos_font_big + col):
            buff_chr = pgm_read_byte_near(_fontData + pos_font + col);
          bitRead(buff_chr,(row>=8?row-8:row))==1?
            buff.setPixel(x_pos+col,y,1):
            buff.setPixel(x_pos+col,y,0);
        }
      }
      x_pos += charWidth+1;
    }

    /* *******************************************************************************************************************
     * HORISONTAL SCROLL
     * *******************************************************************************************************************
     */
    void scrollChar_LR(byte c, int x, int y, int w, int s){ //char,X,Y,width,speed
      if( c < min_font || c > max_font)return;
      int pos_font = getFontPosition(c);
      int pos_font_big = getFontPosition(c + 128);
      int charWidth = pgm_read_byte_near(_fontData + pos_font);
      int charWidthBig = pgm_read_byte_near(_fontData + pos_font_big);
      pos_font++;
      pos_font_big++;
      for (uint16_t col = 0; col < charWidth; col++){
        if(big_font){
          writeByte(x+w-1, y, pgm_read_byte_near(_fontData + pos_font_big + col), 0);
          writeByte(x+w-1, y + 8, pgm_read_byte_near(_fontData + pos_font + col), 0);
        }
        else
          writeByte(x+w-1, y, pgm_read_byte_near(_fontData + pos_font + col), 0);
        delay(s);
        buff.scrollLeft(x, y, w, height_font);
      }
      delay(s);
      buff.scrollLeft(x, y, w, height_font);
    }
    void scrollChar_LL(byte c, int x, int y, int w, int s){ //char,X,Y,width,speed
      if( c < min_font || c > max_font)return;
      int pos_font = getFontPosition(c);
      int pos_font_big = getFontPosition(c + 128);
      int charWidth = pgm_read_byte_near(_fontData + pos_font);
      int charWidthBig = pgm_read_byte_near(_fontData + pos_font_big);
      pos_font++;
      pos_font_big++;
      for (uint16_t col = 0; col < charWidth; col++){
        if(x_pos < 1)return;
        if(big_font){
          writeByte(x+w-1, y, pgm_read_byte_near(_fontData + pos_font_big + col), 0);
          writeByte(x+w-1, y + 8, pgm_read_byte_near(_fontData + pos_font + col), 0);
        }
        else
          writeByte(x+w-1, y, pgm_read_byte_near(_fontData + pos_font + col), 0);
        delay(s);
        buff.scrollLeft(x, y, w, height_font);
        x_pos--;
      }
      delay(s);
      buff.scrollLeft(x, y, w, height_font);
      x_pos--;
    }


    /* *******************************************************************************************************************
     * MOVE
     * *******************************************************************************************************************
     */
    bool checkMove(char* str,int sp, int x, int y, int w, int h){
      char k[5];//key (move to)
      char b[5];//buffer just for check format --> :
      int st = 0;//step
      sscanf(str, "%1s%1s%d", k, b, &st);
      //Serial.println(String(str) + " = " + String(k) + "-" + String(b) + " => " + String(st));
      if(b[0]!=':' || st<1 || st==NULL || k[0]=='E') return false;
      else if(k[0]=='U') move_U(st,sp,x,y,w,h);//Move up
      else if(k[0]=='R') move_R(st,sp,x,y,w,h);//Move right
      else if(k[0]=='D') move_D(st,sp,x,y,w,h);//Move down
      else if(k[0]=='L') move_L(st,sp,x,y,w,h);//Move left
      else if(k[0]=='P') delay(st);//parking(delay)
      else if(k[0]=='E') return false; //End Move
      else return false;
      return true;
    }














  //PUBLIC CLASS ///////////////////////////////////////////////////////////////
  public:
    ronnAnimation(){
      setFont();
    }

    bool setFont(fontType_t *f=font_DEFAULT){ //variable font name on ronnFont.h 
      if (f != _fontData){
        _fontData = (f == nullptr ? font_DEFAULT : f);
      }
      big_font = pgm_read_byte_near(_fontData + 0)=='T'?true:false;
      min_font = pgm_read_byte_near(_fontData + 2);
      max_font = pgm_read_byte_near(_fontData + 3);
      height_font = pgm_read_byte_near(_fontData + 4);
      return(true);
    }
    

    /*
     * *******************************************************************************************************************
     * PRINT TEXT
     * *******************************************************************************************************************
     *    printText(String string, int x, int y)   //string,X,Y
     *    printText_R(String string, int x, int y, int s)  //string,X,Y,speed --> print right with delay animation
     *    printText_RC(String string, int x, int y, int s)  //string,X,Y,speed --> print right with delay and cursor animation
     *    printText_L(String string, int x, int y, int s)  //string,X,Y,speed --> print left with delay animation (x = pixel in right position / end text)
     *    printText_LC(String string, int x, int y, int s)  //string,X,Y,speed --> print left with delay and cursor animation 
     *    scanText_L(String string, int x, int y, int s)  //string,X,Y,speed --> scan left animation
     *    scanText_R(String string, int x, int y, int s)  //string,X,Y,speed --> scan right animation
     * 
     * Font library from MD_MAX72xx / Parola (just font byte --> not use MD_MAX72xx library)
     * Default small font from HUB08SPI not use --> i like font design from MD_MAX72xx, so i use from there..... ^___^
     * You can use "Font Builder.xlsm" create new font (modified from "Font Builder v2.xlsm" in MD_MAX72xx library)
     * Big font (16x16) use from HUB08SPI library, check on function printBigText(string,X)
     * *******************************************************************************************************************
     */
    void printText(String text, int x=0, int y=0){ //string,X,Y
      x_pos = x;
      char* string = text.c_str();
      while (*string != '\0'){
        byte c = *string;
        printChar(c,y,0,0);
        string++;
      }
    }
    void printText_R(String text, int x=0, int y=0, int s=10){ //string,X,Y,speed
      x_pos = x;
      char* string = text.c_str();
      while (*string != '\0'){
        byte c = *string;
        printChar(c,y,s,0);
        string++;
      }
    }
    void printText_RC(String text, int x=0, int y=0, int s=10){ //string,X,Y,speed
      x_pos = x;
      char* string = text.c_str();
      while (*string != '\0'){
        byte c = *string;
        printChar(c,y,s,1);
        string++;
      }
    }
    void printText_L(String text, int x=0, int y=0, int s=10){ //string,X,Y,speed
      char* string = text.c_str();
      x_pos = x + getLastPositionText(string) - 1;
      for(int i=strlen(string)-1;i>=0;i--){
        printCharLeft(string[i],y,s,0);
      }
    }
    void printText_LC(String text, int x=0, int y=0, int s=10){ //string,X,Y,speed
      char* string = text.c_str();
      x_pos = x + getLastPositionText(string) - 1;
      for(int i=strlen(string)-1;i>=0;i--){
        printCharLeft(string[i],y,s,1);
      }
    }

    void scanText_L(String text, int x=0, int y=0, int s=10){ //string,X,Y,speed
      char* string = text.c_str();
      x_pos = x + getLastPositionText(string) - 1;
      for(int i=strlen(string)-1;i>=0;i--){
        printCharLeft(string[i],y,s,2);
      }
      printText_RC(text, x, y, s/4);
    }
    void scanText_R(String text, int x=0, int y=0, int s=10){ //string,X,Y,speed
      x_pos = x;
      char* string = text.c_str();
      while (*string != '\0'){
        byte c = *string;
        printChar(c,y,s,2);
        string++;
      }
      printText_LC(text, x, y, s/4);
    }

    /* *******************************************************************************************************************
     * VERTICAL SCROLL
     * *******************************************************************************************************************
     *    scrollText_U(String string, int x, int y, int w, int s=20)    //String,X,Y,width,speed --> scroll text up
     *    scrollText_D(String string, int x, int y, int w, int s=20)    //String,X,Y,width,speed --> scroll text down
     * *******************************************************************************************************************
     */
    void scrollText_U(String string, int x=0, int y=0, int w=WIDTH, int s=20){ //String,X,Y,width,speed
      for (uint16_t row=0; row<height_font; row++){
        x_pos = x;
        buff.scrollUp(x,y,w,height_font);
        buff.lineAcross(x,y+height_font-1,w,0);
        char* chr = string.c_str();
        while (*chr != '\0'){
          byte c = *chr;
          scrollCharVertical(c,x,y+height_font-1,w,row);
          chr++;
        }
        delay(s);
      }
    }
    void scrollText_D(String string, int x=0, int y=0, int w=WIDTH, int s=20){ //String,X,Y,width,speed
      for (uint16_t row=height_font; row>0; row--){
        x_pos = x;
        buff.scrollDown(x,y,w,height_font);
        buff.lineAcross(x,y,w,0);
        //char* chr = string;
        char* chr = string.c_str();
        while (*chr != '\0'){
          byte c = *chr;
          scrollCharVertical(c,x,y,w,row-1);
          chr++;
        }
        delay(s);
      }
    }


    /* *******************************************************************************************************************
     * HORISONTAL SCROLL
     * *******************************************************************************************************************
     * NOTE : To left only, scroll to right cannot read text ^___^
     * 
     * Scroll to left until first character in X position (until left character)
     *    scrollText_LL(String string, int x, int y, int w, int s) //String,X,Y,width,speed (height auto from font height)
     *    
     * Scroll to left until last character (until right character)
     * You can use this function for continues scroll text --> put on loop
     *    scrollText_LR(String string, int x, int y, int w, int s) //String,X,Y,width,speed (height auto from font height)
     *    
     * *******************************************************************************************************************
     */
    void scrollText_LR(char* string, int x=0, int y=0, int w=WIDTH, int s=20){//String,X,Y,width,speed (height auto from font height)
      if(x<0) return;
      while (*string != '\0'){
        byte c = *string;
        scrollChar_LR(c,x,y,w,s);
        string++;
      }
      for (int a=x+w-1;a>0;a--){ 
        buff.scrollLeft(x, y, w, height_font);
        delay(s);
      }
    }
    void scrollText_LL(char* string, int x=0, int y=0, int w=WIDTH, int s=10){ //string,X,Y,width,speed
      if(x<0) return;
      x_pos=w-1;
      while (*string != '\0'){
        byte c = *string;
        scrollChar_LL(c,x,y,w,s);
        string++;
      }
      
      if(x_pos > 1){
        for (uint16_t i = 0 ; i < x_pos ; i++){
          buff.scrollLeft(x, y, w, height_font);
          delay(s);
        }
      }
    }


    /* *******************************************************************************************************************
     * BIG FONT PRINT
     * *******************************************************************************************************************
     *    printBigText(char* string, int x){ //string,X --> 16 pixel height, y always 0
     *    
     * Font library from HUB08SPI
     * setFont not use in here
     * *******************************************************************************************************************
     */
    void printBigChar(byte c, int x, int y){ //char,X,Y
       byte l = pgm_read_byte_near(font_BIG + (c * 33))+1;  //get pixel width of character
      for (int a=0;a<16;a++){
        buff.writeByte(x,y+a,pgm_read_byte_near(font_BIG + (c * 33) +a*2 +1),1);
        buff.writeByte(x+8,y+a,pgm_read_byte_near(font_BIG + (c * 33) +a*2 +2),1);
      }
    }
    void printBigText(char* string, int x){ //string,X --> 16 pixel height, y always 0
      while (*string){
        byte c = *string-32;
        printBigChar(c,x,0);
        x+=pgm_read_byte_near(font_BIG + (c * 33))+1;
        string++;
      }
    }



    /* *******************************************************************************************************************
     * CLEAR ANIMATION
     * *******************************************************************************************************************
     *  Clear spesific location
     *    clear_L(int x, int y, int w, int h, int m)  //X,Y,width,height,mode(SCROLL,CLEAR) --> clear left
     *    clear_R(int x, int y, int w, int h, int m)  //X,Y,width,height,mode(SCROLL,CLEAR) --> clear right
     *    clear_D(int x, int y, int w, int h, int m)  //X,Y,width,height,mode(SCROLL,CLEAR) --> clear down
     *    clear_U(int x, int y, int w, int h, int m)  //X,Y,width,height,mode(SCROLL,CLEAR) --> clear up
     *    
     *  Clear all pixel on display
     *    clear_L(int m)  //mode(SCROLL,CLEAR) --> clear left
     *    clear_R(int m)  //mode(SCROLL,CLEAR) --> clear right
     *    clear_D(int m)  //mode(SCROLL,CLEAR) --> clear down
     *    clear_U(int m)  //mode(SCROLL,CLEAR) --> clear up
     *  
     *  Clear slice
     *    clearSlice_L(int x, int y, int w, int h)  //X,Y,width,height --> clear slice left
     *    clearSlice_R(int x, int y, int w, int h)  //X,Y,width,height --> clear slice right
     *    clearSlice_L() // all pixel --> clear slice left
     *    clearSlice_R() // all pixel --> clear slice right
     *    
     *  Other clear animation
     *    clear_A1() // all pixel --> random pixel and scroll to center
     *    clear_A2() // all pixel --> blink out
     * *******************************************************************************************************************
     */


    void clear_L(int x, int y, int w, int h, int m=SCROLL){//X,Y,width,height,mode(SCROLL,CLEAR)
      for (uint16_t col = x+w; col >x; col--){
        m==SCROLL?
          buff.scrollLeft(x,y,w,h):
          buff.line(col-1,y,col-1,y+h-1,OFF);
        delay(5);
        }//=1+62-1// 
    }
    uint8_t clear_L(int m=SCROLL){ //mode
      clear_L(0,0,WIDTH,HEIGHT,m);
    }
    void clear_R(int x, int y, int w, int h,int m=SCROLL){//X,Y,width,height,mode(SCROLL,CLEAR)
      for (uint16_t col = x; col < x+w; col++){
        m==SCROLL?
          buff.scrollRight(x,y,w,h):
          buff.line(col,y,col,y+h-1,OFF);
        delay(5);
      }
    }
    uint8_t clear_R(int m=SCROLL){ //mode
      clear_R(0,0,WIDTH,HEIGHT,m);
    }
    void clear_D(int x, int y, int w, int h, int m=SCROLL){//X,Y,width,height,mode(SCROLL,CLEAR)
      for (uint16_t row = y; row < y+h; row++){
        if(m==SCROLL){
          buff.scrollDown(x,y,w,h);
          buff.lineAcross(x,y,w,0);
        }
        else
          buff.line(x,row,x+w-1,row,OFF);
        delay(20);
      }
    }
    uint8_t clear_D(int m=SCROLL){ //mode
      clear_D(0,0,WIDTH,HEIGHT,m);
    }
    void clear_U(int x, int y, int w, int h, int m=SCROLL){//X,Y,width,height
      for (uint16_t row = 0; row < h; row++){
        if(m==SCROLL){
          buff.scrollUp(x,y,w,h);
          buff.lineAcross(x,y+h-1,w,0);
        }
        else
          buff.line(x,y+h-row-1,x+w-1,y+h-row-1,OFF);
        delay(20);
      }
    }
    uint8_t clear_U(int m=SCROLL){ //mode
      clear_U(0,0,WIDTH,HEIGHT,m);
    }
    void clearSlice_L(int x, int y, int w, int h){//X,Y,width,height
      for (uint16_t col = x; col < x+w+1; col++){
        bool bufPixel = 0;
        for (uint16_t i = 0; i < h; i++){//escape empty column pixel
          bufPixel|=buff.getPixel(col,y+i);
        }
        if(!bufPixel)continue;
        else if(col-x==0)buff.line(x,y,x,y+h-1,0);//first column
        else{
          for (uint16_t i = x; i < col+1; i++){
            buff.scrollLeft(x,y,col-x+1,y+h);
            //delay(col-x<10?15-(col-x):5);
            if(i-x+1>8)continue;//make fast animation...
            delay(5);
          }
        }
        buff.line(x,y,x,y+h-1,0);
      }
    }
    uint8_t clearSlice_L(){
      clearSlice_L(0,0,WIDTH,HEIGHT);
    }
    void clearSlice_R(int x, int y, int w, int h){//X,Y,width,height
      for (uint16_t col = x+w; col > x; col--){
        bool bufPixel = 0;
        for (uint16_t i = 0; i < h; i++){//escape empty column pixel
          bufPixel|=buff.getPixel(col-1,y+i);
        }
        if(!bufPixel)continue;
        else if(col==x+w)buff.line(col-1,y,col-1,y+h-1,0);//first column
        else{
          for (uint16_t i = col-1; i < w+x; i++){
            buff.scrollRight(col-1,y,x+w-col+1,y+h);
            if(i+1-col>8)continue; //make fast animation...
            delay(7);
          }
        }
        buff.line(x+w-1,y,x+w-1,y+h-1,0);
      }
    }
    uint8_t clearSlice_R(){
      clearSlice_R(0,0,WIDTH,HEIGHT);
    }
    
    
    void clear_A1(){
      for (uint16_t t = 0; t < 2000; t++){
        uint16_t x = random(0,WIDTH);
        uint16_t y = random(0,HEIGHT);
        display.drawPoint(x,y,1);
        delay(1);
      }
      
      for (uint16_t x = 0; x < WIDTH/2; x++){
        display.drawRect(x,0,1,HEIGHT,0);
        display.drawRect(WIDTH-1-x,0,1,HEIGHT,0);
        delay(10);
      }
    }
    void clear_A2(){
      for (uint16_t t = 0; t < 4; t++){
        buff.invert();
        delay(100);
      }
      display.drawRect(0,0,WIDTH,HEIGHT,1);
      delay(100);
      buff.clear();
    }





    /* ***********************************************************************************************************************************
     * MOVE ANIMATION
     * ***********************************************************************************************************************************
     * Move
     *    move_U(int st=1, int sp=35, int x=0, int y=0, int w=WIDTH, int h=HEIGHT){ //step,speed,X,Y,width,height --> move up
     *    move_L(int st=1, int sp=35, int x=0, int y=0, int w=WIDTH, int h=HEIGHT){ //step,speed,X,Y,width,height --> move left
     *    move_R(int st=1, int sp=35, int x=0, int y=0, int w=WIDTH, int h=HEIGHT){ //step,speed,X,Y,width,height --> move right
     *    move_D(int st=1, int sp=35, int x=0, int y=0, int w=WIDTH, int h=HEIGHT){ //step,speed,X,Y,width,height --> move down
     * 
     * Move To
     *    moveTo(char* moving[], int sp=35, int x=0, int y=0, int w=WIDTH, int h=HEIGHT){//array direction & step,speed,X,Y,width,height
     * example :
     *    char* moving[] = {"D:6", "P:500", "L:20", "U:2","R:41"};
     *    ronn.moveTo(moving);
     * Result:
     *    move down 6 pixel --> Parking 500ms --> move left 20 pixel --> move up 2 pixel --> move right 41 pixel
     * ***********************************************************************************************************************************
     */

    void move_L(int st=1, int sp=35, int x=0, int y=0, int w=WIDTH, int h=HEIGHT){ //step,speed,X,Y,width,height
      //x_pos = x;
      for (int i = 0; i < st; i++){
        buff.scrollLeft(x-1-i,y,w+1,h);//Serial.println(String(x_pos) + ", " + String(y_pos));
        x_pos--;
        delay(sp);
      }
    }
    void move_R(int st=1, int sp=35, int x=0, int y=0, int w=WIDTH, int h=HEIGHT){ //step,speed,X,Y,width,height
      //x_pos = x;
      for (int i = 0; i < st; i++){
        buff.scrollRight(x+i,y,w+1,h);//Serial.println(String(x_pos) + ", " + String(y_pos));
        x_pos++;
        delay(sp);
      }
    }
    void move_D(int st=1, int sp=35, int x=0, int y=0, int w=WIDTH, int h=HEIGHT){ //step,speed,X,Y,width,height
      //y_pos = y;
      //SOMTHING WRONG IN buff.scrollDown
      //Some location move_D error ^___^
      for (int i = 0; i < st; i++){
        buff.scrollDown(x,y+i,w,h+1);
        buff.line(x,y+i,x+w,y+i,0);//Serial.println(String(x_pos) + ", " + String(y_pos));
        y_pos++;
        delay(sp);
      }
    }
    void move_U(int st=1, int sp=35, int x=0, int y=0, int w=WIDTH, int h=HEIGHT){ //step,speed,X,Y,width,height
      //y_pos = y; 
      for (int i = 0; i < st; i++){
        buff.scrollUp(x,y-i-1,w,h+1);
        buff.line(x,y+h-i-1,x+w,y+h-i-1,0);//Serial.println(String(x_pos) + ", " + String(y_pos));
        y_pos--;
        delay(sp);
      }
    }

    
    void moveTo(char* moving[], int sp=35, int x=0, int y=0, int w=WIDTH, int h=HEIGHT){//array direction & step,speed,X,Y,width,height
      int i = 0;
      x_pos = x;
      y_pos = y;
      while(checkMove(moving[i],sp,x_pos,y_pos,w,h)) i++;
    }


     

//END CLASS ///////////////////////////////////////////////////////////////
};


//auto call class
ronnAnimation ronn;
