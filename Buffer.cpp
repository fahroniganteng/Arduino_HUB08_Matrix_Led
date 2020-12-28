/*-------------------------------------------------------------------------------------------------
//    *** Change Log / Version history ***
//
// 0.1 -  A basic Graphics library for buffers,
//        made to use it with any low res (up to 256 x 256 pix) monochrome LCD
//        The LCD must have a width which is divisible by 8 and
//        the data has to be stored byte-wise (1 bit = 1 pixel) starting at the top left.
//        For further information on how to use the functions, read the Documentation.pdf
//        Made by Marvin G. aka MGOS  / Apr 27 2013
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
#include "Arduino.h"
#include "Buffer.h"


//-------------------------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------------------------
Buffer::Buffer(uint8_t * RAM_area, int16_t pixHor, int16_t pixVer)
{
    pixHor = (pixHor >> 3) * 8;
    if (pixHor > 256) pixHor = 256;
    if (pixVer > 256) pixVer = 256;
    width = pixHor;
    height = pixVer;
    columns = width/8;
    size = columns*height;
    array = RAM_area;
}

//-------------------------------------------------------------------------------------------------
// Pointer getter
//-------------------------------------------------------------------------------------------------
uint8_t* Buffer::ptr()
{
    return array;
}
//-------------------------------------------------------------------------------------------------
// Pointer setter
//-------------------------------------------------------------------------------------------------
uint8_t* Buffer::ptr(uint8_t* new_pointer)
{
    array = new_pointer;
    return array;
}
//-------------------------------------------------------------------------------------------------
// Get size
//-------------------------------------------------------------------------------------------------
int16_t  Buffer::getWidth()
{
    return width;
}
int16_t  Buffer::getHeight()
{
    return height;
}
//-------------------------------------------------------------------------------------------------
// clear
//-------------------------------------------------------------------------------------------------
uint8_t * Buffer::clear()
{
    for(uint16_t  i=0; i < size; i++) array[i] = 0;
    return array;
}
//-------------------------------------------------------------------------------------------------
// fill
//-------------------------------------------------------------------------------------------------
uint8_t * Buffer::fill(uint8_t n)
{
    for(uint16_t  i=0; i < size; i++) array[i] = n;
    return array;
}
//-------------------------------------------------------------------------------------------------
// Invert
//-------------------------------------------------------------------------------------------------
uint8_t * Buffer::invert()
{
    for(uint16_t  i=0; i < size; i++) array[i] ^= 0xFF;
    return array;
}
//-------------------------------------------------------------------------------------------------
// Set Pixel
//-------------------------------------------------------------------------------------------------
void Buffer::setPixel(int x, int y, uint8_t mode)
{
    if (y >= height || x >= width || x < 0 || y < 0)
        return;
    if (mode == OFF)
        array[x/8+y*columns] &= ~(128 >> (x & 7));
    else if (mode == ON)
        array[x/8+y*columns] |= (128 >> (x & 7));
    else
        array[x/8+y*columns] ^= (128 >> (x & 7));
}
//-------------------------------------------------------------------------------------------------
// Get Pixel
//-------------------------------------------------------------------------------------------------
bool Buffer::getPixel(int x, int y)
{
    if (y >= height || x >= width || x < 0 || y < 0)
        return true;
    return (array[x/8+y*columns] & (128 >> (x & 7)));
}
//-------------------------------------------------------------------------------------------------
// Scrolling
//-------------------------------------------------------------------------------------------------
void Buffer::scrollUp(int x, int y, int a, int b)
{
    if (x + a > width)
        a = width-x;
    if (y + b > height)
        b = height-y;
    if (x < 0)
    {
        a = a+x;
        x = 0;
    }
    if (y < 0)
    {
        b = b+y;
        y = 0;
    }
    if (y >= height || x >= width || a <= 0 || b <= 1) return;
    uint16_t bottom;
    uint8_t left = x& 7;
    if (left+a <= 8)  //only one byte?
    {
        uint8_t m = 0xFF << (8-a);
        m >>= left;
        bottom = (y+b-1)*columns;
        for (uint16_t l = y*columns+x/8; l < bottom; l+=columns){
            array[l] = (array[l] & (~m)) | (array[l+columns] & m);
        }
    }
    else
    {
        uint8_t right = (x+a)& 7;
        uint16_t start = y*columns+(x+7)/8;  //included
        uint16_t end =   y*columns+(x+a)/8;  //not included
        uint8_t l_m = 0xFF >> left;
        uint8_t r_m = 0xFF << 8-right;
        bottom = (b-1)*columns;
        for (uint16_t l = 0; l < bottom; l+=columns)
        {
            if (left) array[l+start-1] = (array[l+start-1] & (~l_m)) | (array[l+start+columns-1] & l_m);
            for (uint16_t p = start; p < end ; p++) array[l+p] = array[l+p+columns];
            if (right) array[l+end] = (array[l+end] & (~r_m)) | (array[l+end+columns] & r_m);
        }
    }
}
void Buffer::scrollDown(int x, int y, int a, int b)
{
    if (x + a > width)
        a = width-x;
    if (y + b > height)
        b = height-y;
    if (x < 0)
    {
        a = a+x;
        x = 0;
    }
    if (y < 0)
    {
        b = b+y;
        y = 0;
    }
    if (y >= height || x >= width || a <= 0 || b <= 1) return;
    uint8_t left = x& 7;
    //Serial.println(left);
    //Serial.println(a);
    if (left+a <= 8)  //only one byte?
    {
        uint8_t m = 0xFF << (8-a);
        
        m >>= left;
        uint16_t top = y*columns+x/8;


        //SOMETHING WRONG WITH FORMULA
        //i have modified this , But some location still error.... ^___^
        //maybe someone can help.....
        uint16_t bottom = (y+b-1)*columns + x/8 - 8;
        for (uint16_t l = bottom; l > top; l-=columns)
            array[l] = (array[l] & (~m)) | (array[l-columns] & m);

        
      /* RONN MOD. INFO
       * ******************************************************
       * ORIGINAL LIBRARY CODE
        Serial.println();
        for (uint16_t l = (y+b-1)*columns; l > top; l-=columns){
            array[l] = (array[l] & (~m)) | (array[l-columns] & m);
        }
       */
       
       /*
       * TRY THIS CODE WITH ORIGINAL LIBRARY  
       * *******************************************************
        buff.fillRect(0,5,64,6,ON);
        for (byte i = 0; i < 64; i++) {
          buff.fillRect(0,5,64,6,ON);
          for (byte aa = 0; aa < 8; aa++) {
            buff.scrollDown(i,4,3,8);
            delay(10);
          }
          delay(500);
        }
       */

    }
    else
    {
        uint8_t right = (x+a)& 7;
        uint16_t start = y*columns+(x+7)/8;  //included
        uint16_t end =   y*columns+(x+a)/8;  //not included
        uint8_t l_m = 0xFF >> left;
        uint8_t r_m = 0xFF << 8-right;
        for (uint16_t l = (b-1)*columns; l > 0; l-=columns)
        {
            if (left) array[l+start-1] = (array[l+start-1] & (~l_m)) | (array[l+start-columns-1] & l_m);
            for (uint16_t p = start; p < end ; p++) array[l+p] = array[l+p-columns];
            if (right) array[l+end] = (array[l+end] & (~r_m)) | (array[l+end-columns] & r_m);
        }
    }
}
void Buffer::scrollRight(int x, int y, int a, int b)
{
    if (x + a > width)
        a = width-x;
    if (y + b > height)
        b = height-y;
    if (x < 0)
    {
        a = a+x;
        x = 0;
    }
    if (y < 0)
    {
        b = b+y;
        y = 0;
    }
    if (y >= height || x >= width || a <= 1 || b <= 0) return;
    uint8_t left = x& 7;
    uint16_t bottom = (y+b)*columns;
    if (left+a <= 8)  //only one byte?
    {
        uint8_t m = 0xFF << (8-a);
        m >>= left;
        for (uint16_t l = x/8+y*columns; l < bottom; l+=columns)
            array[l] = (array[l] & ~m) | (((array[l] & m) >> 1) & m);

    }
    else
    {
        uint16_t start =   x/8;  //included
        uint16_t end = (x+a)/8;  //included
        uint8_t l_m = 0xFF >> left;
        uint8_t r_m = 0xFF << 8-((x+a)& 7);
        if (!r_m)
        {
            r_m = 0xFF;
            end--;
        }
        uint8_t c,d;
        for (uint16_t l = y*columns; l < bottom; l+=columns)
        {
            c = array[l+start] << 7;
            array[l+start] = (array[l+start] & ~l_m) | (((array[l+start] & l_m) >> 1) & l_m);
            if (end-start>1)
            {
                for (uint8_t p = start+1; p < end; p++)
                {
                    d = array[p + l] << 7;
                    array[p + l] = c | (array[p+l] >> 1);
                    c = d;
                }
            }
            array[l+end] = (array[l+end] & ~r_m) | (((array[l+end] & r_m) >> 1) & r_m) | c;
        }
    }


}
void Buffer::scrollLeft(int x, int y, int a, int b)
{
    if (x + a > width)
        a = width-x;
    if (y + b > height)
        b = height-y;
    if (x < 0)
    {
        a = a+x;
        x = 0;
    }
    if (y < 0)
    {
        b = b+y;
        y = 0;
    }
    if (y >= height || x >= width || a <= 1 || b <= 0) return;
    uint8_t left = x& 7;
    uint16_t bottom = (y+b)*columns;

    if (left+a <= 8)  //only one uint8_t?
    {
        uint8_t m = 0xFF << (8-a);
        m >>= left;
        for (uint16_t l = x/8+y*columns; l < bottom; l+=columns)
            array[l] = (array[l] & ~m) | (((array[l] & m) << 1) & m);
    }
    else
    {
        uint16_t start =   x/8;  //included
        uint16_t end = (x+a)/8;  //included
        uint8_t l_m = 0xFF >> left;
        uint8_t r_m = 0xFF << 8-((x+a)& 7);
        if (!r_m)
        {
            r_m = 0xFF;
            end--;
        }
        uint8_t c,d;
        for (uint16_t l = y*columns; l < bottom; l+=columns)
        {
            c = array[l+end] >> 7;
            array[l+end] = (array[l+end] & ~r_m) | (((array[l+end] & r_m) << 1) & r_m);
            if (end-start>1)
            {
                for (uint8_t p = end-1; p > start ; p--)
                {
                    d = array[p + l] >> 7;
                    array[p + l] = c | (array[p+l] << 1);
                    c = d;
                }
            }
            array[l+start] = (array[l+start] & ~l_m) | (((array[l+start] & l_m) << 1) & l_m) | c;
        }


    }

}
uint8_t * Buffer::scrollUp(int n)
{
    if (n < 0)
        return  scrollDown(-n);

    if (n >= height || !n) return array;
    memmove(array,array+n*columns,(height-n)*columns);
    return array;

}
uint8_t * Buffer::scrollDown(int n)
{
    if (n < 0)
        return  scrollUp(-n);

    if (n >= height || !n) return array;
    memmove(array+n*columns,array,(height-n)*columns);
    return array;
}
uint8_t * Buffer::scrollRight(int n)
{
    if (n < 0)
        return  scrollLeft(-n);

    if (n > 8 || !n) return array;
    if (n == 8)
    {
        memmove(array+1,array,size-1);
        for (uint16_t l = 0; l < size; l+= columns) array[l] = 0;
    }
    else
    {
        uint8_t c,d;
        for (uint16_t l = 0; l < size; l+= columns)
        {
            c = 0;
            for (uint8_t p = 0; p < columns; p++)
            {
                d = array[p + l] << (8-n);
                array[p + l] = c | (array[p+l] >> n);
                c = d;
            }
        }
    }
    return array;
}
uint8_t * Buffer::scrollLeft(int n)
{
    if (n < 0)
        return  scrollRight(-n);

    if (n > 8 || !n) return array;
    if (n == 8)
    {
        memmove(array,array+1,size-1);
        for (uint16_t l = columns-1; l < size; l+= columns) array[l] = 0;
    }
    else
    {
        uint8_t c,d;
        for (uint16_t l = 0; l < size; l+= columns)
        {
            c = 0;
            for (uint8_t p = columns; p;)
            {
                d = array[--p + l] >> (8-n);
                array[p + l] = c | (array[p+l] << n);
                c = d;
            }
        }
    }
    return array;

}
//-------------------------------------------------------------------------------------------------
// Overlay And / Or / Xor
//-------------------------------------------------------------------------------------------------
uint8_t * Buffer::overlay(uint8_t* other_buffer, uint8_t mode)
{
    if (mode == AND)
    {
        for(uint16_t i=0; i < size; i++) array[i] &= other_buffer[i];
    }
    else if (mode == XOR)
    {
        for(uint16_t i=0; i < size; i++) array[i] ^= other_buffer[i];
    }
    else     //Default OR
    {
        for(uint16_t i=0; i < size; i++) array[i] |= other_buffer[i];
    }
    return array;
}
uint8_t * Buffer::overlay(Buffer other_buffer, uint8_t mode)
{
    return overlay(other_buffer.ptr(),mode);
}
uint8_t * Buffer::overlay(const uint8_t* picture, uint8_t mode)
{
    if (mode == AND)
    {
        for(uint16_t i=0; i < size; i++) array[i] &= picture[i];
    }
    else if (mode == XOR)
    {
        for(uint16_t i=0; i < size; i++) array[i] ^= picture[i];
    }
    else     //Default OR
    {
        for(uint16_t i=0; i < size; i++) array[i] |= picture[i];
    }
    return array;
}
//-------------------------------------------------------------------------------------------------
// Overwrite / Copy
//-------------------------------------------------------------------------------------------------
uint8_t * Buffer::overwrite(Buffer other_buffer)
{
    memcpy(array,other_buffer.ptr(),size);
    return array;
}
uint8_t * Buffer::overwrite(uint8_t* other_buffer)
{
    memcpy(array,other_buffer,size);
    return array;
}
uint8_t * Buffer::overwrite(const uint8_t* picture)
{
    memcpy(array,picture,size);
    return array;
}
//-------------------------------------------------------------------------------------------------
// Area mirroring ; Unoptimized ; Does NOT Clip!
//-------------------------------------------------------------------------------------------------
void Buffer::flipV(int x, int y, int a, int b)
{
    if (y+b > height || x+a > width || x < 0 || y < 0 || a<=0 || b<=0) return;
    uint16_t  _lines = b/2;
    uint16_t  d = y+b-1;
    bool c;
    for (uint16_t l = 0; l < _lines; l++)
    {
        for (uint16_t p = x; p < x+a; p++)
        {
            c = getPixel(p,y);
            setPixel(p,y,getPixel(p,d));
            setPixel(p,d,c);
        }
        y++;
        d--;
    }
}
uint8_t * Buffer::flipV()
{
    flipV(0,0,getWidth(),getHeight());
    return array;
}
void Buffer::flipH(int x, int y, int a, int b)
{
    if (y+b > height || x+a > width || x < 0 || y < 0 || a<=0 || b<=0) return;
    uint16_t _lines = a/2;
    uint16_t d = x+a-1;
    bool c;
    for (uint16_t l = 0; l < _lines; l++)
    {
        for (uint16_t p = y; p < y+b; p++)
        {
            c = getPixel(x,p);
            setPixel(x,p,getPixel(d,p));
            setPixel(d,p,c);
        }
        x++;
        d--;
    }

}
uint8_t * Buffer::flipH()
{
    flipH(0,0,getWidth(),getHeight());
    return array;
}
//-------------------------------------------------------------------------------------------------
// Area rotating ; Unoptimized ; Does NOT Clip!
//-------------------------------------------------------------------------------------------------
void Buffer::rotateR(int x, int y, int a)
{
    if (y+a > height || x+a > width || x < 0 || y < 0 || a<=0) return;
    uint16_t r = x+a-1;
    uint16_t b = y+a-1;
    bool c;
    for (uint16_t q = 0; q < a/2; q++)
    {
        for (uint16_t p = 0; p < (a+1)/2; p++)
        {
            c = getPixel(x+p,y+q);
            setPixel(x+p,y+q,getPixel(x+q,b-p));
            setPixel(x+q,b-p,getPixel(r-p,b-q));
            setPixel(r-p,b-q,getPixel(r-q,y+p));
            setPixel(r-q,y+p,c);
        }
    }
}
uint8_t * Buffer::rotateR()
{
    rotateR(0,0,min(getHeight(),getWidth()));
    return array;
}
void Buffer::rotateL(int x, int y, int a)
{
    if (y+a > height || x+a > width || x < 0 || y < 0 || a<=0) return;
    uint16_t r = x+a-1;
    uint16_t b = y+a-1;
    bool c;
    for (uint16_t q = 0; q < a/2; q++)
    {
        for (uint16_t p = 0; p < (a+1)/2; p++)
        {
            c = getPixel(x+p,y+q);
            setPixel(x+p,y+q,getPixel(r-q,y+p));
            setPixel(r-q,y+p,getPixel(r-p,b-q));
            setPixel(r-p,b-q,getPixel(x+q,b-p));
            setPixel(x+q,b-p,c);
        }
    }

}
uint8_t *  Buffer::rotateL()
{
    rotateL(0,0,min(getHeight(),getWidth()));
    return array;
}
//-------------------------------------------------------------------------------------------------
// Bitmap getting ; Does NOT clip! ; May crash when given memory (*buff) is not large enough!
//-------------------------------------------------------------------------------------------------
uint8_t * Buffer::getBitmap(int x, int y, uint16_t a, uint16_t b, uint8_t* buff)
{
    if (y+b > height || x+a > width || x < 0 || y < 0 || !a || !b) return 0; //not entirely on screen
    uint16_t start_line = y;  //included
    uint16_t start_col = x/8;  // included
    uint16_t end_line = y+b;     //excluded
    uint16_t end_col = (x+a+7)/8;  //excluded
    uint8_t offset_left = x& 7;  //bit offset
    uint8_t offset_right = (a+x)& 7;
    uint8_t mask = 0xFF << 8-(a& 7);
    if (!mask) mask = 0xFF;
    uint16_t i = 0;
    if (offset_left)   //Not aligned
    {
        for (uint16_t l = start_line; l < end_line; l++)
        {
            for (uint16_t p = start_col+1; p < end_col; p++)
                buff[i++] = (array[l*columns+p-1] << offset_left) | (array[l*columns+p] >> 8-offset_left);
            if (offset_left < offset_right)
                buff[i++] = (array[l*columns+end_col-1] << offset_left);
            buff[i-1] &= mask;
        }
    }
    else     //Aligned
    {
        for (uint16_t l = start_line; l < end_line; l++)
        {
            for (uint16_t p = start_col; p < end_col; p++)
                buff[i++] = array[l*columns+p];
            buff[i-1] &= mask;
        }
    }
    return buff;
}
uint8_t * Buffer::getBitmap(int x, int y, Buffer buff2)
{
    uint8_t * buff = buff2.ptr();
    uint16_t a = buff2.getWidth();
    uint16_t b = buff2.getHeight();
    if (y+b > height || x+a > width || x < 0 || y < 0 || !a || !b) return 0; //not entirely on screen
    uint16_t start_line = y;  //included
    uint16_t start_col = x/8;  // included
    uint16_t end_line = y+b;     //excluded
    uint16_t end_col = (x+a)/8;  //excluded
    uint8_t offset = x& 7;  //bit offset
    uint16_t i = 0;
    if (offset)   //Not aligned
    {
        for (uint16_t l = start_line; l < end_line; l++)
        {
            for (uint16_t p = start_col; p < end_col; p++)
                buff[i++] = (array[l*columns+p] << offset) | (array[l*columns+p+1] >> 8-offset);
        }
    }
    else     //Aligned
    {
        for (uint16_t l = start_line; l < end_line; l++)
        {
            for (uint16_t p = start_col; p < end_col; p++)
                buff[i++] = array[l*columns+p];
        }
    }
    return buff;
}
//-------------------------------------------------------------------------------------------------
// Bitmap drawing
//-------------------------------------------------------------------------------------------------
void Buffer::bitmap(int x, int y, Buffer pic_buffer, uint8_t mode)
{
    bitmap(x,y,(uint16_t)pic_buffer.getWidth(),(uint16_t)pic_buffer.getHeight(),pic_buffer.ptr(),mode);
}
void Buffer::bitmap(int x, int y, uint16_t a, uint16_t b, const uint8_t* pic, uint8_t mode)
{
    bitmap(x,y,a,b,(uint8_t*)(pic),mode);
}
void Buffer::bitmap(int x, int y, uint16_t a, uint16_t b, uint8_t* pic, uint8_t mode)
{
    if (x >= width || y >= height || x+a <= 0 || y+b <= 0 || !a || !b) return; //outside screen
    uint16_t w_bmp = (a+7)/8;  //width in bytes
    uint16_t start_line_bmp = 0;  //included
    uint16_t start_col_bmp = 0;  // included
    uint16_t end_line_bmp = b;  //excluded
    uint16_t end_col_bmp = w_bmp;  //excluded
    uint8_t offset = x& 7;  //bit offset
    int dy = y;    //vertical offset to get line on buffer; bmp_line+dy -> buff_y
    int dx = x/8;  //...
    if (x + a > width)
        end_col_bmp = columns-(x/8); //right clipping
    if (y + b > height)
        end_line_bmp = height-y;  //bottom clipping
    if (y < 0)
        start_line_bmp = -dy;   //top clipping

    if (offset)   //not aligned
    {
        if (x < 0)
        {
            offset = 8-(abs(x)& 7);  //calculate new offset
            start_col_bmp = -dx;  //left clipping
            dx--;     //correct rounding issue when x < 0

        }
        uint8_t c,d;  //shifted pixels
        if (mode == OFF)
        {
            for (uint16_t l = start_line_bmp; l < end_line_bmp; l++)
            {
                c = 0;
                int p = start_col_bmp;
                d = pic[l*w_bmp+p] << (8-offset);
                if (p+dx >= 0)
                {
                    array[(l+dy)*columns+p+dx] &= ~(0xFF >> offset);
                    array[(l+dy)*columns+p+dx] |= (pic[l*w_bmp+p] >> offset);
                }
                c = d;
                p++;
                for (; p < end_col_bmp; p++)
                {
                    d = pic[l*w_bmp+p] << (8-offset);
                    if (p+dx >= 0 && p+dx < columns)   //don't mess up
                    {
                        array[(l+dy)*columns+p+dx] = (c | (pic[l*w_bmp+p] >> offset));
                    }
                    c = d;
                }
                if (p+dx < columns)
                {
                    array[(l+dy)*columns+p+dx] &= ~(0xFF << (8-offset));
                    array[(l+dy)*columns+p+dx] |= c;
                }
            }
        }
        else
        {
            for (uint16_t l = start_line_bmp; l < end_line_bmp; l++)
            {
                c = 0;
                int p;
                for (p = start_col_bmp; p < end_col_bmp; p++)
                {
                    d = pic[l*w_bmp+p] << (8-offset);
                    if (p+dx >= 0)   //don't mess up
                    {
                        if (mode == XOR)
                            array[(l+dy)*columns+p+dx] ^= (c | (pic[l*w_bmp+p] >> offset));
                        else
                            array[(l+dy)*columns+p+dx] |= (c | (pic[l*w_bmp+p] >> offset));
                    }
                    c = d;
                }
                if (p+dx < columns)
                {
                    if (mode == XOR)
                        array[(l+dy)*columns+p+dx] ^= c;
                    else
                        array[(l+dy)*columns+p+dx] |= c;
                }
            }
        }
    }
    else     //Aligned bitmap
    {
        if (x < 0)
            start_col_bmp = -dx;  //left clipping
        for (uint16_t l = start_line_bmp; l < end_line_bmp; l++)
        {
            for (uint16_t p = start_col_bmp; p < end_col_bmp; p++)
            {
                if (mode == OFF)
                    array[(l+dy)*columns+p+dx] = pic[l*w_bmp+p];
                else if (mode == XOR)
                    array[(l+dy)*columns+p+dx] ^= pic[l*w_bmp+p];
                else
                    array[(l+dy)*columns+p+dx] |= pic[l*w_bmp+p];
            }
        }
    }
}
void Buffer::writeByte(int x, int y, uint8_t data, uint8_t mode)
{
    if (x >= width ||y >= height || x+8<=0 || y < 0) return; //outside screen

    uint8_t offset = x & 7;  //bit offset
    if (offset) //not aligned
    {
        writeByte(x-offset,y,data>>offset,mode);
        writeByte(x+8-offset,y,data<<(8-offset),mode);
    }
    else     //Aligned byte
    {
        uint8_t col = x / 8;
        if (mode == XOR)
            array[y*columns+col] ^= data;
        else
            array[y*columns+col] |= data;
    }
}
//-------------------------------------------------------------------------------------------------
// Quadrilateral
//-------------------------------------------------------------------------------------------------
void Buffer::quad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, uint8_t mode)
{
    line(x0,y0,x1,y1,mode);
    line(x0,y0,x3,y3,mode);
    line(x1,y1,x2,y2,mode);
    line(x3,y3,x2,y2,mode);
    if (mode == XOR)
    {
        setPixel(x0,y0,XOR);
        setPixel(x1,y1,XOR);
        setPixel(x2,y2,XOR);
        setPixel(x3,y3,XOR);
    }
}
//-------------------------------------------------------------------------------------------------
// Triangle
//-------------------------------------------------------------------------------------------------
void Buffer::tri(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t mode)
{
    line(x0,y0,x1,y1,mode);
    line(x0,y0,x2,y2,mode);
    line(x1,y1,x2,y2,mode);
    if (mode == XOR)
    {
        setPixel(x0,y0,XOR);
        setPixel(x1,y1,XOR);
        setPixel(x2,y2,XOR);
    }
}
//-------------------------------------------------------------------------------------------------
// Circle
//-------------------------------------------------------------------------------------------------
void Buffer::circle(int cx, int cy, int radius, uint8_t mode)
{
    if (radius <= 0) return;
    int x = radius, y = 0, xchange, ychange, radiusError;
    xchange = 1 - radius*2;
    ychange = 1;
    radiusError = 0;
    while (x >= y)
    {
        setPixel(cx+y, cy+x, mode);
        setPixel(cx-y, cy-x, mode);
        setPixel(cx+x, cy-y, mode);
        setPixel(cx-x, cy+y, mode);
        if (y && x && x!=y)    //Fixes double pixel inverts on XOR mode
        {
            setPixel(cx-y, cy+x, mode);
            setPixel(cx+x, cy+y, mode);
            setPixel(cx-x, cy-y, mode);
            setPixel(cx+y, cy-x, mode);
        }
        y++;
        radiusError += ychange;
        ychange += 2;
        if (radiusError + radiusError + xchange > 0 )
        {
            x--;
            radiusError += xchange;
            xchange += 2;
        }
    }
}
void Buffer::fillCircle(int cx, int cy, int radius, uint8_t mode)
{
    if (radius <= 0) return;
    int x = radius, y = 0, xchange, ychange, radiusError;
    xchange = 1 - radius*2;
    ychange = 1;
    radiusError = 0;
    while (x >= y)
    {
        if (y && x != y)   //Fixes double pixel inverts on XOR mode
            lineAcross(cx-x,cy+y,x+x+1,mode);
        if (x != y)
            lineAcross(cx-x,cy-y,x+x+1,mode);
        radiusError += ychange;
        ychange += 2;
        if (radiusError + radiusError + xchange > 0 )
        {
            radiusError += xchange;
            xchange += 2;
            lineAcross(cx-y,cy-x,y+y+1,mode);
            lineAcross(cx-y,cy+x,y+y+1,mode);
            x--;
        }
        y++;
    }
}
//-------------------------------------------------------------------------------------------------
// Vertical line
//-------------------------------------------------------------------------------------------------
void Buffer::lineDown(int x,int y, int h, uint8_t mode)
{
    if (x < 0 || x >= width || y >= height) return;
    if (y < 0)
    {
        h = h+y;
        y = 0;
    }
    if (y + h >= height)
        h = height-y;
    if (h <= 0) return;
    uint8_t k = 128 >> (x& 7);
    h = x/8 + (y+h)*columns;
    if (mode == OFF)
    {
        for (uint16_t p = x/8 + y*columns; p < h ; p+= columns) array[p] &= ~k;
    }
    else if (mode == XOR)
    {
        for (uint16_t p = x/8 + y*columns; p < h ; p+= columns) array[p] ^= k;
    }
    else
    {
        for (uint16_t p = x/8 + y*columns; p < h ; p+= columns) array[p] |= k;
    }
}
//-------------------------------------------------------------------------------------------------
// Horizontal line
//-------------------------------------------------------------------------------------------------
void Buffer::lineAcross(int x,int y, int w, uint8_t mode)
{
    if (y < 0 || y >= height || x >= width) return;
    if (x < 0)
    {
        w = w+x;
        x = 0;
    }
    if (x + w > width)
        w = width-x;
    if (w <= 0) return;
    if ((x& 7)+w <= 8)
    {
        uint8_t m = 0xFF << (8-w);
        m >>= (x& 7);
        if (mode == OFF) array[y*columns+x/8] &= ~m;
        else if (mode == XOR) array[y*columns+x/8] ^= m;
        else array[y*columns+x/8] |= m;

    }
    else
    {
        uint16_t start = y*columns+(x+7)/8;  //included
        uint16_t end =   y*columns+(x+w)/8;  //not included
        if (mode == OFF)
        {
            if (x& 7) array[start-1] &= ~(0xFF >> (x& 7));
            for (uint16_t p = start; p < end ; p++) array[p] = 0;
            if ((x+w)& 7) array[end] &= ~(0xFF << (8-(x+w)& 7));
        }
        else if (mode == XOR)
        {
            if (x& 7) array[start-1] ^= 0xFF >> (x& 7);
            for (uint16_t p = start; p < end ; p++) array[p] ^= 0xFF;
            if ((x+w)& 7) array[end] ^= 0xFF << (8-(x+w)& 7);
        }
        else
        {
            if (x& 7) array[start-1] |= 0xFF >> (x& 7);
            for (uint16_t p = start; p < end ; p++) array[p] = 0xFF;
            if ((x+w)& 7) array[end] |= 0xFF << (8-(x+w)& 7);
        }
    }
}

