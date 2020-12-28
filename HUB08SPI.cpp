#include "HUB08SPI.h"
#include "Arduino.h"

HUB08SPI::HUB08SPI()
{
}

void HUB08SPI::begin(uint8_t *displaybuf, uint16_t width, uint16_t height)
{
    this->displaybuf = displaybuf;
    this->width = width;
    this->height = height;
    DDRD |= 0xFC;
    TCCR2B = TCCR2B & 0b11111000 | 0x01;  //32khz pwm on pin 3 & 11
    SPI.begin();
    analogWrite(3,128);
}

void HUB08SPI::drawPoint(uint16_t x, uint16_t y, uint8_t color)
{
    uint8_t *byte = displaybuf + x / 8 + y * width / 8;
    uint8_t  bit = x % 8;

    if (color)
        *byte |= 0x80 >> bit;
    else
        *byte &= ~(0x80 >> bit);
}

void HUB08SPI::drawRect(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint8_t color)
{
    for (uint16_t x = x1; x < x1+w; x++){
        for (uint16_t y = y1; y < y1+h; y++){
            drawPoint(x, y, color);
        }
    }
}
void HUB08SPI::clear()
{
    uint8_t *ptr = displaybuf;
    for (uint16_t i = 0; i < (width * height / 8); i++)
    {
        *ptr = 0x00;
        ptr++;
    }
}

void HUB08SPI::scan()
{
    static uint8_t row = 0;  // from 0 to 15
    uint8_t *head = displaybuf + row * (width/ 8);
    for (uint8_t line = 0; line < (height / 16); line++)  //when more than 16 line display
    {
        uint8_t *ptr = head;
        head += width * 2;              // width * 16 / 8
        for (uint8_t byte = 0; byte < (width / 8); byte++)
        {
            //CHANGE THIS IF DISPLAY INVERT
            //SPI.transfer(~*ptr);
            SPI.transfer(*ptr);
            ptr++;
        }
    }
    
    uint16_t r = -2000;  //timeout. If something is wrong with the connection, we do not want to wait forever. This value is incremented each cycle of the blocking wait - while loop to bail out evantually, when OE never goes HIGH.
    while (!(PIND & (1<<3)) && ++r); // Wait for OE to go HIGH or Timeout (r == 0)
    uint8_t t = (PIND & 0x0F) | (row <<4); //?? Select row by binary? e.g. 1 -> A or 3 -> A,B or 5 -> A,C? yes, exactly. I shift row by 4 and or it to the output to make row appear on the upper half of Port D. the lower half must remain the same.
    PORTD = t;
    PIND = 1<<2;  //toggle latch. It's a lesser known feature of the atmega controllers. Writing to PINx toggles the output in only one processor cycle
    PIND = 1<<2;
    row = (row + 1) & 0x0F; // Everytime this function is called, it scans the next line. Lines only range from 0 to 15. "& 0xF" equals "% 16", so it wraps back to 0 after reaching 15.
  

}
void HUB08SPI::setBrightness(uint8_t brightness){
    analogWrite(3,255-dim_curve[brightness]);
}
