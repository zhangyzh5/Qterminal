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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "protocol.h"
#include "protocol_hnz.h"
#include "protocol_iec101_104.h"
#include "protocol_iec103.h"
#include "protocol_modbus.h"
#include "protocol_iecgoose.h"

#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QIntValidator>
#include <QLineEdit>
#include <QStatusBar>

myio_buffer global_io_buffer[MAX_IO_BUFFER];
int current_ptr;

void put_into_buffer(QByteArray data,int length,int Rx_Tx)
{
   QDateTime dt;
   int i;

   global_io_buffer[current_ptr].valid_flag=true;
   global_io_buffer[current_ptr].datetime=dt.currentDateTime();
   global_io_buffer[current_ptr].buffer_length=length;
   global_io_buffer[current_ptr].send_receive=Rx_Tx;
   for(i=0;i<length;i++)global_io_buffer[current_ptr].data[i]=data[i];
   current_ptr++;
}
//! [0]
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
//! [0]
    ui->setupUi(this);
//! [1]
    serial = new QSerialPort(this);
//! [1]
    settings = new SettingsDialog;
    myThread = new MyThread;
    iec101_104 = new IEC101_104 ;
    iec103 = new IEC103;
    //CModbus *modbus = CModbus();
    modbus = new CModbus;
    iecgoose = new IEC_GOOSE;

    QTimer *time_clock_main=new QTimer(this);

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionExit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    initActionsConnections();
    statusBar()->showMessage("Please choose Protocol in Tools");
    connect(time_clock_main,SIGNAL(timeout()),this,SLOT(check_io_buffer()));
    time_clock_main->start(500);
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));
    connect(settings,SIGNAL(sendData(myio_buffer &)),this,SLOT(receiveData(myio_buffer &)));
    connect(settings,SIGNAL(serial_connect()),this,SLOT(openSerialPort()));
    connect(settings,SIGNAL(serial_disconnect()),this,SLOT(closeSerialPort()));

//! [2]
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
//! [2]

    ui->tableWidget->setColumnWidth(0,40);
    ui->tableWidget->setColumnWidth(1,100);
    ui->lineMonitorTableWidget->setColumnWidth(0,480);
    ui->lineMonitorTableWidget->setColumnWidth(1,550);

    ui->lineMonitorTableWidget->setAutoScroll(true);
    for(int i=0;i<12;i++)
    {
        //for(int j=3;j<5;j++)ui->tableWidget->setItem(i,j,new QTableWidgetItem("*"));
        ui->tableWidget->setRowHeight(i,30);
        ui->tableWidget->setItem(i,1,new QTableWidgetItem("Display, Event"));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem("0.00"));
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(""));
        ui->tableWidget->item(i,0)->setCheckState(Qt::Unchecked);

    }
    //ui->lineMonitorTableWidget->setGeometry(328,240,329,27*3);
    ui->lineMonitorTableWidget->setWordWrap(true);
    //ui->lineMonitorTableWidget->setA setAlignment(Qt::AlignTop);
}
//void MyDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
//                          const QModelIndex &index)
//{
//        QString text = index.model()->data(index, Qt::DisplayRole).toString();
//        //绘制文本
//        painter->drawText( option.rect, Qt::TextWordWrap | Qt::AlignHCenter | Qt::AlignVCenter, text );
//}

MainWindow::~MainWindow()
{
    delete settings;
    delete myThread;
    delete iec101_104;
    delete iec103;
    delete dnp3;
    delete modbus;
    delete iecgoose;
    delete ui;
}

//! [4]
void MainWindow::openSerialPort()
{
    serialRxTimeout=new QTimer(this);
    SettingsDialog::Settings p = settings->settings();

    serial->setPortName(p.comPortName);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite)) {
            ui->actionConnect->setEnabled(false);
            ui->actionDisconnect->setEnabled(true);
            ui->actionConfigure->setEnabled(false);
            ui->statusBar->showMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                                       .arg(p.comPortName).arg(p.stringBaudRate).arg(p.stringParity).arg(p.stringDataBits)
                                       .arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());

        ui->statusBar->showMessage(tr("Open error"));
    }
    serial->clear();
    serialRxTimerFlag=false;
    serialFrameInteval=(11*1000)/p.baudRate+20;
    connect(serialRxTimeout,SIGNAL(timeout()),this,SLOT(serialBuf_Process()));
}
//! [4]

//! [5]
void MainWindow::closeSerialPort()
{
    SettingsDialog::Settings p = settings->settings();
    if (serial->isOpen())
        serial->close();
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    ui->statusBar->showMessage(tr("%1 is Disconnected").arg(p.comPortName));
}
void MainWindow::serialBuf_Process()
{
    QByteArray data;
    SettingsDialog::Settings p = settings->settings();

    serialRxTimerFlag=false;
    serialRxTimeout->stop();
    data= serial->readAll();
    put_into_buffer(data,data.size(),RECEIVE);
    switch(p.protocolId)
    {
      case IEC_103:
            iec103->IEC103_processPacket(serial,data);
            break;
      case MODBUS_RTU:
            modbus->MODBUS_processPacket(serial,data,settings);
            break;
      default:
            break;
    }
}
//! [5]


