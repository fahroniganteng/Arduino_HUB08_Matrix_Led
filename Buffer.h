/*-------------------------------------------------------------------------------------------------
//    *** Change Log / Version history ***
//
// 0.1 -  A basic Graphics library for buffers,
//        made to use it with any low res (up to 256 x 256 pix) monochrome LCD
//        The LCD must have a width which is divisible by 8 and
//        the data has to be stored byte-wise (1 bit = 1 pixel) starting at the top left.
//        For further information on how to use the functions, read the Documentation.pdf
//        Made by Marvin G. aka MGOS  / Apr 28 2013
//        request, bug reports and qustions to: marv.mgos@gmail.com
//-----------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------
//        You are allowed to copy, distribute or modify the source code of this library.
//        Any modification you make should be added to the change log / version history
//        in this document and at the top of each source file, including the author,
//        the date and all changes made.
//
//        Release into public domain.
//-----------------------------------------------------------------------------------------------*/
#ifndef BUFFER_h
#define BUFFER_h

#include "Arduino.h"
#include "inttypes.h"


//Drawing Modes
#define OFF     0
#define CLEAR   0
#define ON      1
#define FILLED  1
#define XOR     2
#define INVERT  2
#define OR      3
#define AND     4


class Buffer{
public:
  Buffer(uint8_t * RAM_area, int16_t pixHor, int16_t pixVer);       //Constructor
  //Control
  int16_t getWidth(void);               //Returns size of buffer
  int16_t getHeight(void);              //Returns size of buffer
  uint8_t* ptr(void);                       //Returns pointer to graphics area
  uint8_t* ptr(uint8_t* new_pointer);          //Changes pointer to another Ram area

  //Buffer manipulation
  uint8_t* overwrite(Buffer other_buffer);
  uint8_t* overwrite(uint8_t* other_buffer);    //Copies another buffer (must be same size!) to this buffer and overwrites it.
  uint8_t* overwrite(const uint8_t* picture);
  uint8_t* overlay(Buffer other_buffer, uint8_t mode);
  uint8_t* overlay(uint8_t* other_buffer, uint8_t mode);   //Puts another on top of this buffer. modes availlable are: OR / AND / XOR
  uint8_t* overlay(const uint8_t* picture, uint8_t mode);
  uint8_t* invert(void);                     //Inverts the colors
  uint8_t* clear(void);                      //Clears the screen
  uint8_t* fill(uint8_t n);                     //fills every location with specified byte.


  void scrollUp(int x, int y, int a, int b);      //Scrolls the content of the given area up one pixel
  void scrollDown(int x, int y, int a, int b);    //Scrolls the content of the given area down one pixel
  void scrollLeft(int x, int y, int a, int b);    //Scrolls the content of the given area left one pixel
  void scrollRight(int x, int y, int a, int b);   //Scrolls the content of the given area right one pixel
  uint8_t* scrollUp(int n);                 //Same as the above, but applies on whole buffer and number of pixels to scroll can be specified
  uint8_t* scrollDown(int n);
  uint8_t* scrollLeft(int n);               // |n| <= 8
  uint8_t* scrollRight(int n);


  void flipV(int x, int y, int a, int b);  //Flips the given area vertically
  void flipH(int x, int y, int a, int b);  //Flips the given area horizontally
  void rotateR(int x, int y, int a);       //Rotates the given square clockwise
  void rotateL(int x, int y, int a);       //Rotates the given square counter clockwise
  uint8_t * flipV(void);       //Same as the above, but applies on whole buffer
  uint8_t * flipH(void);
  uint8_t * rotateR(void);
  uint8_t * rotateL(void);

  //Drawing
  void fillRect(int x, int y, int a, int b, uint8_t mode);  //Draws a filled rectangle at top left x|y a pixels wide and b pixels high
                                         //Modes available: ON=FILLED / OFF=CLEAR / XOR=INVERT
  void rect(int x, int y, int a, int b, uint8_t mode);  //Draws a rectangular frame at top left x|y a pixels wide and b pixels high
                                         //line styles available: ON=FILLED / OFF=CLEAR / XOR=INVERT
  void line(int x1,int y1, int x2, int y2, uint8_t mode);   //Draws a line from x1|y1 to x2|y2
                                         //line styles available: ON=FILLED / OFF=CLEAR / XOR=INVERT
  void lineAcross(int x,int y, int w, uint8_t mode);   //Draws a horizontal, w pixels long line from x|y to the right.
                                         //line styles available: ON=FILLED / OFF=CLEAR / XOR=INVERT
  void lineDown(int x,int y, int h, uint8_t mode);    //Draws a vertical, h pixels long line from x|y to the bottom.
                                         //line styles available: ON=FILLED / OFF=CLEAR / XOR=INVERT
  void circle(int cx, int cy, int radius, uint8_t mode);     //Draws a circle with center cx|cy and given radius
                                         //line styles available: ON=FILLED / OFF=CLEAR / XOR=INVERT
  void fillCircle(int cx, int cy, int radius, uint8_t mode);     //Draws a filled circle with center cx|cy and given radius

  void quad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, uint8_t mode);
  void tri(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t mode);

  //Pixel access
  void setPixel(int x, int y, uint8_t mode);  //Sets the state of the pixel x|y. Modes/Colors available: ON=FILLED / OFF=CLEAR / XOR=INVERT
  bool getPixel(int x, int y);           //Returns whether a pixel is set (1) or not (0).

  //Bitmaps
  void bitmap(int x, int y, uint16_t a, uint16_t b, uint8_t* pic, uint8_t mode);  //Draws a*b bitmap at top left x,y; Modes available OR=FILLED=ON / XOR=INVERT / OFF=CLEAR (deletes background)
  void bitmap(int x, int y, uint16_t a, uint16_t b, const uint8_t* pic, uint8_t mode);
  void bitmap(int x, int y, Buffer pic_buffer, uint8_t mode);  //Same as first, but takes a buffer as a bitmap
  void writeByte(int x, int y, uint8_t data, uint8_t mode); //writes a single line of 8 bits; Modes available OR=FILLED=ON / XOR=INVERT

  uint8_t* getBitmap(int x, int y, uint16_t a, uint16_t b, uint8_t* buff); //Takes a screenshoot of the given area and stores it in another buffer. "Bitmap backwards"
  uint8_t* getBitmap(int x, int y, Buffer buff2); //Same as above, but screenshot is as large as the buffer given.

private:
  uint8_t * array;         //pointer to buffer array
  uint16_t size;	    //#Bytes the buffer holds (Width/8 * Height)
  int16_t width;
  int16_t height;
  uint8_t columns;         //#Columns the buffer has (Width/8)
};





#endif // BUFFER_h
