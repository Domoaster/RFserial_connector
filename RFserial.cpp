/*
 
 */
#include "RFserial.h"
#include "DataAccess.h"
#include "rs232.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


#include <unistd.h>
#include <string.h>
#include <stdexcept>


using namespace domoaster;

RFserial * RFserial::RFserialObj;
char mode[] = {'8', 'N', '1', 0};

void RFserial::Init() {

    baudrate = PARAM_INT("RFserial.baudrate");
    comport_nbr = PARAM_INT("RFserial.comport_nbr");
    device = PARAM_STR("RFserial.device").c_str();
    if (!mRS.Open(device, baudrate)) {
        fprintf(stderr, "Can not open device: %s\n", strerror(errno));
        return;
    }
    start = true;
    usleep(100000);
}

void RFserial::Start() {
    if (pthread_create(&RFserialThread, NULL, RFserial::RFserialLoop, this) < 0) {
        throw std::runtime_error("Can't start ListenSerial thread");

    }
}

void RFserial::Receive() {
    if (start) {
        start = false;
        return;
    }
    n = mRS.PollComport(buf, 4095);
    if (n > 0) {
        periodsData_size = buf[0] << 8;
        periodsData_size += buf[1];
    }

    if (periodsData_size == (n - 2) && periodsData_size < 4096) {
        flag = true;
    }
    if (n > 0 && periodsData_size != (n - 2)) {
    }

    if (flag && n > 0) {
        // Ajout des périodes 
        for (int j = 0; j < periodsData_size / 2; j++) {
            uint16_t tmp;
            tmp = buf[2 * j + 2] << 8;
            tmp += buf[2 * j + 3];
            periodsData.push_back(tmp);
        }
        DOMOASTER_INFO << "Serial message received: " << periodsData_size << " bytes";
        Dispatch(periodsData);
        periodsData.clear();
        flag = false;
    }

    usleep(50000);
}

void RFserial::Send(std::vector<uint8_t> & bytes_out) {
    buf_out_size = bytes_out.size() + 2;
    // Ajout du nombre de bytes
    buf_out[0] = (buf_out_size & 0xFF00) >> 8;
    buf_out[1] = (buf_out_size & 0xFF);
    for (int i = 0; i < buf_out_size - 2; ++i) {
        buf_out[2 + i] = bytes_out[i];
    }
    DOMOASTER_INFO << "Serial message sent";

    mRS.SendBuf(buf_out, buf_out_size);
}

RFserial::RFserial() {
    RFserialObj = this;
}

void RFserial::RFserialLoop() {
    while (1) {
        RFserialObj->Receive();

    }
    return NULL;
}

void* RFserial::RFserialLoop(void* pParam) {
    if (pParam == NULL) return NULL;

    DOMOASTER_INFO << "Enter in thread listen";
    ((RFserial*) pParam)->RFserialLoop();
    DOMOASTER_INFO << "Exit of thread listen";
    return NULL;
}


