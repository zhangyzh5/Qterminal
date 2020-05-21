
#include    "protocol_DNP3.h"
#include    <QMessageBox>
extern void put_into_buffer(QByteArray data,int length,int Rx_Tx);
/************************************************************************
This function updates the contents of *crcAccum using right shifts for
each bit.
************************************************************************/
static unsigned short crctable[256] = {
  0x0000,  0x365e,  0x6cbc,  0x5ae2,  0xd978,  0xef26,  0xb5c4,  0x839a,
  0xff89,  0xc9d7,  0x9335,  0xa56b,  0x26f1,  0x10af,  0x4a4d,  0x7c13,
  0xb26b,  0x8435,  0xded7,  0xe889,  0x6b13,  0x5d4d,  0x07af,  0x31f1,
  0x4de2,  0x7bbc,  0x215e,  0x1700,  0x949a,  0xa2c4,  0xf826,  0xce78,
  0x29af,  0x1ff1,  0x4513,  0x734d,  0xf0d7,  0xc689,  0x9c6b,  0xaa35,
  0xd626,  0xe078,  0xba9a,  0x8cc4,  0x0f5e,  0x3900,  0x63e2,  0x55bc,
  0x9bc4,  0xad9a,  0xf778,  0xc126,  0x42bc,  0x74e2,  0x2e00,  0x185e,
  0x644d,  0x5213,  0x08f1,  0x3eaf,  0xbd35,  0x8b6b,  0xd189,  0xe7d7,
  0x535e,  0x6500,  0x3fe2,  0x09bc,  0x8a26,  0xbc78,  0xe69a,  0xd0c4,
  0xacd7,  0x9a89,  0xc06b,  0xf635,  0x75af,  0x43f1,  0x1913,  0x2f4d,
  0xe135,  0xd76b,  0x8d89,  0xbbd7,  0x384d,  0x0e13,  0x54f1,  0x62af,
  0x1ebc,  0x28e2,  0x7200,  0x445e,  0xc7c4,  0xf19a,  0xab78,  0x9d26,
  0x7af1,  0x4caf,  0x164d,  0x2013,  0xa389,  0x95d7,  0xcf35,  0xf96b,
  0x8578,  0xb326,  0xe9c4,  0xdf9a,  0x5c00,  0x6a5e,  0x30bc,  0x06e2,
  0xc89a,  0xfec4,  0xa426,  0x9278,  0x11e2,  0x27bc,  0x7d5e,  0x4b00,
  0x3713,  0x014d,  0x5baf,  0x6df1,  0xee6b,  0xd835,  0x82d7,  0xb489,
  0xa6bc,  0x90e2,  0xca00,  0xfc5e,  0x7fc4,  0x499a,  0x1378,  0x2526,
  0x5935,  0x6f6b,  0x3589,  0x03d7,  0x804d,  0xb613,  0xecf1,  0xdaaf,
  0x14d7,  0x2289,  0x786b,  0x4e35,  0xcdaf,  0xfbf1,  0xa113,  0x974d,
  0xeb5e,  0xdd00,  0x87e2,  0xb1bc,  0x3226,  0x0478,  0x5e9a,  0x68c4,
  0x8f13,  0xb94d,  0xe3af,  0xd5f1,  0x566b,  0x6035,  0x3ad7,  0x0c89,
  0x709a,  0x46c4,  0x1c26,  0x2a78,  0xa9e2,  0x9fbc,  0xc55e,  0xf300,
  0x3d78,  0x0b26,  0x51c4,  0x679a,  0xe400,  0xd25e,  0x88bc,  0xbee2,
  0xc2f1,  0xf4af,  0xae4d,  0x9813,  0x1b89,  0x2dd7,  0x7735,  0x416b,
  0xf5e2,  0xc3bc,  0x995e,  0xaf00,  0x2c9a,  0x1ac4,  0x4026,  0x7678,
  0x0a6b,  0x3c35,  0x66d7,  0x5089,  0xd313,  0xe54d,  0xbfaf,  0x89f1,
  0x4789,  0x71d7,  0x2b35,  0x1d6b,  0x9ef1,  0xa8af,  0xf24d,  0xc413,
  0xb800,  0x8e5e,  0xd4bc,  0xe2e2,  0x6178,  0x5726,  0x0dc4,  0x3b9a,
  0xdc4d,  0xea13,  0xb0f1,  0x86af,  0x0535,  0x336b,  0x6989,  0x5fd7,
  0x23c4,  0x159a,  0x4f78,  0x7926,  0xfabc,  0xcce2,  0x9600,  0xa05e,
  0x6e26,  0x5878,  0x029a,  0x34c4,  0xb75e,  0x8100,  0xdbe2,  0xedbc,
  0x91af,  0xa7f1,  0xfd13,  0xcb4d,  0x48d7,  0x7e89,  0x246b,  0x1235
};


