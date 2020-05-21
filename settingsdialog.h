/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QTcpSocket>
#include <pcap.h>

#include "mainwindow.h"
#include "protocol.h"
#include "protocol_iec101_104.h"
#include "protocol_DNP3.h"
#include "protocol_modbus.h"
#include "protocol_iecgoose.h"

QT_USE_NAMESPACE

QT_BEGIN_NAMESPACE

namespace Ui {
class SettingsDialog;
}

class QIntValidator;


QT_END_NAMESPACE

#define SEND 1
#define RECEIVE 0
#define SERIAL  0x01
#define TCPIP   0x02
#define ETHERNET 0x03
#define NONE_COM    0x00
#define MAX_MODBUS_POINT    9999

class MyThread : public QThread
 {
     Q_OBJECT

    public:
        MyThread();
        ~MyThread();
    protected:
        void run();
    signals:
        void sendData_1(myio_buffer &);
 };

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    struct SModbus                              //Modbus Slave(Server)
    {
        unsigned int   rtu_role;                /*0: Master; 1: Slave*/
        unsigned char  my_rtu_id;               /*RTU Address configured*/
        unsigned char  function_code;           /* Function Code */
        unsigned int   number_point;            /* Number of pointsd*/      
        unsigned int   rtu_id_received;         /* rtu_id received */

        unsigned int   number_of_coils;
        unsigned int   number_of_discrete_inputs;
        unsigned int   number_of_input_registers;
        unsigned int   number_of_holding_registers;
        unsigned char  exceptionCode;
        unsigned char  exceptionMode;
        unsigned char  coilData[MAX_MODBUS_POINT];                //Read-Write    DO
        unsigned char  disInputData[MAX_MODBUS_POINT];            //Read          DI
        unsigned int   inRegData[MAX_MODBUS_POINT];               //Read          AI
        unsigned int   holdRegData[MAX_MODBUS_POINT];             //Read-write    AO
    };
    struct CModbus                              //Modbus Master(Client)
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
        unsigned char  exceptionCode;
        unsigned char  exceptionMode;
        unsigned char  coil_data[MAX_MODBUS_POINT];
        unsigned char  discrete_input_data[MAX_MODBUS_POINT];
        unsigned int   input_register_data[MAX_MODBUS_POINT];
        unsigned int   holding_register_data[MAX_MODBUS_POINT];
    };
    struct Settings {
        QString protocolName;
        unsigned short protocol_is_network;
        //int protocolId;
        enum protocol_Id protocolId;
        QString comPortName;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        int masterAddr;
        int slaveAddr;
        bool localEchoEnabled;
        bool isMaster;
        QString IPAddress;
        unsigned int poll_timer;
        unsigned int time_sync_timer;
        struct SModbus smodbus;
        struct CModbus cmodbus;
    };

    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    IEC101_104 iec101_104;
    DNP3 dnp3;
    MyThread myThread;
    IEC_GOOSE iecgoose;
    //CModbus modbus;

    Settings settings() const;
signals:
    void sendData(myio_buffer &);
    void serial_connect();
    void serial_disconnect();
private slots:
    void showPortInfo(int idx);
    void apply();
    void comboCellChanged();
    void checkCustomBaudRatePolicy(int idx);
    void checkCustomDevicePathPolicy(int idx);
    void slotConnectClient();
    void slotReadTcpData();
    void slotDisconnected();
    void masterPollTime();
