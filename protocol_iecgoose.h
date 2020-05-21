#ifndef PROTOCOL_IECGOOSE
#define PROTOCOL_IECGOOSE

#include <QtNetwork>
#include <pcap.h>
#define SEND 1
#define RECEIVE 0
class IEC_GOOSE
{
public:
    IEC_GOOSE();

    ~IEC_GOOSE();

    void  GOOSE_send(pcap_t *fp);
    void  GOOSE_masterPollTime(pcap_t *fp);
    void  GOOSE_processPacket(pcap_t *fp);
signals:
    void sendData1(QByteArray &);

/* CP56Time2a timestamp */
struct Cp56time2a {
    unsigned short		msec;
    unsigned char		min	:6;
    unsigned char		res1	:1;
    unsigned char		iv	:1;
    unsigned char		hour	:5;
    unsigned char		res2	:2;
    unsigned char		su	:1;
    unsigned char		mday	:5;
    unsigned char		wday	:3;
    unsigned char		month	:4;
    unsigned char		res3	:4;
    unsigned char		year	:7;
    unsigned char		res4	:1;
}cp56time2a;

struct GOOSE_Header
{
    unsigned char dMac[6];
    unsigned char sMac[6];
    unsigned short TPID;
    unsigned short TCI;
    unsigned short Ethertype;
    unsigned short AppId;
    unsigned short Length;
    unsigned short Res1;
    unsigned short Res2;
}goose_header;
#define MAX_INTERRO_POINT 10
typedef struct
{
    unsigned char  su;
    unsigned int   lru_common_address_size;     /*RTU Address bytes*/

    unsigned int   lru_address;          /*RTU Address*/
    unsigned int   lru_role;                 /*0: Slave; 1: Master*/
    unsigned int   ioBoardNumber;        /* Number of IO Board*/
    unsigned char  trigger_flag;
    unsigned char  point_type_id;
    unsigned int   common_address_config;
    unsigned int   common_address_received;
    unsigned long int emu_info_object_address;
    unsigned long int di_info_object_address;
    unsigned long int do_info_object_address;
    unsigned long int ai_info_object_address;
    int            interrogate_number_di;
    int            interrogate_number_ai;
    unsigned char  interrogate_start;                      //Bit 0:DI; Bit1: DPS; Bit 2:AI
    unsigned char  interro_di_status[MAX_INTERRO_POINT];
    unsigned char  interro_di_type;
    unsigned char  interro_ai_di_status[MAX_INTERRO_POINT];
    unsigned char  emu_di_status;
    int            emu_ai_value_int;
    float          emu_ai_value_float;
    double         ai_value;
    unsigned char  originator;

} IECGOOSE_TOOL_CFG;
IECGOOSE_TOOL_CFG cfg;

};

#endif // PROTOCOL_IECGOOSE