unsigned short DNP3::DNP_calculate_CRC(unsigned char *p, unsigned int count)
{
    unsigned crc = 0;
    while(count--) crc = (crc >> 8) ^ crctable[(crc ^ *p++) & 0x00ff];
    return ~crc;
}

void DNP3::DNP_calculate_CRC1
(
unsigned char dataOctet,   // Data octet
unsigned short *crcAccum    // Pointer to 16-bit crc accumulator
)
{
unsigned char idx; // Index
unsigned short temp; // Temporary variable
#define REVPOLY 0xA6BC // Reverse polynomial for right shifts
// Perform right shifts and update crc accumulator
for (idx = 0; idx < 8; idx++)
{
temp = (*crcAccum ^ dataOctet) & 1;
*crcAccum >>= 1;
dataOctet >>= 1;
if (temp)
*crcAccum ^= REVPOLY;
}
} /* end computeCRC() */

#define DNP_CRC_INIT 0xFFFF
#define DNP_CRC_POLY 0xA6BC

unsigned short DNP3::DNP_calculate_CRC2(unsigned char *pBuffer, unsigned int uNumBytes)
{
    char  cData;
    unsigned char cuBitCtr;

    unsigned short uCrc = DNP_CRC_INIT;
    while( uNumBytes-- )
    {
        cData = *pBuffer++;


        for ( cuBitCtr = 0xff; cuBitCtr; cuBitCtr >>= 1)
        {

            if ( (uCrc ^ cData) & 1)
            {
                uCrc >>= 1;
                uCrc ^= DNP_CRC_POLY;
            }
            else
                uCrc >>= 1;

            cData >>= 1;
        }
    }

    return ( uCrc ^ DNP_CRC_INIT ) ;
}

void DNP3::DNP3_master_resetLink(QTcpSocket *p_socket)
{
    QByteArray data;
    unsigned char data_1[16];
    unsigned short crc;
    datalink_header.bit.DIR=1;
    datalink_header.bit.PRM=1;
    datalink_header.bit.FCB=0;
    datalink_header.bit.FCV=0;
    datalink_header.bit.CODE=0;

    data[0]=0x05;
    data[1]=0x64;
    data[2]=0x05;
    data[3]=datalink_header.byte;
    data[4]=cfg.remote_address&0xff;
    data[5]=(cfg.remote_address>>8)&0xff;
    data[6]=cfg.local_address&0xff;
    data[7]=(cfg.local_address>>8)&0xff;

    for(int i=0;i<8;i++)data_1[i]=data[i];
    crc=DNP_calculate_CRC(data_1,8);
    //crc=~crc;
    data[8]=crc&0xff;
    data[9]=(crc>>8)&0xff;

    p_socket->write(data,10);
    put_into_buffer(data,10,SEND);
}

