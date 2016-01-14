/*
 ***************************************************************************
 *
 * Author: Teunis van Beelen
 *
 * Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 Teunis van Beelen
 *
 * Email: teuniz@gmail.com
 *
 ***************************************************************************
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***************************************************************************
 */

/* Last revision: December 19, 2015 */

/* For more info and how to use this libray, visit: http://www.teuniz.net/RS-232/ */


#ifndef rs232_INCLUDED
#define rs232_INCLUDED

#include <stdio.h>
#include <string.h>

#include "ISerial.h"

#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/file.h>

typedef const char * PCSTR;

class RS232 : public ISerial {
public:
    RS232(PCSTR dev, int baud);
    RS232();
    ~RS232();
    bool Open(PCSTR dev, int baud);
    int OpenComport(int, int, const char *);
    virtual int PollComport(unsigned char *, int);
    int SendByte(int, unsigned char);
    virtual int SendBuf(unsigned char *, int);
    void CloseComport(int);
    void Close();
    void cputs(int, const char *);
    int IsDCDEnabled(int);
    int IsCTSEnabled(int);
    int IsDSREnabled(int);
    void enableDTR(int);
    void disableDTR(int);
    void enableRTS(int);
    void disableRTS(int);
    void flushRX(int);
    void flushTX(int);
    void flushRXTX(int);

protected:
    int mPort;
    struct termios mNewPortSettings;
    struct termios mOldPortSettings;

};

#endif


