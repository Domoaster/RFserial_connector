#ifndef __I_SERIAL_H__
#define __I_SERIAL_H__
#include <stdio.h>
typedef unsigned char uint8_t ;
typedef uint8_t * puint8_t ;

class ISerial
{
public:
	virtual ~ISerial() {} 
	virtual int PollComport(unsigned char *, int) = 0 ;
	//virtual int SendBuf(int, unsigned char *, int) = 0 ;
	virtual int SendBuf(unsigned char *, int) = 0 ;
} ;

#endif // __I_SERIAL_H__