//-------------------------------------------------------------------------------------------------
// Any line
//-------------------------------------------------------------------------------------------------
void Buffer::line(int x1,int y1, int x2, int y2, uint8_t mode)
{
    int CurrentX, CurrentY, Xinc, Yinc, Dx, Dy, TwoDx, TwoDy, AccumulatedError;
    Dx = x2-x1;
    Dy = y2-y1;
    CurrentX = x1;
    CurrentY = y1;
    Xinc = 1;
    Yinc = 1;
    if(Dx < 0)
    {
        Xinc = -1;
        Dx = -Dx;
    }
    if (Dy < 0)
    {
        Yinc = -1;
        Dy = -Dy;
    }
    TwoDx = Dx + Dx;
    TwoDy = Dy + Dy;
    setPixel(x1,y1, mode);
    if (Dx || Dy)
    {
        if (Dy <= Dx)
        {
            AccumulatedError = 0;
            do
            {
                CurrentX += Xinc;
                AccumulatedError += TwoDy;
                if(AccumulatedError > Dx)
                {
                    CurrentY += Yinc;
                    AccumulatedError -= TwoDx;
                }
                setPixel(CurrentX,CurrentY, mode);
            }
            while (CurrentX != x2);
        }
        else
        {
            AccumulatedError = 0;
            do
            {
                CurrentY += Yinc;
                AccumulatedError += TwoDx;
                if(AccumulatedError > Dy)
                {
                    CurrentX += Xinc;
                    AccumulatedError -= TwoDy;
                }
                setPixel(CurrentX,CurrentY, mode);
            }
            while (CurrentY != y2);
        }
    }
}
//-------------------------------------------------------------------------------------------------
// filled Rectangle
//-------------------------------------------------------------------------------------------------
void Buffer::fillRect(int x, int y, int a, int b, uint8_t mode)
{
    if (x + a > width)
        a = width-x;
    if (y + b > height)
        b = height-y;
    if (x < 0)
    {
        a = a+x;
        x = 0;
    }
    if (y < 0)
    {
        b = b+y;
        y = 0;
    }
    if (y >= height || x >= width || a <= 0 || b <= 0) return;
    uint16_t bottom;
    uint8_t left = (x& 7);
    if (left+a <= 8)   //only one byte?
    {
        uint8_t m = 0xFF << (8-a);
        m >>= left;
        bottom = (y+b)*columns;
        if (mode == OFF)
        {
            for (uint16_t l = y*columns+x/8; l < bottom; l+=columns)
                array[l] &= ~m;
        }
        else if (mode == XOR)
        {
            for (uint16_t l = y*columns+x/8; l < bottom; l+=columns)
                array[l] ^= m;
        }
        else
        {
            for (uint16_t l = y*columns+x/8; l < bottom; l+=columns)
                array[l] |= m;
        }
    }
    else
    {
        uint8_t right = (x+a)& 7;
        uint16_t start = y*columns+(x+7)/8;  //included
        uint16_t end =   y*columns+(x+a)/8;  //not included
        uint8_t l_m = 0xFF >> left;
        uint8_t r_m = 0xFF << 8-right;
        bottom = b*columns;
        if (mode == OFF)
        {
            for (uint16_t l = 0; l < bottom; l+=columns)
            {
                if (left) array[l+start-1] &= ~l_m;
                for (uint16_t p = start; p < end ; p++) array[l+p] = 0;
                if (right) array[l+end] &= ~r_m;
            }
        }
        else if (mode == XOR)
        {
            for (uint16_t l = 0; l < bottom; l+=columns)
            {
                if (left) array[l+start-1] ^= l_m;
                for (uint16_t p = start; p < end ; p++) array[l+p] ^= 0xFF;
                if (right) array[l+end] ^= r_m;
            }
        }
        else
        {
            for (uint16_t l = 0; l < bottom; l+=columns)
            {
                if (left) array[l+start-1] |= l_m;
                for (uint16_t p = start; p < end ; p++) array[l+p] = 0xFF;
                if (right) array[l+end] |= r_m;
            }
        }
    }
}
//-------------------------------------------------------------------------------------------------
// Rectangular Frame
//-------------------------------------------------------------------------------------------------
void Buffer::rect(int x, int y, int a, int b, uint8_t mode)
{
    lineAcross(x,y,a,mode);
    if (b > 1) lineAcross(x,y+b-1,a,mode);
    if (b > 2)
    {
        lineDown(x,y+1,b-2,mode);
        lineDown(x+a-1,y+1,b-2,mode);
    }
}