void MainWindow::about()
{
    //char data[]={0x0b,0xda,0x1d,0x45,0x00,0x32,0x00,0x10,0x83}; //HNZ
    //unsigned char data[]={0x0a,0x01,0x04,0xa1,0x00,0x01};  //MODBUS CRC should be 0x AC 63
    //unsigned int crc_data,data_length=6;
    QString Pstr="General Protocol Simulator", str;

    //crc_data=HNZ_calculate_crc(data, data_length);
    //crc_data=modbus_calculate_crc(data, data_length);
    //Pstr=QString::number(crc_data);
    //Pstr.sprintf("CRC1(L)= %02X ,CRC2(H)= %02X",crc_data&0xff,crc_data>>8);
    //Pstr.asprintf("General Protocol Simulator Y=%08lx",y);
    //Pstr.append(Pstr.arg(y,0,16));
    //str=QString("%1").arg(new_data.y,0,16);
    //str=QString("%1").arg(new_data.z);
    //Pstr=QString("%1").arg(crc_data,0,16);
    QMessageBox::about(this, Pstr,
                       tr("Welcome to use the <b>General Protocol Simulator</b>! <br> Not allowed to use without permission from designer! <br> @Copyright on April 29"));
 //QMessageBox::about(this,)
}

void Big_Endian_Byte_Swap()
{

    unsigned long int x=0x84420369;
    union Data_32
            {
               unsigned long int y;
               float z;
            };

    union Data_32 new_data;

    new_data.y=((x/0x1000000))*0x10000+((x/0x10000))%0x100*0x1000000+(x%0x100)*0x100+(x/0x100)%0x100;

}

//! [6]
void MainWindow::writeData(const QByteArray &data)
{
    serial->write(data);
}
//! [6]

//! [7]
void MainWindow::writeData_serial()
{
    QByteArray data;

    if(ui->tableWidget->item(0,0)->checkState()==Qt::Checked)
        QMessageBox::about(this,tr("Checked"),tr("dsdsddsds"));
    else
        QMessageBox::about(this,tr("Unchecked"),tr("dsdsddsds"));
    data[0]=0x01;
    data[1]=0x02;
    data[2]=0x03;
    data[3]=0x04;
    data[4]=0x05;
    data[5]=0x06;
    data[6]=0x07;

    serial->write(data);
}
//! [7]
//! [8]
void MainWindow::readData()
{
    if(!serialRxTimerFlag)
    {
        serialRxTimeout->start(serialFrameInteval);
        serialRxTimerFlag=true;
    }
    else
    {
        serialRxTimeout->stop();
        serialRxTimeout->start(serialFrameInteval);
    }
}
//! [8]

//! [9]
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}
//! [9]


void MainWindow::initActionsConnections()
{
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(openSerialPort()));
    connect(ui->actionRun_Once, SIGNAL(triggered()), this, SLOT(writeData_serial()));
    //connect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    //ui->actionExit->setShortcut(tr("Ctrl+Q"));
    connect(ui->actionConfigure, SIGNAL(triggered()), settings, SLOT(show()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
}
void MainWindow::receiveData(myio_buffer &buffer)
{
    QString Pstr;

    Pstr=buffer.datetime.toString("yyyy-MM-dd hh:mm:ss:zzz ");
    if(buffer.send_receive==SEND) Pstr+="(Tx:) ";
    else Pstr+="(Rx:) ";
    if(buffer.valid_flag==true)
    {
    buffer.valid_flag=false;
    QDataStream out(&buffer.data,QIODevice::ReadWrite);
    while(!out.atEnd())
    {
        qint8 outChar=0;
        out>>outChar;
        Pstr+= QString("%1 ").arg(outChar&0xFF, 2, 16, QLatin1Char('0'));

    }
    ui->lineMonitorTableWidget->setRowCount(buffer_ptr+1);
    ui->lineMonitorTableWidget->setItem(buffer_ptr++,0,new QTableWidgetItem(Pstr));
    if(buffer_ptr>=MAX_IO_BUFFER)buffer_ptr=0;
    ui->lineMonitorTableWidget->scrollToBottom();
    }
    //QMessageBox::information(NULL,"AA","BB");
}

void MainWindow::check_io_buffer()
{
    QString Pstr,Qstr;
    myio_buffer *buffer;

    for(int i=0;i<MAX_IO_BUFFER;i++)
    {
        buffer=(global_io_buffer+i);
        Pstr=buffer->datetime.toString("yyyy-MM-dd hh:mm:ss:zzz ");
        if(buffer->send_receive==SEND) Pstr+="(Tx): ";
        else Pstr+="(Rx): ";
        Qstr=Pstr;
        if(buffer->valid_flag==true)
        {
            buffer->valid_flag=false;
            QDataStream out(&buffer->data,QIODevice::ReadWrite);
            while(!out.atEnd())
            {
                qint8 outChar=0;
                out>>outChar;
                Pstr+= QString("%1 ").arg(outChar&0xFF, 2, 16, QLatin1Char('0'));
            }
            ui->lineMonitorTableWidget->setRowCount(buffer_ptr+1);

            ui->lineMonitorTableWidget->setItem(buffer_ptr,0,new QTableWidgetItem(Pstr));
            ui->lineMonitorTableWidget->resizeRowToContents(buffer_ptr);
            ui->lineMonitorTableWidget->setItem(buffer_ptr,1,new QTableWidgetItem(Qstr));
            ui->lineMonitorTableWidget->resizeRowToContents(buffer_ptr);
            ui->lineMonitorTableWidget->scrollToBottom();
            buffer_ptr++;
            if(buffer_ptr>=MAX_IO_BUFFER)buffer_ptr=0;
        }

    }

}