void DNP3::DNP3_masterPollClass0_1_2_3(QTcpSocket *p_socket)
{
    QByteArray data;
    unsigned char data_1[16];
    unsigned short crc;
    unsigned short total_length,segment_length;
    datalink_header.bit.DIR=1;
    datalink_header.bit.PRM=1;
    datalink_header.bit.FCB=1;
    datalink_header.bit.FCV=1;
    datalink_header.bit.CODE=DL_SEND_CONFIRM;

    total_length=0x14;
    data[0]=DL_START1;
    data[1]=DL_START2;
    data[2]=total_length;
    data[3]=datalink_header.byte;
    data[4]=cfg.remote_address&0xff;
    data[5]=(cfg.remote_address>>8)&0xff;
    data[6]=cfg.local_address&0xff;
    data[7]=(cfg.local_address>>8)&0xff;
    segment_length=8;
    for(int i=0;i<segment_length;i++)data_1[i]=data[i];
    crc=DNP_calculate_CRC(data_1,segment_length);
    data[8]=crc&0xff;
    data[9]=(crc>>8)&0xff;
    transport_header.bit.FIN=1;
    transport_header.bit.FIR=1;
    transport_header.bit.Seq=0;
    application_header.bit.FIN=1;
    application_header.bit.FIR=1;
    application_header.bit.CON=0;
    application_header.bit.Seq++;
    data[10]=transport_header.byte;
    data[11]=application_header.byte;
    data[12]=0x01;                          //Application Response
    data[13]=0x3c;
    data[14]=0x04;
    data[15]=0x06;
    data[16]=0x3c;
    data[17]=0x03;
    data[18]=0x06;
    data[19]=0x3c;
    data[20]=0x02;
    data[21]=0x06;
    data[22]=0x3c;
    data[23]=0x01;
    data[24]=0x06;
    segment_length=total_length-5;
    for(int i=0;i<segment_length;i++)data_1[i]=data[10+i];
    crc=DNP_calculate_CRC(data_1,segment_length);
    data[25]=crc&0xff;
    data[26]=(crc>>8)&0xff;

    p_socket->write(data,27);
    put_into_buffer(data,27,SEND);
}
void DNP3::DNP3_masterPollTime(QTcpSocket *p_socket)
{
    if(reset_link==true)
    {
        DNP3_master_resetLink(p_socket);
        return;
    }
    if(time_sync==true)
    {
        //DNP3_master_timeSync(p_socket);
        time_sync=false;
        return;
    }
    DNP3_masterPollClass0_1_2_3(p_socket);

}
void DNP3::DNP3_FUNC_0_handle(QTcpSocket *p_socket)
{
    QByteArray data;
    unsigned char data_1[16];
    unsigned short crc;
    datalink_header.bit.DIR=0;
    datalink_header.bit.PRM=0;
    datalink_header.bit.FCB=0;
    datalink_header.bit.FCV=0;
    datalink_header.bit.CODE=0;

    data[0]=0x05;
    data[1]=0x64;
    data[2]=0x05;
    data[3]=datalink_header.byte;
    data[4]=cfg.remote_address&0xff;
    data[5]=(cfg.remote_address>>8)&0xff;
    data[6]=cfg.local_address&0xff;
    data[7]=(cfg.local_address>>8)&0xff;

    for(int i=0;i<8;i++)data_1[i]=data[i];
    crc=DNP_calculate_CRC(data_1,8);
    //crc=~crc;
    data[8]=crc&0xff;
    data[9]=(crc>>8)&0xff;

    p_socket->write(data,10);
    put_into_buffer(data,10,SEND);
}
void DNP3::DNP3_FUNC_4_No_Data_handle(QTcpSocket *p_socket)
{
    QByteArray data;
    unsigned char data_1[16];
    unsigned short crc;
    unsigned short total_length,segment_length;
    datalink_header.bit.DIR=0;
    datalink_header.bit.PRM=1;
    datalink_header.bit.FCB=0;
    datalink_header.bit.FCV=0;
    datalink_header.bit.CODE=4;

    total_length=0x0a;
    data[0]=0x05;
    data[1]=0x64;
    data[2]=total_length;
    data[3]=datalink_header.byte;
    data[4]=cfg.remote_address&0xff;
    data[5]=(cfg.remote_address>>8)&0xff;
    data[6]=cfg.local_address&0xff;
    data[7]=(cfg.local_address>>8)&0xff;
    segment_length=8;
    for(int i=0;i<segment_length;i++)data_1[i]=data[i];
    crc=DNP_calculate_CRC(data_1,segment_length);
    data[8]=crc&0xff;
    data[9]=(crc>>8)&0xff;
    transport_header.bit.FIN=1;
    transport_header.bit.FIR=1;
    transport_header.bit.Seq=0;
    application_header.bit.FIN=1;
    application_header.bit.FIR=1;
    application_header.bit.CON=0;
    application_header.bit.Seq++;
    data[10]=transport_header.byte;
    data[11]=application_header.byte;
    data[12]=0x81;                          //Application Response
    data[13]=Need_Time;                     //if Need time
    data[14]=0x00;
    segment_length=total_length-5;
    for(int i=0;i<segment_length;i++)data_1[i]=data[10+i];
    crc=DNP_calculate_CRC(data_1,segment_length);
    data[15]=crc&0xff;
    data[16]=(crc>>8)&0xff;

    p_socket->write(data,17);
    put_into_buffer(data,17,SEND);
}

