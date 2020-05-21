
#include    "protocol_iecgoose.h"
#include    <QMessageBox>
#include    <pcap.h>

extern void put_into_buffer(QByteArray data,int length,int Rx_Tx);

void IEC_GOOSE::GOOSE_send(pcap_t *fp)
{
    u_char packet[100];
    QByteArray data;

    /* Supposing to be on ethernet, set mac destination to 1:1:1:1:1:1 */
        packet[0]=0x01;
        packet[1]=0x0c;
        packet[2]=0x0d;
        packet[3]=0x00;
        packet[4]=0x7a;
        packet[5]=0xb1;

        /* set mac source to 2:2:2:2:2:2 */
        packet[6]=0x4e;
        packet[7]=0x32;
        packet[8]=0x01;
        packet[9]=0x6b;
        packet[10]=0x33;
        packet[11]=0x2e;

        /* Fill the rest of the packet */
        for(int i=12;i<100;i++)
        {
            packet[i]=i%256;
        }

        /* Send down the packet */
        if (pcap_sendpacket(fp, packet, 100 /* size */) != 0)
        {

        }
    ;

    put_into_buffer(data,10,SEND);
}


void IEC_GOOSE::GOOSE_masterPollTime(pcap_t *fp)
{

}

void  IEC_GOOSE::GOOSE_processPacket(pcap_t *fp)
{

    //Pstr.sprintf("CRC(Cal)=%04x vs CRC(Rev)= %02x %02x",crc&0xffff,data[8]&0xff,data[9]&0xff);

//            else QMessageBox::information(NULL,"DNP 3.0","CRC Passed!");

    //Pstr.sprintf("R/L.Add=%04x/%04x vs D/R= %04x/%04x",cfg.remote_address,cfg.local_address,destination_add,resource_add);
    //QMessageBox::information(NULL,"DNP 3 Addr Error!",Pstr);
}
IEC_GOOSE::~IEC_GOOSE()
{

}

IEC_GOOSE::IEC_GOOSE()
{

}

