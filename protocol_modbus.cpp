#include <QMessageBox>
#include "protocol_modbus.h"
#include "settingsdialog.h"

extern int Send_Sequence_Number;
extern int Receive_Sequence_Number;
extern void put_into_buffer(QByteArray data,int length,int Rx_Tx);
extern SettingsDialog setting;

unsigned int CModbus::modbus_calculate_crc(QByteArray &buf1, unsigned int len)
{
    /*
    //unsigned char data[]={0x0a,0x01,0x04,0xa1,0x00,0x01};  //MODBUS CRC should be 0x AC 63
    //unsigned int crc_data,data_length=6;
     */
    unsigned char buf[255];
    for(unsigned int i=0;i<len;i++)buf[i]=buf1[i];
    unsigned int crc=MODBUS_CRC_INIT;
    for (unsigned int pos = 0; pos < len; pos++)
    {
        crc ^= (unsigned int)buf[pos];
        for (int i = 8; i != 0; i--)
        {
            if ((crc & 0x0001) != 0)
            {
                crc >>= 1;
                crc ^= MODBUS_CRC_POLY;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    crc = ((crc & 0x00ff) << 8) | ((crc & 0xff00) >> 8);
    return crc;
}

void CModbus::MODBUS_READ_INPUT_REGISTER_SLAVE(QSerialPort *serial,unsigned int start_address,unsigned int data_num,SettingsDialog *p)
{
    QByteArray data;

    unsigned int data_bytes;
    unsigned int crc,data_length;
    data_bytes=data_num*2;
    data_length=data_bytes+5;

    data[0]=p->settings().smodbus.my_rtu_id;                              //Slave ID
    data[1]=MODBUS_READ_INPUT_REGISTER;                 //Function Code
    data[2]=data_bytes;                                 //Followed Bytes
    int j=3;
    for(unsigned int i=0;i<data_num;i++)
    {
        data[j]=(p->settings().smodbus.inRegData[start_address+i]>>8)&0xff;
        data[j+1]=(p->settings().smodbus.inRegData[start_address+i])&0xff;
        j+=2;
    }
    crc=modbus_calculate_crc(data,data_bytes+3);
    data[data_bytes+3]=((crc&0xff00)>>8)&0xff;
    data[data_bytes+4]=crc&0x00ff;

    serial->write(data,data_length);
    put_into_buffer(data,data_length,SEND);
}

void CModbus::MODBUS_READ_HOLDING_REGISTER_SLAVE(QSerialPort *serial,unsigned int start_address,unsigned int data_num,SettingsDialog *p)
{
    QByteArray data;
    unsigned int data_bytes;
    unsigned int crc,data_length;
    data_bytes=data_num*2;
    data_length=data_bytes+5;

    data[0]=p->settings().smodbus.my_rtu_id;                              //Slave ID
    data[1]=MODBUS_READ_HOLDING_REGISTER;                 //Function Code
    data[2]=data_bytes;                                 //Followed Bytes
    int j=3;
    for(unsigned int i=0;i<data_num;i++)
    {
        data[j]=(p->settings().smodbus.holdRegData[start_address+i]>>8)&0xff;
        data[j+1]=(p->settings().smodbus.holdRegData[start_address+i])&0xff;
        j+=2;
    }
    crc=modbus_calculate_crc(data,data_bytes+3);
    data[data_bytes+3]=((crc&0xff00)>>8)&0xff;
    data[data_bytes+4]=crc&0x00ff;

    serial->write(data,data_length);
    put_into_buffer(data,data_length,SEND);
}

void CModbus::MODBUS_READ_DISCRETE_INPUT_SLAVE(QSerialPort *serial,unsigned int start_address,unsigned int data_num,SettingsDialog *p)
{
    QByteArray data;
    unsigned int data_bytes;
    unsigned int crc,data_length;
    data_bytes=data_num/8;
    if(data_num%8)data_bytes+=1;
    data_length=data_bytes+5;

    //coil_update();
    data[0]=p->settings().smodbus.my_rtu_id;                              //Slave ID
    data[1]=MODBUS_READ_DISCTRETE_INPUT;                 //Function Code
    data[2]=data_bytes;                                 //Followed Bytes
    int j=3;
    for(unsigned int i=0;i<data_bytes;i++,j++)
    {
        data[j]=0;
        for(int k=0;k<8;k++)if(p->settings().smodbus.disInputData[start_address+i*8+k]==1)data[j]=data[j]|(0x01<<k);
    }
    crc=modbus_calculate_crc(data,data_bytes+3);
    data[data_bytes+3]=((crc&0xff00)>>8)&0xff;
    data[data_bytes+4]=crc&0x00ff;

    serial->write(data,data_length);
    put_into_buffer(data,data_length,SEND);
}
void CModbus::MODBUS_READ_COILS_SLAVE(QSerialPort *serial,unsigned int start_address,unsigned int data_num,SettingsDialog *p)
{
    QByteArray data;
    unsigned int data_bytes;
    unsigned int crc,data_length;
    data_bytes=data_num/8;
    if(data_num%8)data_bytes+=1;
    data_length=data_bytes+5;

    data[0]=p->settings().smodbus.my_rtu_id;                              //Slave ID
    data[1]=MODBUS_READ_COILS;                 //Function Code
    data[2]=data_bytes;                                 //Followed Bytes
    int j=3;
    for(unsigned int i=0;i<data_bytes;i++,j++)
    {
        data[j]=0;
        for(int k=0;k<8;k++)if(p->settings().smodbus.coilData[start_address+i*8+k]==1)data[j]=data[j]|(0x01<<k);
    }
    crc=modbus_calculate_crc(data,data_bytes+3);
    data[data_bytes+3]=((crc&0xff00)>>8)&0xff;
    data[data_bytes+4]=crc&0x00ff;

    serial->write(data,data_length);
    put_into_buffer(data,data_length,SEND);
}
void CModbus::MODBUS_SEND_EXCEPTION_INFO(QSerialPort *serial,unsigned char function_code,SettingsDialog *p)
{
    QByteArray data;

    unsigned int crc,data_length;
    data_length=5;

    data[0]=p->settings().smodbus.my_rtu_id;        //Slave ID
    data[1]=function_code|0x80;                     //Function Code | exception flag;
    data[2]=p->settings().smodbus.exceptionCode;    //Exception Code

    crc=modbus_calculate_crc(data,3);
    data[3]=((crc&0xff00)>>8)&0xff;
    data[4]=crc&0x00ff;

    serial->write(data,data_length);
    put_into_buffer(data,data_length,SEND);
}
void  CModbus::MODBUS_processPacket(QSerialPort *serial, QByteArray &data,SettingsDialog *p)
{

    //unsigned char data_1[50];
    QByteArray data_1;
    unsigned char fun_code;
    unsigned int  crc_data,crc_cal,start_address,data_num;

    QString Pstr="General Protocol Simulator";
    //QMessageBox::information(NULL,Pstr,Pstr);
            //about(this, Pstr, tr("hhhhh"));
 //QMessageBox::about(this,)

    for(int i=0;i<data.size();i++)data_1[i]=data[i];
    modbus_cfg.rtu_id_received=data_1[0];
   if(modbus_cfg.rtu_id_received!=p->settings().smodbus.my_rtu_id)return;            //Compare RTU ID
   start_address=((data_1[2]&0x00ff)<<8)+(data_1[3]&0x00ff);
   data_num=((data_1[4]&0x00ff)<<8)+(data_1[5]&0x00ff);
   //if(cfg.rtu_role==MODBUS_MASTER)return;                   // SLave Mode
    crc_cal=modbus_calculate_crc((data_1),6);
    crc_data=((data_1[6]&0x00ff)<<8)+(data_1[7]&0x00ff);
    //Pstr=QString("%1 %2 %3").arg(modbus_cfg.rtu_id_received,0,10).arg(p->settings().modbus.my_rtu_id,0,10).arg(p->settings().modbus.rtu_role,0,10);
    //QMessageBox::information(NULL,"CRC==================",Pstr);
    if(crc_cal!=crc_data)
    {
        //QMessageBox::information(NULL,"CRC error, Please Check!",Pstr);
        return;
    }

   if(modbus_cfg.rtu_role!=MODBUS_MASTER)
    {
        fun_code=data_1[1]&0x0f;
        if(p->settings().smodbus.exceptionMode==1)
        {
            MODBUS_SEND_EXCEPTION_INFO(serial,fun_code,p);
            return;
        }
        switch(fun_code)
        {
            case MODBUS_READ_COILS:                 //Function Code: 1
                MODBUS_READ_COILS_SLAVE(serial,start_address,data_num,p);
                break;
            case MODBUS_READ_DISCTRETE_INPUT:       //Function Code: 2
                MODBUS_READ_DISCRETE_INPUT_SLAVE(serial,start_address,data_num,p);
                break;
            case MODBUS_READ_HOLDING_REGISTER:     //Function Code: 3
                MODBUS_READ_HOLDING_REGISTER_SLAVE(serial,start_address,data_num,p);
                break;
            case MODBUS_READ_INPUT_REGISTER:        //Function Code: 4               
                MODBUS_READ_INPUT_REGISTER_SLAVE(serial,start_address,data_num,p);
                break;
            case MODBUS_WRITE_SINGLE_COIL:          // Code: 5
                break;
            case MODBUS_WRITE_SINGLE_REGISTER:      // Code: 6
                break;
            case MODBUS_WRITE_MULTIPLE_COILS:      // Code: 15
                break;
            case MODBUS_WRITE_MULTIPLE_REGISTERS:  // Code: 16

            default:
                break;
        }
    }

    else
    {}
}

CModbus::~CModbus()
{

}

CModbus::CModbus()
{

}