void DNP3::DNP3_APPL_FUNC_23_handle(QTcpSocket *p_socket)
{
    QByteArray data;
    unsigned char data_1[16];
    unsigned short crc;
    unsigned short total_length,segment_length;
    datalink_header.bit.DIR=0;
    datalink_header.bit.PRM=1;
    datalink_header.bit.FCB=0;
    datalink_header.bit.FCV=0;
    datalink_header.bit.CODE=4;

    total_length=0x10;
    data[0]=0x05;
    data[1]=0x64;
    data[2]=total_length;
    data[3]=datalink_header.byte;
    data[4]=cfg.remote_address&0xff;
    data[5]=(cfg.remote_address>>8)&0xff;
    data[6]=cfg.local_address&0xff;
    data[7]=(cfg.local_address>>8)&0xff;
    segment_length=8;
    for(int i=0;i<segment_length;i++)data_1[i]=data[i];
    crc=DNP_calculate_CRC(data_1,segment_length);
    data[8]=crc&0xff;
    data[9]=(crc>>8)&0xff;
    transport_header.bit.FIN=1;
    transport_header.bit.FIR=1;
    transport_header.bit.Seq++;
    application_header.bit.FIN=1;
    application_header.bit.FIR=1;
    application_header.bit.CON=0;
    application_header.bit.Seq++;
    data[10]=transport_header.byte;
    data[11]=application_header.byte;
    data[12]=0x81;                          //Application Response
    data[13]=0x10;
    data[14]=0x00;
    data[15]=0x34;                          //Time delay: Obj-Var: 0x34 0x02
    data[16]=0x02;
    data[17]=0x07;
    data[18]=0x01;
    data[19]=0x00;
    data[20]=0x00;
    segment_length=total_length-5;
    for(int i=0;i<segment_length;i++)data_1[i]=data[10+i];
    crc=DNP_calculate_CRC(data_1,segment_length);
    data[21]=crc&0xff;
    data[22]=(crc>>8)&0xff;

    p_socket->write(data,23);
    put_into_buffer(data,23,SEND);
}

