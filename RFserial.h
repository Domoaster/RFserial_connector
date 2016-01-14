
#include "Kernel.h"
#include "Float.h"
#include "IConnector.h"
#include "rs232.h"

namespace domoaster {

    typedef const char * PCSTR;

    class RFserial : public IConnector {
    public:

        std::string Name() {
            return "RF over serial";
        }

        std::string Class() {
            return "RFserial";
        }
        void Init();
        void Start();

        void Receive();
        void Send(bool){};
        void Send(std::vector<uint8_t> &);

    public:
        int baudrate;
        int comport_nbr;
        PCSTR device;
        bool start;

        uint8_t buf[4096];
        uint8_t buf_out[4096];
        uint16_t periodsData_size;
        bool flag;
        int n;
        int buf_out_size;
        vector<uint16_t> periodsData;


        pthread_t RFserialThread;
        static RFserial * RFserialObj;

        RFserial();
        void RFserialLoop();
        static void *RFserialLoop(void* pParam);

    private:
        RS232 mRS;
    };

}; // namespace domoaster

extern "C" int LoadPlugins(domoaster::Kernel &);