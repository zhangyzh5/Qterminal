
#include "protocol_iec101_104.h"
#include <QDateTime>
#include <QTime>
#include <QMessageBox>
#include <QString>

int Send_Sequence_Number=0;
int Receive_Sequence_Number=0;
extern void put_into_buffer(QByteArray data,int length,int Rx_Tx);
//cp56time2a cp56time2a_stamp;
//control_field_struct control_field_u;

//TYPE_ID_FIELD TYPE_ID;

void  IEC101_104::IEC101_104_systemTimeTocp56time2a()
{
    QTime time = QTime::currentTime();
    QDate date =QDate::currentDate();

    //msec=datetime1.time.hour()
    cp56time2a.msec=time.msec();
    cp56time2a.min=time.minute();
    cp56time2a.hour=time.hour();
    cp56time2a.mday=date.day();
    cp56time2a.wday=date.dayOfWeek();
    cp56time2a.month=date.month();
    cp56time2a.year =date.year()%100;
    cp56time2a.su=cfg.su;

}

void IEC101_104::IEC101_M_SP_DP_TB_1_handle(QTcpSocket *p_socket,unsigned char point_type)
{
    //connect(settings,SIGNAL(sendData(QByteArray &)),this,SLOT(receiveData(QByteArray &)));
    QByteArray data;
    Send_Sequence_Number+=2;
    data[0]=0x68;
    data[1]=0x15;
    data[2]=Send_Sequence_Number&0xff;
    data[3]=(Send_Sequence_Number>>8)&0xff;
    data[4]=Receive_Sequence_Number&0xff;
    data[5]=(Receive_Sequence_Number>>8)&0xff;
    data[6]=point_type;
    data[7]=0x01;                                     //info number =1;
    data[8]=SPONT;
    data[9]=cfg.originator;
    data[10]=cfg.common_address_config&0xff;
    data[11]=(cfg.common_address_config>>8)&0xff;
    data[12]=cfg.emu_info_object_address&0xff;
    data[13]=(cfg.emu_info_object_address>>8)&0xff;
    data[14]=(cfg.emu_info_object_address>>16)&0xff;
    if(point_type==M_SP_TB_1)data[15]=cfg.emu_di_status;
    else if(point_type==M_DP_TB_1)data[15]=cfg.emu_di_status+1;

    IEC101_104_systemTimeTocp56time2a();
    data[16]=cp56time2a.msec&0xff;
    data[17]=(cp56time2a.msec>>8)&0xff;
    data[18]=cp56time2a.min&0x3f;
    data[19]=(cp56time2a.hour&0x1f) | cfg.su;
    data[20]=(cp56time2a.mday&0x1f) | ((cp56time2a.wday<<5)&0xe0);
    data[21]=cp56time2a.month&0x0f;
    data[22]=cp56time2a.year&0x7f;

    p_socket->write(data,23);
    put_into_buffer(data,23,SEND);
}

void IEC101_104::IEC101_M_ME_TF_1_handle(QTcpSocket *p_socket,unsigned char point_type)
{
    QByteArray data;
    union{
        unsigned char ai_byte[4];
        float ai_float;
    }ai_value;
    int i;
    Send_Sequence_Number+=2;
    data[0]=0x68;
    //data[1]=0x15;
    data[2]=Send_Sequence_Number&0xff;
    data[3]=(Send_Sequence_Number>>8)&0xff;
    data[4]=Receive_Sequence_Number&0xff;
    data[5]=(Receive_Sequence_Number>>8)&0xff;
    data[6]=point_type;
    data[7]=0x01;                                     //info number =1;
    data[8]=SPONT;
    data[9]=cfg.originator;
    data[10]=cfg.common_address_config&0xff;
    data[11]=(cfg.common_address_config>>8)&0xff;
    data[12]=cfg.emu_info_object_address&0xff;
    data[13]=(cfg.emu_info_object_address>>8)&0xff;
    data[14]=(cfg.emu_info_object_address>>16)&0xff;
    i=15;
    if(point_type==M_ME_TF_1)
    {
        ai_value.ai_float=cfg.emu_ai_value_float;
        data[i++]=ai_value.ai_byte[0];
        data[i++]=ai_value.ai_byte[1];
        data[i++]=ai_value.ai_byte[2];
        data[i++]=ai_value.ai_byte[3];
    }
    else if(point_type==M_ME_TD_1)
    {
        data[i++]=cfg.emu_ai_value_int&0xff;
        data[i++]=(cfg.emu_ai_value_int>>8)&0xff;
    }
    data[i++]=0;                                                                //QDS=0
    IEC101_104_systemTimeTocp56time2a();
    data[i++]=cp56time2a.msec&0xff;
    data[i++]=(cp56time2a.msec>>8)&0xff;
    data[i++]=cp56time2a.min&0x3f;
    data[i++]=(cp56time2a.hour&0x1f) | cfg.su;
    data[i++]=(cp56time2a.mday&0x1f) | ((cp56time2a.wday<<5)&0xe0);
    data[i++]=cp56time2a.month&0x0f;
    data[i++]=cp56time2a.year&0x7f;
    data[1]=i-2;
    p_socket->write(data,i);
    put_into_buffer(data,i,SEND);
}

