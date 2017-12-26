



#ifndef _MINITRANSFER_H_
#define _MINITRANSFER_H_

#include <stdint.h>

class wire{
public:
    wire();
    ~wire();
    void begin();
    void beginTransmission(uint8_t i2caddr);
    void endTransmission(bool b = true);
    void write(uint8_t c);
    void read();
private:

};


class spi{
public:
    spi();
    ~spi();
    void begin();
private:

};


#endif
