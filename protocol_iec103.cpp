
#include "protocol_iec103.h"
#include <QDateTime>
#include <QTime>
#include <QMessageBox>
#include <QString>

extern int Send_Sequence_Number;
extern int Receive_Sequence_Number;
extern void put_into_buffer(QByteArray data,int length,int Rx_Tx);
//cp56time2a cp56time2a_stamp;
//control_field_struct control_field_u;
//IEC103_TOOL_CFG iec103_tool_settings;

//TYPE_ID_FIELD TYPE_ID;

void  IEC103::IEC103_systemTimeTocp56time2a()
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

void IEC103::IEC103_M_SP_DP_TB_1_handle(QSerialPort *serial,unsigned char point_type)
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

    IEC103_systemTimeTocp56time2a();
    data[16]=cp56time2a.msec&0xff;
    data[17]=(cp56time2a.msec>>8)&0xff;
    data[18]=cp56time2a.min&0x3f;
    data[19]=(cp56time2a.hour&0x1f) | cfg.su;
    data[20]=(cp56time2a.mday&0x1f) | ((cp56time2a.wday<<5)&0xe0);
    data[21]=cp56time2a.month&0x0f;
    data[22]=cp56time2a.year&0x7f;

    serial->write(data,23);
    put_into_buffer(data,23,SEND);
}

void IEC103::IEC103_M_ME_TF_1_handle(QSerialPort *serial,unsigned char point_type)
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
    IEC103_systemTimeTocp56time2a();
    data[i++]=cp56time2a.msec&0xff;
    data[i++]=(cp56time2a.msec>>8)&0xff;
    data[i++]=cp56time2a.min&0x3f;
    data[i++]=(cp56time2a.hour&0x1f) | cfg.su;
    data[i++]=(cp56time2a.mday&0x1f) | ((cp56time2a.wday<<5)&0xe0);
    data[i++]=cp56time2a.month&0x0f;
    data[i++]=cp56time2a.year&0x7f;
    data[1]=i-2;
    serial->write(data,i);
    put_into_buffer(data,i,SEND);
}

void IEC103::IEC103_C_CS_NA_1_handle(QSerialPort *serial,QByteArray &data)                 //Time Sync
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
    IEC103_systemTimeTocp56time2a();
    data[15]=cp56time2a.msec&0xff;
    data[16]=(cp56time2a.msec>>8)&0xff;
    data[17]=cp56time2a.min&0x3f;
    data[18]=(cp56time2a.hour&0x1f) | cfg.su;
    data[19]=(cp56time2a.mday&0x1f) | ((cp56time2a.wday<<5)&0xe0);
    data[20]=cp56time2a.month&0x0f;
    data[21]=cp56time2a.year&0x7f;

    serial->write(data,22);
    put_into_buffer(data,22,SEND);
}
void IEC103::IEC103_M_SP_DP_NA_1_handle(QSerialPort *serial,unsigned long info_object_address, unsigned int points_number,unsigned char point_type,unsigned char cause)
{
    int i,j;
    QByteArray buffer;

    //if(iec103_tool_settings.interrogate_number_sps==0)return;
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
    serial->write(buffer,j);
    put_into_buffer(buffer,j,SEND);
}

void IEC103::IEC103_C_IC_NA_1_handle(QSerialPort *serial,QByteArray &data)                 //Integrity Poll
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
    serial->write(data,16);
    put_into_buffer(data,16,SEND);
    if(cfg.interrogate_start&0x01)
        IEC103_M_SP_DP_NA_1_handle(serial,cfg.di_info_object_address,cfg.interrogate_number_di,di_type,INROGEN);
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
    serial->write(data,16);
    put_into_buffer(data,16,SEND);
}

void IEC103::IEC103_Ack_handle(QSerialPort *serial,QByteArray &data)
{
    control_field_u.bit.FUNC=0x00;
    control_field_u.bit.FCV_DFC=0;
    control_field_u.bit.FCB_ACD=cfg.acd_flag;
    control_field_u.bit.PRM=0;

    data[0]=0x10;
    data[1]=control_field_u.byte;
    data[2]=cfg.link_address;
    data[3]=data[1]+data[2];
    data[4]=0x16;
    serial->write(data,5);
    put_into_buffer(data,5,SEND);
}

void IEC103::IEC103_Req_Link_Status_handle(QSerialPort *serial,QByteArray &data)
{
    control_field_u.bit.FUNC=0x0b;
    control_field_u.bit.FCV_DFC=0;
    control_field_u.bit.FCB_ACD=cfg.acd_flag;
    control_field_u.bit.PRM=0;

    data[0]=0x10;
    data[1]=control_field_u.byte;
    data[2]=cfg.link_address;
    data[3]=data[1]+data[2];
    data[4]=0x16;
    serial->write(data,5);
    put_into_buffer(data,5,SEND);
}