void IEC101_104::IEC101_C_CS_NA_1_handle(QTcpSocket *p_socket,QByteArray &data)                 //Time Sync
{

    Receive_Sequence_Number=(data[2]+(data[3]<<8))+2; //Receive_Sequence_Number from Master
    Send_Sequence_Number=(data[4]+(data[5]<<8));    //Send_Sequence_Number from RTU
    cfg.common_address_received=data[10]+data[11]*256;
    cfg.originator=data[9];
    data[0]=0x68;
    data[1]=0x14;
    data[2]=data[4];
    data[3]=data[5];
    data[4]=(Receive_Sequence_Number)&0xff;
    data[5]=(Receive_Sequence_Number>>8)&0xff;
    data[6]=0x67;
    data[7]=0x01;
    data[8]=0x07;
    data[9]=cfg.originator;
    data[10]=cfg.common_address_config&0xff;
    data[11]=(cfg.common_address_config>>8)&0xff;
    data[12]=0;
    data[13]=0;
    data[14]=0;
    IEC101_104_systemTimeTocp56time2a();
    data[15]=cp56time2a.msec&0xff;
    data[16]=(cp56time2a.msec>>8)&0xff;
    data[17]=cp56time2a.min&0x3f;
    data[18]=(cp56time2a.hour&0x1f) | cfg.su;
    data[19]=(cp56time2a.mday&0x1f) | ((cp56time2a.wday<<5)&0xe0);
    data[20]=cp56time2a.month&0x0f;
    data[21]=cp56time2a.year&0x7f;

    p_socket->write(data,22);
    put_into_buffer(data,22,SEND);
}
void IEC101_104::IEC101_M_SP_DP_NA_1_handle(QTcpSocket *p_socket,unsigned long info_object_address, unsigned int points_number,unsigned char point_type,unsigned char cause)
{
    int i,j;
    QByteArray buffer;

    //if(iec101_104_tool_settings.interrogate_number_sps==0)return;
    Send_Sequence_Number+=2;
    j=points_number;
    buffer[0]=0x68;
    buffer[1]=j+13;
    buffer[2]=Send_Sequence_Number&0xff;
    buffer[3]=(Send_Sequence_Number>>8)&0xff;
    buffer[4]=(Receive_Sequence_Number)&0xff;
    buffer[5]=(Receive_Sequence_Number>>8)&0xff;
    buffer[6]=point_type;
    buffer[7]=j|0x80;
    buffer[8]=cause;
    buffer[9]=cfg.originator;
    buffer[10]=cfg.common_address_config&0xff;
    buffer[11]=(cfg.common_address_config>>8)&0xff;
    buffer[12]=info_object_address&0xff;
    buffer[13]=(info_object_address>>8)&0xff;
    buffer[14]=(info_object_address>>16)&0xff;
    if(cause==INROGEN)
    {
        if(point_type==M_SP_NA_1)for(i=0;i<j;i++)buffer[15+i]=cfg.interro_di_status[i];
        else if(point_type==M_DP_NA_1)for(i=0;i<j;i++)buffer[15+i]=cfg.interro_di_status[i]+1;
    }
    else
    {
        if(point_type==M_SP_NA_1)buffer[15]=cfg.emu_di_status;
        else if(point_type==M_DP_NA_1)buffer[15]=cfg.emu_di_status+1;
    }
    j+=15;
    //QMessageBox::information(NULL,"Interrogate","message sent");
    p_socket->write(buffer,j);
    put_into_buffer(buffer,j,SEND);
}