void  DNP3::DNP3_app_layer_handle(QTcpSocket *p_socket, QByteArray &data)
{
    unsigned int app_func_code;
    unsigned short obj_var;
    app_func_code=data[APP_FUNC];
    obj_var=(data[13]<<8)+data[14];

    switch(app_func_code)
    {
      case 0:
      case 1:
        if(obj_var==0x3c01||obj_var==0x3c02||obj_var==0x3c03||obj_var==0x3c04)//class 1,2,3,0
        {
            DNP3_FUNC_4_No_Data_handle(p_socket);
            break;
        }
        if(obj_var==0x0100||obj_var==0x0a00||obj_var==0x2802||obj_var==0x2800||obj_var==0x2801)//class 1,2,3,0
        {
            DNP3_FUNC_4_No_Data_handle(p_socket);
            break;
        }
        else
        break;
      case 2:
        if(obj_var==0x5001)                         //Object 80 Var 1:Write IIN.1 IIN.2
        {
            DNP3_FUNC_4_No_Data_handle(p_socket);
            break;
        }
        else if(obj_var==0x3201)                    // Object 50 Var 1: Write Data and Time
        {
            Need_Time=0x00;
            DNP3_FUNC_4_No_Data_handle(p_socket);
            break;
        }
        else
        break;
      case 0x17:
        DNP3_APPL_FUNC_23_handle(p_socket);
        break;
      default:
        break;
    }
}
void  DNP3::DNP3_processPacket_slave(QTcpSocket *p_socket, QByteArray &data)
{
    unsigned int dl_func_code;
    dl_func_code=data[3]&0x0f;
    switch(dl_func_code)
    {
    case DL_RESET_LINK:                                     // 0:Reset Link
        //QMessageBox::information(NULL,"DNP 3.0","CODE 0");
        DNP3_FUNC_0_handle(p_socket);
        break;
    case DL_RESET_USER:                                     // 1:Reset User Process
    case DL_TEST_LINK:                                      // 2:Test Data Link
    case DL_SEND_CONFIRM:                                   // 3:User Data need confirm
        break;
    case DL_SEND_NO_REPLY:                                  // 4:User Data don't need confirm
        DNP3_app_layer_handle(p_socket,data);
        break;
    case DL_REQUEST_STATUS:                                 // 9:Request Link Status
        break;
    default:
        break;
    }
}

void  DNP3::DNP3_processPacket_master(QTcpSocket *p_socket, QByteArray &data)
{
    unsigned int dl_func_code;
    dl_func_code=data[3]&0x0f;
    switch(dl_func_code)
    {
    case DL_RESET_LINK:                                     // 0:Reset Link confirmation
        //QMessageBox::information(NULL,"DNP 3.0","CODE 0");
        reset_link=false;
        break;
    case DL_RESET_USER:                                     // 1:Reset User Process
    case DL_TEST_LINK:                                      // 2:Test Data Link
    case DL_SEND_CONFIRM:                                   // 3:User Data need confirm
        break;
    case DL_SEND_NO_REPLY:                                  // 4:User Data don't need confirm
        //DNP3_app_layer_handle(p_socket,data);
        break;
    case DL_REQUEST_STATUS:                                 // 9:Request Link Status
        break;
    default:
        break;
    }
}

void  DNP3::DNP3_processPacket(QTcpSocket *p_socket, QByteArray &data,int isMaster)
{
    unsigned short crc;
    unsigned char data_1[16];

    unsigned short destination_add,resource_add;
    QString Pstr;
    int length;
    datalink_header.byte=data[3]&0xff;

    length=data[2]+3;
    for(int i=0;i<length+2;i++)data_1[i]=data[i];
    crc=DNP_calculate_CRC(data_1,8);                    //Check Header CRC
    Pstr.sprintf("CRC(Cal)=%04x vs CRC(Rev)= %02x %02x",crc&0xffff,data[8]&0xff,data[9]&0xff);
    if(crc!=(data_1[8]+(data_1[9]<<8)))
    {
        QMessageBox::information(NULL,"DNP 3 CRC Error!",Pstr);
        return;
    }
    destination_add=data_1[4]+(data_1[5]<<8);
    resource_add=data_1[6]+(data_1[7]<<8);

//            else QMessageBox::information(NULL,"DNP 3.0","CRC Passed!");

    //Pstr.sprintf("R/L.Add=%04x/%04x vs D/R= %04x/%04x",cfg.remote_address,cfg.local_address,destination_add,resource_add);
    //QMessageBox::information(NULL,"DNP 3 Addr Error!",Pstr);
    if(cfg.remote_address!=resource_add)return;
    if(cfg.local_address!=destination_add)return;   
    if(isMaster)DNP3_processPacket_master(p_socket,data);
    else        DNP3_processPacket_slave(p_socket,data);
}
DNP3::~DNP3()
{

}

DNP3::DNP3()
{
    application_header.bit.Seq=0;
    transport_header.bit.Seq=0;
    Need_Time=0x10;
    reset_link=true;
    time_sync=true;
    cfg.time_sync=false;
}