void IEC103::IEC103_Req_C1_handle(QSerialPort *serial,QByteArray &data1)
{
    QByteArray data;
    unsigned char cs;
    unsigned char COT, INF;
    control_field_u.bit.FUNC=0x08;
    control_field_u.bit.FCV_DFC=0;
    control_field_u.bit.FCB_ACD=0;
    control_field_u.bit.PRM=0;

    if(cfg.lru_normal_flag==true)
    {

    }
    else
    {
        if(cfg.lru_inited_flag==false)
        {
           control_field_u.bit.FCB_ACD=0x01;
           COT=0x04;
           INF=0x03;
           cfg.lru_inited_flag=true;
        }
        else
        {
            COT=0x05;
            INF=0x04;
            cfg.lru_normal_flag=true;
        }

    data[0]=0x68;
    data[1]=0x15;
    data[2]=0x15;
    data[3]=0x68;
    data[4]=control_field_u.byte;
    data[5]=cfg.link_address;
    data[6]=M_IRF_NA_3;
    data[7]=0x81;
    data[8]=COT;
    data[9]=cfg.link_address;
    data[10]=136; // or 131
    data[11]=INF;
    data[12]=0x02;
    data[13]='G';
    data[14]='E';
    data[15]=' ';
    data[16]='D';
    data[17]='I';
    data[18]='G';
    data[19]='T';
    data[20]='A';
    data[21]='L';
    data[22]='1';
    data[23]='.';
    data[24]='0';
    cs=0x00;
    for(int i=0;i<21;i++)cs+=data[4+i];
    data[25]=cs&0xff;
    data[26]=0x16;
    serial->write(data,27);
    put_into_buffer(data,28,SEND);
    }
}
void IEC103::IEC103_Req_C2_handle(QSerialPort *serial,QByteArray &data1)
{
    QByteArray data;
    unsigned char cs;
    unsigned char COT, INF;
    control_field_u.bit.FUNC=0x08;
    control_field_u.bit.FCV_DFC=0;
    control_field_u.bit.FCB_ACD=0;
    control_field_u.bit.PRM=0;

    COT=0x02;   // Cycle
    INF=0x90;   // Mearand I

    data[0]=0x68;
    data[1]=0x0a;
    data[2]=0x0a;
    data[3]=0x68;
    data[4]=control_field_u.byte;
    data[5]=cfg.link_address;
    data[6]=M_MEI_NA_3;
    data[7]=0x1;
    data[8]=COT;
    data[9]=cfg.link_address;
    data[10]=136; // or 131
    data[11]=INF;
    data[12]=0xf0;
    data[13]=0xff;

    cs=0x00;
    for(int i=0;i<10;i++)cs+=data[4+i];
    data[14]=cs&0xff;
    data[15]=0x16;
    serial->write(data,16);
    put_into_buffer(data,16,SEND);
}

void IEC103::IEC103_spont(QSerialPort *serial)
{
    unsigned char point_type;
    point_type=cfg.point_type_id;

    switch(point_type)
    {
    case M_SP_NA_1:
    case M_DP_NA_1:
        IEC103_M_SP_DP_NA_1_handle(serial,cfg.emu_info_object_address,1,point_type,SPONT);
        break;
    case M_SP_TB_1:
    case M_DP_TB_1:
        IEC103_M_SP_DP_TB_1_handle(serial,point_type);
        break;
    case M_ME_TF_1:
    case M_ME_TD_1:
    case M_ME_TE_1:
        IEC103_M_ME_TF_1_handle(serial,point_type);
        break;
     default:
        break;
    }
}

void  IEC103::IEC103_processPacket(QSerialPort *serial, QByteArray &data)
{
    unsigned char data_1[50];
    unsigned char fun_code;

    for(int i=0;i<data.size();i++)data_1[i]=data[i];
    if(!(data_1[0]==0x10||data_1[0]==0x68))return;

    if(data_1[0]==0x10)                                    //if Length == 4
    {
        fun_code=data_1[1]&0x0f;
        cfg.link_address=data_1[2];
        switch(fun_code)
        {
            case 0:
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:                                     //Reset_FCB
                cfg.lru_inited_flag=false;
                cfg.lru_normal_flag=false;
                IEC103_Ack_handle(serial,data);
                break;
            case 8:
            case 9:                                     //Req_Link_Status
                IEC103_Req_Link_Status_handle(serial,data);
                break;
            case 10:
                IEC103_Req_C1_handle(serial,data);
                break;
            case 11:
                IEC103_Req_C2_handle(serial,data);
                break;
            case 12:
            case 13:
            case 14:
            case 15:
                break;
            default:
                break;
        }
    }
    else if(data_1[0]==0x68)
    {
        IEC103_Ack_handle(serial,data);
    }
    else
    {}
}
IEC103::~IEC103()
{

}

IEC103::IEC103()
{
    cfg.lru_inited_flag=false;
    cfg.lru_normal_flag=false;
    control_field_u.bit.RES=0;
}