void IEC101_104::IEC101_C_IC_NA_1_handle(QTcpSocket *p_socket,QByteArray &data)                 //Integrity Poll
{
    Receive_Sequence_Number=(data[2]+data[3]*256)+2; //Receive_Sequence_Number from Master
    Send_Sequence_Number=(data[4]+data[5]*256);    //Send_Sequence_Number from RTU
    unsigned char di_type;
    di_type=M_SP_NA_1+cfg.interro_di_type*2;
    cfg.originator=data[9];
    data[0]=0x68;
    data[1]=0x0e;
    data[2]=Send_Sequence_Number&0xff;
    data[3]=(Send_Sequence_Number>>8)&0xff;
    data[4]=(Receive_Sequence_Number)&0xff;
    data[5]=(Receive_Sequence_Number>>8)&0xff;
    data[6]=0x64;
    data[7]=0x01;
    data[8]=0x07;       //Cause
    data[9]=cfg.originator;       //Originator
    data[10]=cfg.common_address_config&0xff;
    data[11]=(cfg.common_address_config>>8)&0xff;
    data[12]=0;
    data[13]=0;
    data[14]=0;
    data[15]=INROGEN;                          //0x14: Station Global
    p_socket->write(data,16);
    put_into_buffer(data,16,SEND);
    if(cfg.interrogate_start&0x01)
        IEC101_M_SP_DP_NA_1_handle(p_socket,cfg.di_info_object_address,cfg.interrogate_number_di,di_type,INROGEN);
    Send_Sequence_Number+=2;
    data[0]=0x68;
    data[1]=0x0e;
    data[2]=Send_Sequence_Number&0xff;
    data[3]=(Send_Sequence_Number>>8)&0xff;
    data[4]=(Receive_Sequence_Number)&0xff;
    data[5]=(Receive_Sequence_Number>>8)&0xff;
    data[6]=0x64;
    data[7]=0x01;
    data[8]=0x07;
    data[9]=cfg.originator;
    data[10]=cfg.common_address_config&0xff;
    data[11]=(cfg.common_address_config>>8)&0xff;
    data[12]=0;
    data[13]=0;
    data[14]=0;
    data[15]=INROGEN;                          //0x14: Station Global
    p_socket->write(data,16);
    put_into_buffer(data,16,SEND);
}

void IEC101_104::IEC101_Nothing_handle(QTcpSocket *p_socket,QByteArray &data)
{
    control_field_u.byte=data[2];
    if(control_field_u.bit.FORMAT==U_FORMAT)
    {
        if(control_field_u.bit.STARTDT==0x01)control_field_u.bit.STARTDT=0x02;  //act-->con
        if(control_field_u.bit.STOPDT==0x01)control_field_u.bit.STOPDT=0x02;    //act-->con
        if(control_field_u.bit.TESTFR==0x01)control_field_u.bit.TESTFR=0x02;    //act-->con
        data[0]=0x68;
        data[1]=0x04;
        data[2]=control_field_u.byte;
        data[3]=0x00;
        data[4]=0x00;
        data[5]=0x00;
        p_socket->write(data,6);
        put_into_buffer(data,6,SEND);
    }
}
void IEC101_104::IEC101_104_spont(QTcpSocket *p_socket)
{
    unsigned char point_type;
    point_type=cfg.point_type_id;

    switch(point_type)
    {
    case M_SP_NA_1:
    case M_DP_NA_1:
        IEC101_M_SP_DP_NA_1_handle(p_socket,cfg.emu_info_object_address,1,point_type,SPONT);
        break;
    case M_SP_TB_1:
    case M_DP_TB_1:
        IEC101_M_SP_DP_TB_1_handle(p_socket,point_type);
        break;
    case M_ME_TF_1:
    case M_ME_TD_1:
    case M_ME_TE_1:
        IEC101_M_ME_TF_1_handle(p_socket,point_type);
        break;
     default:
        break;
    }
}

void  IEC101_104::IEC101_104_processPacket(QTcpSocket *p_socket, QByteArray &data)
{
    unsigned char data_1[10];

    for(int i=0;i<10;i++)data_1[i]=data[i];
    if(data_1[1]==0x04)                                    //if Length == 4
    {
        if(data_1[2]&0x04)                                    //STARTDT act=1
        {
            IEC101_Nothing_handle(p_socket,data);
        }
        else if(data_1[2]==0x83||data_1[2]==0x43)                              //TESTFR con=1
        {
            IEC101_Nothing_handle(p_socket,data);
        }

    }
    else if(data_1[1]>0x4)
    {
        switch(data_1[6])
        {
          case 0x67:
            IEC101_C_CS_NA_1_handle(p_socket,data);
            break;
          case 0x64:
            if(cfg.interrogate_start)IEC101_C_IC_NA_1_handle(p_socket,data);
            else IEC101_Nothing_handle(p_socket,data);
            break;
          default:
            break;
        }
    }
    else
    {

    }
}
IEC101_104::~IEC101_104()
{

}

IEC101_104::IEC101_104()
{
    //connect(settings,SIGNAL(sendData(QByteArray &)),this,SLOT(receiveData(QByteArray &)));
}