/*
 *  Added by QT below :
 */


    void on_selectProtocolConfirm_clicked();

    void on_ioboardButton_clicked();

    void on_RtuAddSizeBox_currentIndexChanged(int index);

    void on_rtuAddressEdit_textChanged(const QString &arg1);

    void on_ioboardTable_cellChanged(int row, int column);


    void on_com_connectButton_clicked();

    void on_com_disconnectButton_clicked();

    void on_IEC101_triggerEmulatorButton_clicked();

    void on_IEC101_pointTypeComboBox_currentIndexChanged(int index);

    void on_TCP_AcceptAllHostCheckBox_stateChanged(int arg1);

    void on_IEC101_TimeBasecomboBox_currentIndexChanged(int index);

    void on_IEC101_commonAddressLineEdit_textEdited(const QString &arg1);

    void on_IEC101_diInfoObjectAddresslineEdit_textChanged(const QString &arg1);

    void on_IEC101_aiInfoObjectAddresslineEdit_textChanged(const QString &arg1);

    void on_IEC101_interogateDIcheckBox_clicked();

    void on_IEC101_emuDIStatusComboBox_currentIndexChanged(int index);

    void on_IEC101_InterroDIComboBox_currentIndexChanged(int index);

    void on_IEC101_emuAIpointValuelineEdit_textChanged(const QString &arg1);

    void on_IEC101_interrogateDIlineEdit_textChanged(const QString &arg1);

    void on_IEC101_diTypeComboBox_currentIndexChanged(int index);

    void on_DNP3_RemoteAddressLineEdit_textChanged(const QString &arg1);

    void on_DNP3_LocalAddressLineEdit_textChanged(const QString &arg1);

    void on_slaveMasterBox_currentIndexChanged(int index);

    void on_TCP_ipLineEdit1_textChanged(const QString &arg1);

    void on_DNP3_timeSyncCheckBox_clicked(bool checked);

    void on_DNP3_triggerEmulatorButton_clicked();

    void on_gooseSendButton_clicked();

    void on_protocolChoose_currentIndexChanged(int index);

    void inReg_update();
    void holdReg_update();
    void discrete_update();
    void coil_update();
    void on_serverIDEdit_valueChanged(int arg1);

    void on_sModbusSettingsApplyButton_clicked();
    void leftToRightFormatConvert(unsigned long int oriData, int bytes);
    void on_actualToHexConvertButton_clicked();
    unsigned long ByteSwap(unsigned long input,int Bytes);
    void on_hexToActualConvertButton_clicked();

    void on_analogTypeBox_currentIndexChanged(const QString &arg1);

    void on_diCheck_0_stateChanged(int arg1);

    void on_diCheck_1_stateChanged(int arg1);

    void on_diCheck_2_stateChanged(int arg1);

    void on_diCheck_3_stateChanged(int arg1);

    void on_diCheck_4_stateChanged(int arg1);

    void on_diCheck_5_stateChanged(int arg1);

    void on_diCheck_6_stateChanged(int arg1);

    void on_diCheck_7_stateChanged(int arg1);

    void on_diCheck_8_stateChanged(int arg1);

    void on_diCheck_9_stateChanged(int arg1);

    void on_diCheck_10_stateChanged(int arg1);

    void on_diCheck_11_stateChanged(int arg1);

    void on_diCheck_12_stateChanged(int arg1);

    void on_diCheck_13_stateChanged(int arg1);

    void on_diCheck_14_stateChanged(int arg1);

    void on_diCheck_15_stateChanged(int arg1);

    void on_coilCheck_0_stateChanged(int arg1);

    void on_coilCheck_1_stateChanged(int arg1);

    void on_coilCheck_2_stateChanged(int arg1);

    void on_coilCheck_3_stateChanged(int arg1);

    void on_coilCheck_4_stateChanged(int arg1);

    void on_coilCheck_5_stateChanged(int arg1);

    void on_coilCheck_6_stateChanged(int arg1);

    void on_coilCheck_7_stateChanged(int arg1);

    void on_coilCheck_8_stateChanged(int arg1);

    void on_coilCheck_9_stateChanged(int arg1);

    void on_coilCheck_10_stateChanged(int arg1);

    void on_coilCheck_11_stateChanged(int arg1);

    void on_coilCheck_12_stateChanged(int arg1);

    void on_coilCheck_13_stateChanged(int arg1);

    void on_coilCheck_14_stateChanged(int arg1);

    void on_coilCheck_15_stateChanged(int arg1);

    void on_ExceptionCodecomboBox_currentIndexChanged(int index);

    void on_setExceptionCode_stateChanged(int arg1);

private:
    void fillPortsParameters();
    void fillPortsInfo();
    void updateSettings();
//    void capturePackage(u_char *user, const struct pcap_pkthdr *h, const u_char *packet);
    void IEC101_C_CS_NA_1_handle(QTcpSocket *,QByteArray &);                //Time Sync    

private:

    Ui::SettingsDialog *ui;
    Settings currentSettings;
    QIntValidator *intValidator;    
};

#endif // SETTINGSDIALOG_H
