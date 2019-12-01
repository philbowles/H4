/*
 MIT License

Copyright (c) 2019 Phil Bowles <H48266@gmail.com>
   github     https://github.com/philbowles/H4
   blog       https://8266iot.blogspot.com
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/H4-Esp8266-Firmware-Support-2338535503093896/


Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

 Modified 23 November 2006 by David A. Mellis
 Modified 03 August 2015 by Chuck Todd
 Modified 30 November 2019 by Phil Bowles 
  (Lifted from Arduion and heavily modified for STM32, Ubuntu and Raspberry Pi to remove Arduino-specifics
*/

#ifndef Print_h
#define Print_h

#include <inttypes.h>
#include <stdio.h> // for size_t
#include <string.h>
#include <string>

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

// uncomment next line to support printing of 64 bit ints.
#if LLONG_MAX > LONG_MAX
	#define SUPPORT_LONGLONG
#endif

class Print {
  private:
    size_t printNumber(unsigned long, uint8_t);
#ifdef SUPPORT_LONGLONG
    void printLLNumber(uint64_t, uint8_t);
#endif
    size_t printFloat(double, uint8_t);
  public:
    Print(){}
    virtual ~Print(){}

    virtual size_t write(uint8_t) = 0;

    size_t write(const char *str)
    {
      if (str == NULL) {
        return 0;
      }
      return write((const uint8_t *)str, strlen(str));
    }

    virtual size_t write(const uint8_t *buffer, size_t size);

    size_t write(const char *buffer, size_t size)
    {
      return write((const uint8_t *)buffer, size);
    }

    size_t print(std::string s);
    size_t print(const char[]);
    size_t print(char);
    size_t print(unsigned char, int = DEC);
    
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    size_t print(double, int = 2);
    
    size_t println(std::string s);
    size_t println(const char[]);
    size_t println(char);
    size_t println(unsigned char, int = DEC);
    size_t println(int, int = DEC);
    size_t println(unsigned int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    size_t println(double, int = 2);
    size_t println(void);
    
#ifdef SUPPORT_LONGLONG
    void println(int64_t, uint8_t = DEC);
    void print(int64_t, uint8_t = DEC);
    void println(uint64_t, uint8_t = DEC);
    void print(uint64_t, uint8_t = DEC);
#endif
};

#endif
