/************************************************************************************************************************************************
 *   Function: PROTOCOL_MODBUS.H
 *   Author:   Robert Zhang
 *
 *      ((C)) Copyright Robert Zhang, 2021.
 *                          All rights reserved.
 *
 *   Description:
 *
 *   This is the include file for a Data Processing Application
 *   Revision History:
 *
 *  Num Modified by         Date            Reason
 *  --- -------------       ---------       ----------------------
 *  100 Robert Zhang        02-May-2020    Created from Modbus RTU/TCP/ASCII
 ** 101 Robert Zhang        10-May-2020    Finished basic functions: reading Coils, Discrete Inputs, Holding registers and input registers
 **************************************************************************************************************************************************/

#ifndef PROTOCOL_MODBUS
#define PROTOCOL_MODBUS

#include <QtNetwork>
#include <QSerialPort>
#define SEND 1
#define RECEIVE 0
class SettingsDialog;
class CModbus
{
public:
    CModbus();

    ~CModbus();
signals:
    void sendData1(QByteArray &);

void  MODBUS_processPacket(QSerialPort *serial, QByteArray &data,SettingsDialog *settings);
void  MODBUS_spont(QSerialPort *serial);
void  MODBUS_READ_COILS_SLAVE(QSerialPort *serial,unsigned int start_address,unsigned int data_number,SettingsDialog *settings);
void  MODBUS_READ_DISCRETE_INPUT_SLAVE(QSerialPort *serial,unsigned int start_address,unsigned int data_number,SettingsDialog *settings);
void  MODBUS_READ_HOLDING_REGISTER_SLAVE(QSerialPort *serial,unsigned int start_address,unsigned int data_number,SettingsDialog *settings);
void  MODBUS_READ_INPUT_REGISTER_SLAVE(QSerialPort *serial,unsigned int start_address,unsigned int data_number,SettingsDialog *settings);
void  MODBUS_SEND_EXCEPTION_INFO(QSerialPort *serial,unsigned char function_code,SettingsDialog *settings);

void  MODBUS_M_SP_DP_TB_1_handle(QSerialPort *serial,unsigned char point_type);
void  MODBUS_M_ME_TF_1_handle(QSerialPort *serial,unsigned char point_type);

private:
    SettingsDialog *settings;
/* Usefull macros */
//#define CHAR char
//#define LONG long
//#define VOID void
//#define INT unsigned int

#define MODBUS_SWAP(x)            ((x << 8) | (x >> 8))

/* General Defines */
#define MODBUS_SUCCESS    0
#define MODBUS_FAILURE    -1
#define MODBUS_DISABLED   -1

#define MODBUS_CRC_POLY        0xA001 // or 0x8005/A001
#define MODBUS_CRC_INIT        0xFFFF // 0xffff/0x0000

#define MODBUS_STATUS_ON      0x08
#define MODBUS_STATUS_OFF     0x00
#define MODBUS_VALIDITY_ON    0x10
#define MODBUS_VALIDITY_OFF   0x00
#define MODBUS_OPEN           0x10
#define MODBUS_CLOSE          0x08

#define MODBUS_LRU_STARTUP 1      /* Lru startup flag*/
#define MODBUS_LRU_RUNNING 0      /* Lru running flag*/

/*  message defines */
#define MODBUS_ADDR_OFFSET    2  /* offset to address field       */
#define MODBUS_MSG_OFFSET     4  /* offset to the message field   */
#define MODBUS_CTRL_OFFSET    3  /* offset to the control field   */
#define MODBUS_STARTFLG_OFFSET 0 /* offset to the start flag      */
#define MODBUS_ENDFLG_OFFSET  4  /* offset from the end to the end flag   */

#define MODBUS_MIN_RX_LENGTH  8  /* Minimum message length    */
#define MODBUS_MAX_INF_LENGTH 37 /* Maximum size of the information field */
#define MODBUS_MAX_MSG_LENGTH 31 /* Maximum size of a message */
#define MODBUS_MAX_ADDR_BYTES 4  /* Maximum number of bytes in address field*/

#define MODBUS_HEADER_LENGTH  2
#define MODBUS_ADDRESS_LENGTH 2
#define MODBUS_DATA_LENGTH    2
#define MODBUS_CRC_LENGTH     2


/* Modbus FUNCTION Codes */
#define MODBUS_READ_COILS               1
#define MODBUS_READ_DISCTRETE_INPUT     2
#define MODBUS_READ_HOLDING_REGISTER    3
#define MODBUS_READ_INPUT_REGISTER      4
#define MODBUS_WRITE_SINGLE_COIL        5
#define MODBUS_WRITE_SINGLE_REGISTER    6
#define MODBUS_WRITE_MULTIPLE_COILS     15
#define MODBUS_WRITE_MULTIPLE_REGISTERS 16

/* Not supported for now
#define MODBUS_MASK_WRITE_RIGISTER      22
#define MODBUS_READ_WRITE_REGISTERS     23
#define MODBUS_READ_FIFO_QUEUE          24
#define MODBUS_READ_FILE_RECORD         20
#define MODBUS_WRITE_FILE_RECORD        21
#define MODBUS_READ_EXCEPTION_STATUS    7
#define MODBUS_DIAGNOSTIC               8
#define MODBUS_GET_COM_EVENT_COUNTER    11
#define MODBUS_GET_COM_EVENT_LOG        12
#define MODBUS_REPORT_SERVER_ID         17
*/

/* Modbus Exception Codes */
#define MODBUS_ILLEGAL_DATA_FUNCTION    1
#define MODBUS_ILLEGAL_DATA_ADDRESS     2
#define MODBUS_ILLEGAL_DATA_VALUE       3
#define MODBUS_SLAVE_DEVICE_FAIL        4
#define MODBUS_ACKNOWLEDGE              5
#define MODBUS_SLAVE_DEVICE_BUSY        6
#define MODBUS_NEGATIVE_ACKNOWLEDGE     7
#define MODBUS_MEMORY_PARITY_ERROR      8
#define MODBUS_RTU_UNAVAILABLE          10
#define MODBUS_TARGET_DEVICE_FAIL       11


#define MODBUS_MASTER 0
#define MODBUS_SLAVE 1


typedef struct
{
    unsigned int   rtu_role;                /*0: Master; 1: Slave*/
    unsigned char  my_rtu_id;               /*RTU Address configured*/
    unsigned char  function_code;           /* Function Code */
    unsigned int   number_point;            /* Number of pointsd*/
    unsigned int   coil_start_address;           /* Start address */
    unsigned int   discrete_input_start_address;           /* Start address */
    unsigned int   input_register_start_address;           /* Start address */
    unsigned int   holding_register_start_address;           /* Start address */
    unsigned int   rtu_id_received;         /* rtu_id received */

    unsigned int   number_of_coils;
    unsigned int   number_of_discrete_inputs;
    unsigned int   number_of_input_registers;
    unsigned int   number_of_holding_registers;
    unsigned char  exception_code;
    unsigned char  exception_mode;
    unsigned char  coil_data[2];
    unsigned char  discrete_input_data[2];
    unsigned int   input_register_data[16];
    unsigned int   holding_register_data[16];

} MODBUS_TOOL_CFG;
public:
    MODBUS_TOOL_CFG modbus_cfg;


unsigned int  MODBUS_check_crc (QByteArray &,unsigned int );
unsigned int modbus_calculate_crc(QByteArray &, unsigned int );
void modbus_write_data(char *, char *,  char*, int);

#define MODBUS_BOARD_MAX   256

};

#endif // PROTOCOL_MODBUS
