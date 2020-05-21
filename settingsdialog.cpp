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
#include <pcap.h>
#include <QtSerialPort/QSerialPortInfo>
#include <QIntValidator>
#include <QLineEdit>
#include <QMessageBox>
#include <QtNetwork>
#include <QtNetwork/QTcpSocket>
#include <QDateTime>
#include <QComboBox>
#include <QStatusBar>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "protocol_hnz.h"
#include "protocol_iec101_104.h"
#include "protocol_DNP3.h"
#include "protocol_modbus.h"
#include "protocol_iecgoose.h"
#include "mainwindow.h"



QT_USE_NAMESPACE

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");
extern HNZ_TOOL_CFG hnz_tool_settings;
extern myio_buffer global_io_buffer[];
extern int current_ptr;
void capturePackage(u_char *user, const struct pcap_pkthdr *h, const u_char *packet);

QIntValidator *aIntValidator= new QIntValidator;
QDoubleValidator *fIntValidator= new QDoubleValidator;
QMap<int,QPair<QWidget*,QString> > map[4];
QTcpServer *m_pTcpServer;
QTcpSocket *m_pConnectSocket;
pcap_if_t *m_pAlldevs,*m_pCurrentdev;
pcap_t *adhandle;
char errbuf[PCAP_ERRBUF_SIZE];

#define 	PCAP_OPENFLAG_PROMISCUOUS   1

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{

    ui->setupUi(this);

    intValidator = new QIntValidator(0, 4000000, this);
    QDoubleValidator *doubleValidator = new QDoubleValidator(-4000000.0, 4000000.0, 2,this);

    ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);
    //statusBar()->
    connect(ui->applyButton, SIGNAL(clicked()),this, SLOT(apply()));
    connect(ui->serialPortInfoListBox, SIGNAL(currentIndexChanged(int)),this, SLOT(showPortInfo(int)));
    connect(ui->baudRateBox, SIGNAL(currentIndexChanged(int)),this, SLOT(checkCustomBaudRatePolicy(int)));
    connect(ui->serialPortInfoListBox, SIGNAL(currentIndexChanged(int)),this, SLOT(checkCustomDevicePathPolicy(int)));

    //connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(openSerialPort()));

    //fillPortsParameters();
    //fillPortsInfo();

    ui->parametersBox_2->setEnabled(false);
    ui->IEC101_interrogateDIlineEdit->setEnabled(false);
    ui->IEC101_interrogateAIlineEdit->setEnabled(false);
    ui->parametersBox_2->setVisible(false);
    ui->IOBoardLabel->setVisible(false);
    ui->ioboardInput->setVisible(false);
    ui->ioboardButton->setVisible(false);
    ui->ioboardTable->setVisible(false);
    ui->IEC101_emuAIpointValuelineEdit->setVisible(false);
    //aIntValidator->setRange(-65535,0xffff);
    //ui->IEC101_emuAIpointValuelineEdit->setValidator(aIntValidator);
    //fIntValidator->setRange(-65535.0,65535.0);
    ui->IEC101_emuAIpointValuelineEdit->setValidator(doubleValidator);
    aIntValidator->setRange(0,65535);
    ui->TCP_PortLineEdit->setValidator(aIntValidator);
    aIntValidator->setRange(0,65535);
    ui->IEC101_emuInfoObjectAddresslineEdit->setValidator(aIntValidator);
    aIntValidator->setRange(0,65535);
    ui->IEC101_diInfoObjectAddresslineEdit->setValidator(aIntValidator);
    aIntValidator->setRange(0,65535);
    ui->IEC101_aiInfoObjectAddresslineEdit->setValidator(aIntValidator);
    aIntValidator->setRange(0,MAX_INTERRO_POINT);
    ui->IEC101_interrogateDIlineEdit->setValidator(aIntValidator);
    aIntValidator->setRange(0,MAX_INTERRO_POINT);
    ui->IEC101_interrogateAIlineEdit->setValidator(aIntValidator);
    aIntValidator->setRange(0,65535);
    ui->IEC101_commonAddressLineEdit->setValidator(aIntValidator);
    QRegExp rx_ip("(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])");
    QValidator * validator = new QRegExpValidator(rx_ip, this);
    ui->TCP_ipLineEdit1->setValidator(validator);
    aIntValidator->setRange(0,65535);
    ui->DNP3_LocalAddressLineEdit->setValidator(aIntValidator);
    aIntValidator->setRange(0,65535);
    ui->DNP3_RemoteAddressLineEdit->setValidator(aIntValidator);
    ui->DNP3_MasterGroupBox->setEnabled(false);
    QPalette pal = ui->IEC104_digitalOutput_PushButton->palette();
    pal.setColor(QPalette::Button,Qt::red);
    ui->IEC104_digitalOutput_PushButton->setPalette(pal);
    ui->IEC104_digitalOutput_PushButton->setAutoFillBackground(true);
    ui->IEC104_digitalOutput_PushButton->setFlat(true);
    //for(int i=1;i<5;i++)
    {
    //map[1].insert(2,QPair<QWidget*,QString>(ui->tabWidget->widget(2),ui->tabWidget->tabText(2)));
    //ui->tabWidget->removeTab(2);
    }
    ui->selectProtocolConfirm->setEnabled(false);    
    ui->SerialOptionsGroupBox->setEnabled(false);
    ui->NetworkOptionsGroupBox->setEnabled(false);
    ui->netInterfaceLabel->setEnabled(false);
    ui->com_connectButton->setEnabled(false);
    ui->com_disconnectButton->setEnabled(false);
    ui->netInterfaceListBox->setEnabled(false);

    for(int i=0;i<MAX_INTERRO_POINT;i++)iec101_104.cfg.interro_di_status[i]=0;
    currentSettings.smodbus.my_rtu_id=0x01;
    currentSettings.smodbus.rtu_role=MODBUS_MASTER;
    for (int i=0;i<16;i++)currentSettings.smodbus.inRegData[i]=0x00;
    currentSettings.isMaster=false;
    updateSettings();

    ui->tabWidget->setCurrentIndex(0);
    ui->tabWidget->setTabEnabled(0,1);  //General Tab Visable only
    for(int i=1;i<8;i++)
    {
        ui->tabWidget->setTabEnabled(i,0);
    }
    //ui->SettingsDialog->

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
    if(m_pAlldevs!=NULL)pcap_freealldevs(m_pAlldevs);
}

SettingsDialog::Settings SettingsDialog::settings() const
{
    return currentSettings;
}

void SettingsDialog::showPortInfo(int idx)
{
    if (idx == -1)
        return;

    QStringList list = ui->serialPortInfoListBox->itemData(idx).toStringList();
    ui->descriptionLabel->setText(tr("%1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
    //ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
}

void SettingsDialog::apply()
{
    updateSettings();
    hide();
}

void SettingsDialog::checkCustomBaudRatePolicy(int idx)
{
    bool isCustomBaudRate = !ui->baudRateBox->itemData(idx).isValid();
    ui->baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        ui->baudRateBox->clearEditText();
        QLineEdit *edit = ui->baudRateBox->lineEdit();
        edit->setValidator(intValidator);
    }
}

void SettingsDialog::checkCustomDevicePathPolicy(int idx)
{
    bool isCustomPath = !ui->serialPortInfoListBox->itemData(idx).isValid();
    ui->serialPortInfoListBox->setEditable(isCustomPath);
    if (isCustomPath)
        ui->serialPortInfoListBox->clearEditText();
}

void SettingsDialog::fillPortsParameters()
{
    if(currentSettings.protocolId==IEC_103)
    {
       ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
       ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
       ui->baudRateBox->setCurrentIndex(0);
       ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
       ui->dataBitsBox->setCurrentIndex(0);
       ui->dataBitsBox->setEnabled(false);
       ui->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
       ui->parityBox->setCurrentIndex(0);
       ui->parityBox->setEnabled(false);
       ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
       ui->stopBitsBox->setCurrentIndex(0);
       ui->stopBitsBox->setEnabled(false);

    }
    else
    {
    ui->baudRateBox->addItem(QStringLiteral("1200"), QSerialPort::Baud1200);
    ui->baudRateBox->addItem(QStringLiteral("2400"), QSerialPort::Baud2400);
    ui->baudRateBox->addItem(QStringLiteral("4800"), QSerialPort::Baud4800);
    ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->baudRateBox->addItem(QStringLiteral("57600"), QSerialPort::Baud57600);
    ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->baudRateBox->addItem(tr("Custom"));
    ui->baudRateBox->setCurrentIndex(3);

    ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->dataBitsBox->setCurrentIndex(0);

    ui->parityBox->addItem(tr("None"), QSerialPort::NoParity);
    ui->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    ui->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);
    ui->parityBox->setCurrentIndex(1);

    ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopBitsBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);
    }
    ui->flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    ui->flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
    ui->flowControlBox->setCurrentIndex(0);
}

void SettingsDialog::fillPortsInfo()
{
    ui->serialPortInfoListBox->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        ui->serialPortInfoListBox->addItem(list.first(), list);
    }

    ui->serialPortInfoListBox->addItem(tr("None"));
}

void SettingsDialog::updateSettings()
{
    currentSettings.comPortName = ui->serialPortInfoListBox->currentText();

    if (ui->baudRateBox->currentIndex() == 4) {
        currentSettings.baudRate = ui->baudRateBox->currentText().toInt();
    } else {
        currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
                    ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
    }
    currentSettings.stringBaudRate = QString::number(currentSettings.baudRate);

    currentSettings.dataBits = static_cast<QSerialPort::DataBits>(
                ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
    currentSettings.stringDataBits = ui->dataBitsBox->currentText();

    currentSettings.parity = static_cast<QSerialPort::Parity>(
                ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
    currentSettings.stringParity = ui->parityBox->currentText();

    currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
                ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
    currentSettings.stringStopBits = ui->stopBitsBox->currentText();

    currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
                ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
    currentSettings.stringFlowControl = ui->flowControlBox->currentText();

    //currentSettings.localEchoEnabled = ui->localEchoCheckBox->isChecked();
}



void SettingsDialog::on_selectProtocolConfirm_clicked()
{
    QString temp;
    int i,j,err=0;

    i=ui->rtuAddressEdit->text().toInt();
    j=ui->RtuAddSizeBox->currentIndex();
    if(i<=0) err=1;
    else
    {
    switch (j)
    {
    case 0:
        if(i>0x1f) err =1;
        break;
    case 1:
        if(i>0x1fff)err=1;
        break;
    case 2:
        if(i>0xfffff)err=1;
        break;
    default: break;
    }
    }
    if (err==1)
    {
      QMessageBox::about(this,tr("RTU"),tr("RTU address out of range, please check!"));
            return;
    }

    hnz_tool_settings.lru_address_size=ui->RtuAddSizeBox->currentIndex();
    hnz_tool_settings.lru_address=ui->rtuAddressEdit->text().toInt();
    hnz_tool_settings.lru_role=ui->slaveMasterBox->currentIndex();

    ui->selectProtocolConfirm->setEnabled(false);
    i=ui->rtuAddressEdit->text().toInt();
    j=ui->RtuAddSizeBox->currentIndex();
    //QMessageBox::about(this,tr("Prot"),temp.append("int= %1").arg(i));
}

void SettingsDialog::on_ioboardButton_clicked()
{
    int temp;
    QStringList header;
    QString Pstr;
    QComboBox *combo[HNZ_BOARD_MAX];
    header<<"Board No."<<"Module Type"<<"Description";

    temp=ui->ioboardInput->text().toInt();
    if(temp<=0 ||temp>HNZ_BOARD_MAX)
    {
        QMessageBox::about(this,tr("HNZ Protocol"),tr("I/O Board Number should in the range of 1~256"));
        return;
    }
    hnz_tool_settings.ioBoardNumber=temp;
    ui->ioboardTable->setVisible(true);

    ui->ioboardTable->setRowCount(temp);
    ui->ioboardTable->setColumnCount(3);
    ui->ioboardTable->setHorizontalHeaderLabels(header);

    for(int i=0;i<temp;i++)
    {
        Pstr="IO Board";
        combo[i]=new QComboBox;
        ui->ioboardTable->setRowHeight(i,30);
        ui->ioboardTable->setColumnWidth(0,80);
        ui->ioboardTable->setColumnWidth(1,80);
        ui->ioboardTable->setColumnWidth(2,140);
        ui->ioboardTable->setItem(i,0,new QTableWidgetItem(Pstr.append(" %1").arg(i+1)));
        ui->ioboardTable->setCellWidget(i,1,combo[i]);
        combo[i]->insertItem(0,"Single DI");
        combo[i]->insertItem(1,"Double DI");
        combo[i]->insertItem(2,"Counter");
        combo[i]->insertItem(3,"DO T/C");
        combo[i]->insertItem(4,"DO Pulse");
        combo[i]->insertItem(5,"DO Latch");
        combo[i]->insertItem(6,"AI");
        combo[i]->insertItem(7,"AO");
        connect(combo[i], SIGNAL(currentIndexChanged(int)),this, SLOT(comboCellChanged()));
    }
    ui->selectProtocolConfirm->setEnabled(true);
}

void SettingsDialog::on_RtuAddSizeBox_currentIndexChanged(int index)
{
    ui->selectProtocolConfirm->setEnabled(true);
    index++;
}

void SettingsDialog::on_rtuAddressEdit_textChanged(const QString &arg1)
{
    QString Pstr;
    ui->selectProtocolConfirm->setEnabled(true);
    //*arg1 +="RTU Address changed";
    Pstr=arg1;
}

void SettingsDialog::on_ioboardTable_cellChanged(int row, int column)
{
    ui->selectProtocolConfirm->setEnabled(true);
    row++;column++;
}

void SettingsDialog::comboCellChanged()
{
    ui->selectProtocolConfirm->setEnabled(true);
}
MyThread::MyThread()
{
    //connect(this,sendData_Thread(),);
}
MyThread::~MyThread()
{}

 void MyThread::run()
 {
     struct pcap_pkthdr *header;
     QByteArray data;
     QString pStr;
     const u_char *pkt_data;
     struct bpf_program filter;                      /* 已经编译好的过滤器 */
     int res;
     char filter_app[] = "ether proto 0x88b8";       /* GOOSE Ethernet Type */
     if ((adhandle= pcap_open_live(m_pCurrentdev->name,65535,PCAP_OPENFLAG_PROMISCUOUS,1000,errbuf))== NULL)
       {
          pcap_freealldevs(m_pAlldevs);
          //QMessageBox::information(this, tr("IEC60870-5-104"),m_pCurrentdev->description );
       }
     pcap_compile(adhandle,&filter,filter_app,0,0);
     pcap_setfilter(adhandle,&filter);
     //pcap_loop(adhandle,-1,capturePackage,NULL);
    //QMessageBox::information(NULL, "IEC61850 GOOSE","Waaiting ...");
     //header->len=10;
     //put_into_buffer(data,header->len,RECEIVE);
     //current_ptr--;
     //emit sendData_1(global_io_buffer[current_ptr]);
     //current_ptr++;
     while((res = pcap_next_ex( adhandle, &header, &pkt_data)) >= 0){

             if(res == 0)
                 continue;/* 超时时间到 */
             for(unsigned int i=0;i<header->len;i++)data[i]=pkt_data[i];
             //pStr.sprintf("%02x %02x",pkt_data[12],pkt_data[13]);
             //QMessageBox::information(NULL, "IEC61850 GOOSE",pStr);
             put_into_buffer(data,header->len,RECEIVE);
             current_ptr--;
             emit sendData_1(global_io_buffer[current_ptr]);
             current_ptr++;
             //local_tv_sec = header->ts.tv_sec;
             //ltime=localtime(&local_tv_sec);
             //strftime( timestr, sizeof timestr, "%H:%M:%S", ltime);

             //printf("%s,%.6d len:%d\n", timestr, header->ts.tv_usec, header->len);
         }
 }


void SettingsDialog::on_com_connectButton_clicked()
{  
    int i,index;
    QTimer *time_clock=new QTimer(this);

    updateSettings();
    if(currentSettings.protocol_is_network==ETHERNET)
    {
        index=ui->netInterfaceListBox->currentIndex();
        for(m_pCurrentdev=m_pAlldevs, i=0; i< index;m_pCurrentdev=m_pCurrentdev->next, i++);
        //connect(myThread,SIGNAL(sendData_1(myio_buffer &)),MainWindow,SLOT(receiveData_1(myio_buffer &)));
        myThread.start();
        //TestThread.wait();
        ui->netInterfaceListBox->setEnabled(false);
    }
    else if(currentSettings.protocol_is_network==TCPIP)
    {
        int TCP_port=ui->TCP_PortLineEdit->text().toInt();
        if(currentSettings.protocolName=="IEC60870-5-104" && TCP_port!=2404)QMessageBox::information(this, tr("IEC60870-5-104"), tr("The value you input isn't the recommended port by Standard"));
        if(currentSettings.protocolName=="DNP LAN/WAN" && (TCP_port!=20000&&TCP_port!=19999&&TCP_port!=20001))QMessageBox::information(this, tr("DNP LAN/WAN"), tr("The port should be 19999,20000 or 20001"));
        if(currentSettings.isMaster==0)                                     //if Slave Mode
        {
            m_pTcpServer = new QTcpServer(this);
            if(ui->TCP_AcceptAllHostCheckBox->isChecked()==true)
            {
                if (!m_pTcpServer->listen(QHostAddress::Any, TCP_port))
                    {
                                                                                //listen fail
                        qDebug()<<m_pTcpServer->errorString();
                        QMessageBox::information(this, tr("Listen Fail"), m_pTcpServer->errorString());
                    }
                QMessageBox::information(this, tr("TCP/IP Connnection"), tr("Any of IP request will be accepted"));
            }
            else
            {
                if (!m_pTcpServer->listen(QHostAddress(currentSettings.IPAddress), TCP_port))
                {
                    //listen fail
                    qDebug()<<m_pTcpServer->errorString();
                    QMessageBox::information(this, tr("Listen Fail"), m_pTcpServer->errorString());
                }
                QMessageBox::information(this, tr("IEC60870-5-104"), tr("Only specified IP request will be accepted"));
            }
            //m_pConnectSocket = m_pTcpServer->nextPendingConnection();
        connect(m_pTcpServer, SIGNAL(newConnection()), this, SLOT(slotConnectClient()));
        //connect(m_pTcpServer, SIGNAL(readyRead()), this, SLOT(slotReadTcpData()));
        //connect(m_pTcpServer, SIGNAL(disconnected()),m_pTcpServer, SLOT(deleteLater()));
        //connect(m_pTcpServer, SIGNAL(disconnected()),this, SLOT(slotDisconnected()));

        }
        else                                                                    //if Master Mode
        {
            connect(time_clock,SIGNAL(timeout()),this,SLOT(masterPollTime()));
            currentSettings.poll_timer=ui->Master_pollL_TimeineEdit->text().toUInt();
            time_clock->start(currentSettings.poll_timer);
            m_pConnectSocket = new QTcpSocket(this);
            m_pConnectSocket->connectToHost(QHostAddress(currentSettings.IPAddress),TCP_port);

            connect(m_pConnectSocket, SIGNAL(readyRead()), this, SLOT(slotReadTcpData()));
            connect(m_pConnectSocket, SIGNAL(disconnected()),m_pConnectSocket, SLOT(deleteLater()));
            connect(m_pConnectSocket, SIGNAL(disconnected()),this, SLOT(slotDisconnected()));
        }
        ui->NetworkOptionsGroupBox->setEnabled(false);
    }
    else
    {
        emit serial_connect();
    }

    ui->com_connectButton->setEnabled(false);
    ui->com_disconnectButton->setEnabled(true);

    //ui->TCP_PortLineEdit->setEnabled(false);
    //ui->TCP_ipLineEdit1->setEnabled(false);
    //ui->TCP_tcpudpcomboBox->setEnabled(false);
    //ui->TCP_AcceptAllHostCheckBox->setEnabled(false);

}
void SettingsDialog::masterPollTime()
{
    dnp3.DNP3_masterPollTime(m_pConnectSocket);

    current_ptr--;
    emit sendData(global_io_buffer[current_ptr]);
    current_ptr++;
    //QMessageBox::information(this, tr("DNP Master"), "Timer");
}

void SettingsDialog::on_com_disconnectButton_clicked()
{

    if(currentSettings.protocol_is_network==ETHERNET)       // GOOSE
    {
        pcap_close(adhandle);
        ui->netInterfaceListBox->setEnabled(true);
        if(myThread.isRunning())myThread.exit();
    }
    else if(currentSettings.protocol_is_network==TCPIP)
    {
        if(currentSettings.isMaster==0)
        {
            m_pTcpServer->~QTcpServer();
            ui->TCP_AcceptAllHostCheckBox->setEnabled(true);
        }
        else
        {
            m_pConnectSocket->~QTcpSocket();
        }

        ui->NetworkOptionsGroupBox->setEnabled(true);
    }
    else
    {
        emit serial_disconnect();
    }
    ui->com_connectButton->setEnabled(true);
    ui->com_disconnectButton->setEnabled(false);
    dnp3.reset_link=true;
}
//收到客户端连接
void SettingsDialog::slotConnectClient()
{
    m_pConnectSocket = m_pTcpServer->nextPendingConnection();
    connect(m_pConnectSocket, SIGNAL(readyRead()), this, SLOT(slotReadTcpData()));
    connect(m_pConnectSocket, SIGNAL(disconnected()),m_pConnectSocket, SLOT(deleteLater()));
    connect(m_pConnectSocket, SIGNAL(disconnected()),this, SLOT(slotDisconnected()));
    //QMessageBox::information(this, tr("IEC60870-5-104"), tr(" A client connection request coming...... "));
}

//接收数据
void SettingsDialog::slotReadTcpData()
{
    QByteArray byteArray;

    byteArray = m_pConnectSocket->readAll();
    //QMessageBox::information(this, tr("IEC60870-5-104"), tr("Data Received"));
    //处理数据

    put_into_buffer(byteArray,byteArray.size(),RECEIVE);
    current_ptr--;
    emit sendData(global_io_buffer[current_ptr]);
    current_ptr++;
    switch(currentSettings.protocolId)
    {
       case DNP_TCP:          //DNP LAN/WAN
        //QMessageBox::information(this, tr("DNP3.0 LAN/WAN"), tr(" A client connection request coming...... "));
        dnp3.DNP3_processPacket(m_pConnectSocket,byteArray,currentSettings.isMaster);
        break;
       case IEC_104:          //IEC60870-5-104
        iec101_104.IEC101_104_processPacket(m_pConnectSocket,byteArray);
        break;
       default:
        break;
    }
    //current_ptr--;
    for(int i=0;i<MAX_IO_BUFFER;i++)
    {
        if(global_io_buffer[i].valid_flag==true)
        {
           emit sendData(global_io_buffer[i]);
        }
    }
        //current_ptr++;
}

//断开连接
void SettingsDialog::slotDisconnected()
{
   // m_pConnectSocket->disconnect();
}

void SettingsDialog::on_IEC101_triggerEmulatorButton_clicked()
{
    //QString Pstr;
    //int current_ptr;
    int index;
    unsigned char point_type_id[8]={iec101_104.M_SP_NA_1,iec101_104.M_DP_NA_1,iec101_104.M_SP_TB_1,
                                    iec101_104.M_DP_TB_1,iec101_104.M_ME_NA_1,iec101_104.M_ME_TD_1,
                                    iec101_104.M_ME_TE_1,iec101_104.M_ME_TF_1};

    index=ui->IEC101_pointTypeComboBox->currentIndex();
    iec101_104.cfg.point_type_id=point_type_id[index];
    iec101_104.cfg.emu_info_object_address=ui->IEC101_emuInfoObjectAddresslineEdit->text().toLong();
    switch(index)
    {
        case 0:
        case 1:
        case 2:
        case 3:
            iec101_104.cfg.emu_di_status=ui->IEC101_emuDIStatusComboBox->currentIndex();
            break;
        case 4:
        case 5:
        case 6:
            iec101_104.cfg.emu_ai_value_int=ui->IEC101_emuAIpointValuelineEdit->text().toInt();
            break;
        case 7:
            iec101_104.cfg.emu_ai_value_float=ui->IEC101_emuAIpointValuelineEdit->text().toFloat();
            break;
        default:
            break;
    }


    if(iec101_104.cfg.emu_info_object_address==0)
    {
       QMessageBox::information(this, tr("IEC60870-5-104"), tr("Please Input valid info object address first!"));
       return;
    }


    iec101_104.cfg.trigger_flag=0xaa;
    //return;
    //current_ptr=
    iec101_104.IEC101_104_spont(m_pConnectSocket);
    current_ptr--;
    emit sendData(global_io_buffer[current_ptr]);
    current_ptr++;
}

void SettingsDialog::on_IEC101_pointTypeComboBox_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0: //SPS
    case 1: //DPS
    case 2: //SPS with CP56Time2a Timetag
    case 3: //DPS with CP56Time2a Timetag
        ui->IEC101_emuDIStatusComboBox->setVisible(true);
        ui->IEC101_emuAIpointValuelineEdit->setVisible(false);
        break;
    case 4: //AI
    case 5: //Normolized AI with CP56Time2a Timetag
    case 6: //Scaled AI with CP56Time2a Timetag
    case 7: //Float AI IEEE STD 754 with CP56Time2a Timetag
        ui->IEC101_emuDIStatusComboBox->setVisible(false);
        ui->IEC101_emuAIpointValuelineEdit->setVisible(true);
        break;
    default:break;

    }
}

void SettingsDialog::on_TCP_AcceptAllHostCheckBox_stateChanged(int arg1)
{
    if(!arg1)
    {
        ui->TCP_ipLineEdit1->setEnabled(true);
        ui->TCP_hostipLabel->setEnabled(true);
    }
    else
    {
        ui->TCP_ipLineEdit1->setEnabled(false);
        ui->TCP_hostipLabel->setEnabled(false);
    }
}


void SettingsDialog::on_IEC101_TimeBasecomboBox_currentIndexChanged(int index)                      //Time Base changed
{
    if(index==1)iec101_104.cfg.su=IEC_SU;
    else iec101_104.cfg.su=0x00;
}

void SettingsDialog::on_IEC101_commonAddressLineEdit_textEdited(const QString &arg1)                //Common Address changed
{
    iec101_104.cfg.common_address_config=arg1.toInt();
}

void SettingsDialog::on_IEC101_diInfoObjectAddresslineEdit_textChanged(const QString &arg1)        //SPS info Address changed
{
    iec101_104.cfg.di_info_object_address=arg1.toInt();
}


void SettingsDialog::on_IEC101_aiInfoObjectAddresslineEdit_textChanged(const QString &arg1)
{
    iec101_104.cfg.ai_info_object_address=arg1.toInt();
}

void SettingsDialog::on_IEC101_interogateDIcheckBox_clicked()                                  //Enable DI Interrogate
{
    if(ui->IEC101_interogateDIcheckBox->isChecked()==true)
    {
        ui->IEC101_interrogateDIlineEdit->setEnabled(true);
        iec101_104.cfg.interrogate_start|=0x01;
        iec101_104.cfg.interro_di_type=ui->IEC101_diTypeComboBox->currentIndex();
    }
    else
    {
        ui->IEC101_interrogateDIlineEdit->setEnabled(false);
        iec101_104.cfg.interrogate_start&=0xfe;
    }
}

void SettingsDialog::on_IEC101_emuDIStatusComboBox_currentIndexChanged(int index)
{
    iec101_104.cfg.emu_di_status=index;
}

void SettingsDialog::on_IEC101_InterroDIComboBox_currentIndexChanged(int index)
{
    for(int i=0;i<MAX_INTERRO_POINT;i++)iec101_104.cfg.interro_di_status[i]=index;
}

void SettingsDialog::on_IEC101_emuAIpointValuelineEdit_textChanged(const QString &arg1)
{
    iec101_104.cfg.emu_ai_value_float=arg1.toFloat();
}

void SettingsDialog::on_IEC101_interrogateDIlineEdit_textChanged(const QString &arg1)
{
    iec101_104.cfg.interrogate_number_di=arg1.toInt();
    if(iec101_104.cfg.interrogate_number_di>MAX_INTERRO_POINT)
    {
        iec101_104.cfg.interrogate_number_di=MAX_INTERRO_POINT;
    }
    if(iec101_104.cfg.interrogate_number_di<1)
    {
        iec101_104.cfg.interrogate_number_di=1;
    }
}

void SettingsDialog::on_IEC101_diTypeComboBox_currentIndexChanged(int index)
{
    iec101_104.cfg.interro_di_type=index;
}

void SettingsDialog::on_DNP3_RemoteAddressLineEdit_textChanged(const QString &arg1)
{
    dnp3.cfg.remote_address=arg1.toInt();
}

void SettingsDialog::on_DNP3_LocalAddressLineEdit_textChanged(const QString &arg1)
{
    dnp3.cfg.local_address=arg1.toInt();
}

void SettingsDialog::on_slaveMasterBox_currentIndexChanged(int index)
{
    if(currentSettings.protocol_is_network==ETHERNET)return;
    currentSettings.isMaster=index;

    if(index==1)                                    //Slave Mode
    {
        ui->DNP3_MasterGroupBox->setEnabled(false);
        ui->TCP_AcceptAllHostCheckBox->setEnabled(true);
        ui->TCP_hostipLabel->setText("Master IP");
        ui->TCP_hostipLabel->setEnabled(false);
        ui->TCP_ipLineEdit1->setEnabled(false);
        ui->TCP_AcceptAllHostCheckBox->show();
        if(currentSettings.protocolId==MODBUS_RTU ||currentSettings.protocolId==MODBUS_TCP||currentSettings.protocolId==MODBUS_ASCII )
        {
            ui->tabWidget->setTabEnabled(2,true);
            ui->tabWidget->setTabEnabled(1,false);
            currentSettings.smodbus.rtu_role=MODBUS_SLAVE;

            {
                currentSettings.smodbus.my_rtu_id=1;
                currentSettings.smodbus.exceptionCode=0;
                currentSettings.smodbus.exceptionMode=0;
                for (int i=0;i<MAX_MODBUS_POINT;i++)
                {
                    currentSettings.smodbus.coilData[i]=0x00;
                    currentSettings.smodbus.disInputData[i]=0x00;
                    currentSettings.smodbus.inRegData[i]=0x00;
                    currentSettings.smodbus.holdRegData[i]=0x00;
                }
            }
        }
    }
    else
    {                                               //Master Mode
        ui->DNP3_MasterGroupBox->setEnabled(true);
        ui->TCP_AcceptAllHostCheckBox->setEnabled(false);
        ui->TCP_hostipLabel->setText("Slave IP");
        ui->TCP_hostipLabel->setEnabled(true);
        ui->TCP_ipLineEdit1->setEnabled(true);
        ui->TCP_AcceptAllHostCheckBox->hide();
        if(currentSettings.protocolId==MODBUS_RTU ||currentSettings.protocolId==MODBUS_TCP||currentSettings.protocolId==MODBUS_ASCII )
        {
            ui->tabWidget->setTabEnabled(1,true);
            ui->tabWidget->setTabEnabled(2,false);
        }
        currentSettings.smodbus.rtu_role=MODBUS_MASTER;
    }
}

void SettingsDialog::on_TCP_ipLineEdit1_textChanged(const QString &arg1)
{
    currentSettings.IPAddress=arg1;
    //QMessageBox::information(this, tr("IEC60870-5-104"), arg1);
}

void SettingsDialog::on_DNP3_timeSyncCheckBox_clicked(bool checked)
{
    dnp3.cfg.time_sync=checked;                                                     //True: Time Sync will send to slave
}

void SettingsDialog::on_DNP3_triggerEmulatorButton_clicked()
{
    //QString Pstr;
    //int current_ptr;
    int index;
    unsigned char point_type_id[8]={iec101_104.M_SP_NA_1,iec101_104.M_DP_NA_1,iec101_104.M_SP_TB_1,
                                    iec101_104.M_DP_TB_1,iec101_104.M_ME_NA_1,iec101_104.M_ME_TD_1,
                                    iec101_104.M_ME_TE_1,iec101_104.M_ME_TF_1};

    index=ui->IEC101_pointTypeComboBox->currentIndex();
    iec101_104.cfg.point_type_id=point_type_id[index];
    iec101_104.cfg.emu_info_object_address=ui->IEC101_emuInfoObjectAddresslineEdit->text().toLong();
    switch(index)
    {
        case 0:
        case 1:
        case 2:
        case 3:
            dnp3.cfg.emu_di_status=ui->IEC101_emuDIStatusComboBox->currentIndex();
            break;
        case 4:
        case 5:
        case 6:
            dnp3.cfg.emu_ai_value_int=ui->IEC101_emuAIpointValuelineEdit->text().toInt();
            break;
        case 7:
            dnp3.cfg.emu_ai_value_float=ui->IEC101_emuAIpointValuelineEdit->text().toFloat();
            break;
        default:
            break;
    }


    if(iec101_104.cfg.emu_info_object_address==0)
    {
       QMessageBox::information(this, tr("IEC60870-5-104"), tr("Please Input valid info object address first!"));
       return;
    }


    iec101_104.cfg.trigger_flag=0xaa;
    iec101_104.IEC101_104_spont(m_pConnectSocket);
    current_ptr--;
    emit sendData(global_io_buffer[current_ptr]);
    current_ptr++;
}


void SettingsDialog::on_gooseSendButton_clicked()
{
    iecgoose.GOOSE_send(adhandle);
}


void SettingsDialog::on_protocolChoose_currentIndexChanged(int index)
{

    int i;
    QString Pstr,tempStr;//="0000";

    Pstr=currentSettings.protocolName=ui->protocolChoose->currentText();
    currentSettings.protocolId=(enum protocol_Id)index;
    //Pstr.append(" Selected"); //or    Pstr+=" Selected";
    //Pstr=QString::number(i);

    //QMessageBox::about(this,tr("Protocol"),Pstr+" Selected");


    switch(currentSettings.protocolId)
    {
        case DNP_SERIAL:
            currentSettings.protocol_is_network=SERIAL;
            break;
        case DNP_TCP:
            currentSettings.protocol_is_network=TCPIP;
            ui->TCP_PortLineEdit->setText("20000");
            ui->TCP_tcpudpcomboBox->setEnabled(true);
            dnp3.cfg.remote_address=ui->DNP3_RemoteAddressLineEdit->text().toInt();
            dnp3.cfg.local_address=ui->DNP3_LocalAddressLineEdit->text().toInt();
            ui->slaveMasterBox->setEnabled(true);
            ui->slaveMasterBox->clear();
            ui->slaveMasterBox->addItem("Slave");
            ui->slaveMasterBox->addItem("Master");
            break;
        case IEC_101:
            currentSettings.protocol_is_network=SERIAL;
            ui->parityBox->setCurrentIndex(1);            //Parity: Even
            break;
        case IEC_103:
            currentSettings.protocol_is_network=SERIAL;
            ui->parityBox->setCurrentIndex(1);            //Parity: Even
            break;
        case IEC_104:
            ui->tabWidget->insertTab(2,map[1].value(2).first,map[1].value(2).second);
            ui->TCP_PortLineEdit->setText("2404");
            ui->TCP_tcpudpcomboBox->setEnabled(false);
            currentSettings.protocol_is_network=TCPIP;
            break;
        case MODBUS_RTU:
        case MODBUS_ASCII:
            currentSettings.protocol_is_network=SERIAL;
            ui->tabWidget->setTabEnabled(1,1);
            ui->slaveMasterBox->setEnabled(true);
            ui->slaveMasterBox->clear();
            ui->slaveMasterBox->addItem("Master");
            ui->slaveMasterBox->addItem("Slave");
            //ui->tabWidget->setTabEnabled(1,1);
            break;
        case MODBUS_TCP:
            currentSettings.protocol_is_network=TCPIP;
            ui->TCP_PortLineEdit->setText("502");
            ui->TCP_tcpudpcomboBox->setEnabled(true);
            //dnp3.cfg.remote_address=ui->DNP3_RemoteAddressLineEdit->text().toInt();
            //dnp3.cfg.local_address=ui->DNP3_LocalAddressLineEdit->text().toInt();
            ui->slaveMasterBox->setEnabled(true);
            ui->slaveMasterBox->clear();
            ui->slaveMasterBox->addItem("Master");
            ui->slaveMasterBox->addItem("Slave");
            //ui->tabWidget->setTabEnabled(1,1);
            break;
        case IEC61850_GOOSE:
            currentSettings.protocol_is_network=ETHERNET;
            ui->netInterfaceListBox->setEnabled(true);
            ui->netInterfaceLabel->setEnabled(true);
            ui->slaveMasterBox->setEnabled(true);
            ui->slaveMasterBox->clear();
            ui->netInterfaceListBox->clear();
            ui->slaveMasterBox->addItem("Publisher");
            ui->slaveMasterBox->addItem("Subscriber");

            if (pcap_findalldevs(&m_pAlldevs, errbuf) == -1)
            {
                QMessageBox::information(this,"Network Interface", "Error in pcap_findalldevs");
            }


            for(m_pCurrentdev = m_pAlldevs,i=1; m_pCurrentdev!= NULL; m_pCurrentdev = m_pCurrentdev->next,i++)
            {
                tempStr.sprintf("%d. ",i);
                tempStr.append(m_pCurrentdev->description);
                ui->netInterfaceListBox->addItem(tempStr);
            }
            break;

        case PPC_HNZ:
            ui->dataBitsBox->setCurrentIndex(0);
            ui->dataBitsBox->setEnabled(false);
            ui->parityBox->setCurrentIndex(0);
            ui->parityBox->setEnabled(false);
            ui->stopBitsBox->setCurrentIndex(0);
            ui->stopBitsBox->setEnabled(false);
            ui->parametersBox_2->setEnabled(true);
            ui->slaveMasterBox->setCurrentIndex(1);
            ui->slaveMasterBox->setEnabled(false);
            ui->IOBoardLabel->setEnabled(true);
            ui->ioboardInput->setEnabled(true);
            ui->ioboardButton->setEnabled(true);
            ui->parametersBox_2->setVisible(true);
            ui->IOBoardLabel->setVisible(true);
            ui->ioboardInput->setVisible(true);
            ui->ioboardButton->setVisible(true);
            if(hnz_tool_settings.ioBoardNumber>0 &&hnz_tool_settings.ioBoardNumber<=HNZ_BOARD_MAX)ui->ioboardTable->setVisible(true);
            currentSettings.protocol_is_network=SERIAL;
            break;
        default:
            ui->parametersBox_2->setVisible(false);
            ui->IOBoardLabel->setVisible(false);
            ui->ioboardInput->setVisible(false);
            ui->ioboardButton->setVisible(false);
            ui->ioboardTable->setVisible(false);
            currentSettings.protocol_is_network=NONE_COM;
            break;

    }

    if(currentSettings.protocol_is_network==TCPIP)
    {
        ui->SerialOptionsGroupBox->setEnabled(false);
        ui->NetworkOptionsGroupBox->setEnabled(true);
        ui->netInterfaceLabel->setEnabled(false);
    }
    else if(currentSettings.protocol_is_network==SERIAL)
    {
        ui->SerialOptionsGroupBox->setEnabled(true);
        ui->NetworkOptionsGroupBox->setEnabled(false);
        ui->netInterfaceLabel->setEnabled(false);
        fillPortsParameters();
        fillPortsInfo();
    }
    else if(currentSettings.protocol_is_network==ETHERNET)
    {
        ui->netInterfaceListBox->setEnabled(true);
        ui->NetworkOptionsGroupBox->setEnabled(false);
    }
    else
    {
        ui->SerialOptionsGroupBox->setEnabled(false);
        ui->NetworkOptionsGroupBox->setEnabled(false);
        ui->netInterfaceLabel->setEnabled(false);
    }
    ui->selectProtocolConfirm->setEnabled(true);
    ui->com_connectButton->setEnabled(true);
    hnz_tool_settings.lru_address_size=ui->RtuAddSizeBox->currentIndex()+1;

}

void SettingsDialog::inReg_update()
{
    unsigned int value;
    unsigned int regAddress;
    bool ok1,ok2;
    QString str1,str2;

    for(int i=0;i<MAX_MODBUS_POINT;i++)currentSettings.smodbus.inRegData[i]=0;

    str1 = ui->inReg_0->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->inRegAddr_0->text();
    regAddress=str2.toUInt(&ok2,10);

    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.inRegData[regAddress]=value;
    }

    str1 = ui->inReg_1->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->inRegAddr_1->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.inRegData[regAddress]=value;
    }
    str1 = ui->inReg_2->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->inRegAddr_2->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.inRegData[regAddress]=value;
    }
    str1 = ui->inReg_3->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->inRegAddr_3->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.inRegData[regAddress]=value;
    }
    str1 = ui->inReg_4->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->inRegAddr_4->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.inRegData[regAddress]=value;
    }
    str1 = ui->inReg_5->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->inRegAddr_5->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.inRegData[regAddress]=value;
    }
    str1 = ui->inReg_6->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->inRegAddr_6->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.inRegData[regAddress]=value;
    }
    str1 = ui->inReg_7->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->inRegAddr_7->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.inRegData[regAddress]=value;
    }
    str1 = ui->inReg_8->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->inRegAddr_8->text();
    regAddress=str2.toUInt(&ok2,10);

    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.inRegData[regAddress]=value;
    }

    str1 = ui->inReg_9->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->inRegAddr_9->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.inRegData[regAddress]=value;
    }
    str1 = ui->inReg_10->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->inRegAddr_10->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.inRegData[regAddress]=value;
    }
    str1 = ui->inReg_11->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->inRegAddr_11->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.inRegData[regAddress]=value;
    }
    str1 = ui->inReg_12->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->inRegAddr_12->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.inRegData[regAddress]=value;
    }
    str1 = ui->inReg_13->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->inRegAddr_13->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.inRegData[regAddress]=value;
    }
    str1 = ui->inReg_14->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->inRegAddr_14->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.inRegData[regAddress]=value;
    }
    str1 = ui->inReg_15->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->inRegAddr_15->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.inRegData[regAddress]=value;
    }
}

void SettingsDialog::holdReg_update()
{
    unsigned int value;
    unsigned int regAddress;
    bool ok1,ok2;
    QString str1,str2;

    for(int i=0;i<MAX_MODBUS_POINT;i++)currentSettings.smodbus.holdRegData[i]=0;

    str1 = ui->holdReg_0->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->holdRegAddr_0->text();
    regAddress=str2.toUInt(&ok2,10);

    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.holdRegData[regAddress]=value;
    }

    str1 = ui->holdReg_1->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->holdRegAddr_1->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.holdRegData[regAddress]=value;
    }
    str1 = ui->holdReg_2->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->holdRegAddr_2->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.holdRegData[regAddress]=value;
    }
    str1 = ui->holdReg_3->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->holdRegAddr_3->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.holdRegData[regAddress]=value;
    }
    str1 = ui->holdReg_4->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->holdRegAddr_4->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.holdRegData[regAddress]=value;
    }
    str1 = ui->holdReg_5->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->holdRegAddr_5->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.holdRegData[regAddress]=value;
    }
    str1 = ui->holdReg_6->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->holdRegAddr_6->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.holdRegData[regAddress]=value;
    }
    str1 = ui->holdReg_7->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->holdRegAddr_7->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.holdRegData[regAddress]=value;
    }
    str1 = ui->holdReg_8->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->holdRegAddr_8->text();
    regAddress=str2.toUInt(&ok2,10);

    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.holdRegData[regAddress]=value;
    }

    str1 = ui->holdReg_9->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->holdRegAddr_9->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.holdRegData[regAddress]=value;
    }
    str1 = ui->holdReg_10->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->holdRegAddr_10->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.holdRegData[regAddress]=value;
    }
    str1 = ui->holdReg_11->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->holdRegAddr_11->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.holdRegData[regAddress]=value;
    }
    str1 = ui->holdReg_12->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->holdRegAddr_12->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.holdRegData[regAddress]=value;
    }
    str1 = ui->holdReg_13->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->holdRegAddr_13->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.holdRegData[regAddress]=value;
    }
    str1 = ui->holdReg_14->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->holdRegAddr_14->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.holdRegData[regAddress]=value;
    }
    str1 = ui->holdReg_15->text();
    value=str1.toUInt(&ok1,16);
    str2 = ui->holdRegAddr_15->text();
    regAddress=str2.toUInt(&ok2,10);
    if(ok1&&ok2&&(regAddress<MAX_MODBUS_POINT))
    {
        currentSettings.smodbus.holdRegData[regAddress]=value;
    }
}
void SettingsDialog::on_serverIDEdit_valueChanged(int arg1)
{
    currentSettings.smodbus.my_rtu_id=arg1;
}

void SettingsDialog::discrete_update()
{
    unsigned int discreteAddress;
    bool ok;
    QString str;

    for(int i=0;i<MAX_MODBUS_POINT;i++)currentSettings.smodbus.disInputData[i]=0;

    str = ui->diAddr_0->text();
    discreteAddress=str.toUInt(&ok,10);
    if(ok&&(discreteAddress<MAX_MODBUS_POINT)&&(ui->diCheck_0->isChecked()))
    {
        currentSettings.smodbus.disInputData[discreteAddress]=1;
    }

    str = ui->diAddr_1->text();
    discreteAddress=str.toUInt(&ok,10);
    if(ok&&(discreteAddress<MAX_MODBUS_POINT)&&(ui->diCheck_1->isChecked()))
    {
        currentSettings.smodbus.disInputData[discreteAddress]=1;
    }

    str = ui->diAddr_2->text();
    discreteAddress=str.toUInt(&ok,10);
    if(ok&&(discreteAddress<MAX_MODBUS_POINT)&&(ui->diCheck_2->isChecked()))
    {
        currentSettings.smodbus.disInputData[discreteAddress]=1;
    }

    str = ui->diAddr_3->text();
    discreteAddress=str.toUInt(&ok,10);
    if(ok&&(discreteAddress<MAX_MODBUS_POINT)&&(ui->diCheck_3->isChecked()))
    {
        currentSettings.smodbus.disInputData[discreteAddress]=1;
    }

    str = ui->diAddr_4->text();
    discreteAddress=str.toUInt(&ok,10);
    if(ok&&(discreteAddress<MAX_MODBUS_POINT)&&(ui->diCheck_4->isChecked()))
    {
        currentSettings.smodbus.disInputData[discreteAddress]=1;
    }

    str = ui->diAddr_5->text();
    discreteAddress=str.toUInt(&ok,10);
    if(ok&&(discreteAddress<MAX_MODBUS_POINT)&&(ui->diCheck_5->isChecked()))
    {
        currentSettings.smodbus.disInputData[discreteAddress]=1;
    }

    str = ui->diAddr_6->text();
    discreteAddress=str.toUInt(&ok,10);
    if(ok&&(discreteAddress<MAX_MODBUS_POINT)&&(ui->diCheck_6->isChecked()))
    {
        currentSettings.smodbus.disInputData[discreteAddress]=1;
    }

    str = ui->diAddr_7->text();
    discreteAddress=str.toUInt(&ok,10);
    if(ok&&(discreteAddress<MAX_MODBUS_POINT)&&(ui->diCheck_7->isChecked()))
    {
        currentSettings.smodbus.disInputData[discreteAddress]=1;
    }

    str = ui->diAddr_8->text();
    discreteAddress=str.toUInt(&ok,10);
    if(ok&&(discreteAddress<MAX_MODBUS_POINT)&&(ui->diCheck_8->isChecked()))
    {
        currentSettings.smodbus.disInputData[discreteAddress]=1;
    }

    str = ui->diAddr_9->text();
    discreteAddress=str.toUInt(&ok,10);
    if(ok&&(discreteAddress<MAX_MODBUS_POINT)&&(ui->diCheck_9->isChecked()))
    {
        currentSettings.smodbus.disInputData[discreteAddress]=1;
    }

    str = ui->diAddr_10->text();
    discreteAddress=str.toUInt(&ok,10);
    if(ok&&(discreteAddress<MAX_MODBUS_POINT)&&(ui->diCheck_10->isChecked()))
    {
        currentSettings.smodbus.disInputData[discreteAddress]=1;
    }

    str = ui->diAddr_11->text();
    discreteAddress=str.toUInt(&ok,10);
    if(ok&&(discreteAddress<MAX_MODBUS_POINT)&&(ui->diCheck_11->isChecked()))
    {
        currentSettings.smodbus.disInputData[discreteAddress]=1;
    }

    str = ui->diAddr_12->text();
    discreteAddress=str.toUInt(&ok,10);
    if(ok&&(discreteAddress<MAX_MODBUS_POINT)&&(ui->diCheck_12->isChecked()))
    {
        currentSettings.smodbus.disInputData[discreteAddress]=1;
    }

    str = ui->diAddr_13->text();
    discreteAddress=str.toUInt(&ok,10);
    if(ok&&(discreteAddress<MAX_MODBUS_POINT)&&(ui->diCheck_13->isChecked()))
    {
        currentSettings.smodbus.disInputData[discreteAddress]=1;
    }

    str = ui->diAddr_14->text();
    discreteAddress=str.toUInt(&ok,10);
    if(ok&&(discreteAddress<MAX_MODBUS_POINT)&&(ui->diCheck_14->isChecked()))
    {
        currentSettings.smodbus.disInputData[discreteAddress]=1;
    }

    str = ui->diAddr_15->text();
    discreteAddress=str.toUInt(&ok,10);
    if(ok&&(discreteAddress<MAX_MODBUS_POINT)&&(ui->diCheck_15->isChecked()))
    {
        currentSettings.smodbus.disInputData[discreteAddress]=1;
    }
}

void SettingsDialog::coil_update()
{
    unsigned int address;
    bool ok;
    QString str;

    for(int i=0;i<MAX_MODBUS_POINT;i++)currentSettings.smodbus.coilData[i]=0;

    str = ui->coilAddr_0->text();
    address=str.toUInt(&ok,10);
    if(ok&&(address<MAX_MODBUS_POINT)&&(ui->coilCheck_0->isChecked()))
    {
        currentSettings.smodbus.coilData[address]=1;
    }

    str = ui->coilAddr_1->text();
    address=str.toUInt(&ok,10);
    if(ok&&(address<MAX_MODBUS_POINT)&&(ui->coilCheck_1->isChecked()))
    {
        currentSettings.smodbus.coilData[address]=1;
    }

    str = ui->coilAddr_2->text();
    address=str.toUInt(&ok,10);
    if(ok&&(address<MAX_MODBUS_POINT)&&(ui->coilCheck_2->isChecked()))
    {
        currentSettings.smodbus.coilData[address]=1;
    }

    str = ui->coilAddr_3->text();
    address=str.toUInt(&ok,10);
    if(ok&&(address<MAX_MODBUS_POINT)&&(ui->coilCheck_3->isChecked()))
    {
        currentSettings.smodbus.coilData[address]=1;
    }

    str = ui->coilAddr_4->text();
    address=str.toUInt(&ok,10);
    if(ok&&(address<MAX_MODBUS_POINT)&&(ui->coilCheck_4->isChecked()))
    {
        currentSettings.smodbus.coilData[address]=1;
    }

    str = ui->coilAddr_5->text();
    address=str.toUInt(&ok,10);
    if(ok&&(address<MAX_MODBUS_POINT)&&(ui->coilCheck_5->isChecked()))
    {
        currentSettings.smodbus.coilData[address]=1;
    }

    str = ui->coilAddr_6->text();
    address=str.toUInt(&ok,10);
    if(ok&&(address<MAX_MODBUS_POINT)&&(ui->coilCheck_6->isChecked()))
    {
        currentSettings.smodbus.coilData[address]=1;
    }

    str = ui->coilAddr_7->text();
    address=str.toUInt(&ok,10);
    if(ok&&(address<MAX_MODBUS_POINT)&&(ui->coilCheck_7->isChecked()))
    {
        currentSettings.smodbus.coilData[address]=1;
    }

    str = ui->coilAddr_8->text();
    address=str.toUInt(&ok,10);
    if(ok&&(address<MAX_MODBUS_POINT)&&(ui->coilCheck_8->isChecked()))
    {
        currentSettings.smodbus.coilData[address]=1;
    }

    str = ui->coilAddr_9->text();
    address=str.toUInt(&ok,10);
    if(ok&&(address<MAX_MODBUS_POINT)&&(ui->coilCheck_9->isChecked()))
    {
        currentSettings.smodbus.coilData[address]=1;
    }

    str = ui->coilAddr_10->text();
    address=str.toUInt(&ok,10);
    if(ok&&(address<MAX_MODBUS_POINT)&&(ui->coilCheck_10->isChecked()))
    {
        currentSettings.smodbus.coilData[address]=1;
    }

    str = ui->coilAddr_11->text();
    address=str.toUInt(&ok,10);
    if(ok&&(address<MAX_MODBUS_POINT)&&(ui->coilCheck_11->isChecked()))
    {
        currentSettings.smodbus.coilData[address]=1;
    }

    str = ui->coilAddr_12->text();
    address=str.toUInt(&ok,10);
    if(ok&&(address<MAX_MODBUS_POINT)&&(ui->coilCheck_12->isChecked()))
    {
        currentSettings.smodbus.coilData[address]=1;
    }

    str = ui->coilAddr_13->text();
    address=str.toUInt(&ok,10);
    if(ok&&(address<MAX_MODBUS_POINT)&&(ui->coilCheck_13->isChecked()))
    {
        currentSettings.smodbus.coilData[address]=1;
    }

    str = ui->coilAddr_14->text();
    address=str.toUInt(&ok,10);
    if(ok&&(address<MAX_MODBUS_POINT)&&(ui->coilCheck_14->isChecked()))
    {
        currentSettings.smodbus.coilData[address]=1;
    }

    str = ui->coilAddr_15->text();
    address=str.toUInt(&ok,10);
    if(ok&&(address<MAX_MODBUS_POINT)&&(ui->coilCheck_15->isChecked()))
    {
        currentSettings.smodbus.coilData[address]=1;
    }
}

void SettingsDialog::on_sModbusSettingsApplyButton_clicked()
{
    inReg_update();
    holdReg_update();
    discrete_update();
    coil_update();
}

#define BigEndian           0
#define LittleEndian        1
#define BigEndianByteSwap   2
#define LittleEndianByte    3

void SettingsDialog::leftToRightFormatConvert(unsigned long int oriData, int bytes)
{
    QString pstr;  
    QString analogFormat=ui->analogFormatBox->currentText();

    union UreOrderData
    {
        unsigned char uChar[4];
        unsigned int  uint[2];
        unsigned long uLongData;
    };

    UreOrderData reFormatData;
    UreOrderData inputData;
    inputData.uLongData=oriData;
    if(bytes==1)inputData.uLongData&=0xff;
    if(bytes==2)inputData.uLongData&=0xffff;

    if(analogFormat.compare(QStringLiteral("Little_Endian"))==0)
    {
        switch (bytes)
        {
            case 2:
                reFormatData.uChar[0]=inputData.uChar[1];
                reFormatData.uChar[1]=inputData.uChar[0];
                break;
            case 4:
                reFormatData.uChar[0]=inputData.uChar[3];
                reFormatData.uChar[1]=inputData.uChar[2];
                reFormatData.uChar[2]=inputData.uChar[1];
                reFormatData.uChar[3]=inputData.uChar[0];
                break;
            default:
            break;
        }
    }
    else if(analogFormat.compare(QStringLiteral("Big_Endian_Byte_Swap"))==0)
    {
        switch (bytes)
        {
            case 4:
                reFormatData.uChar[0]=inputData.uChar[1];
                reFormatData.uChar[1]=inputData.uChar[0];
                reFormatData.uChar[2]=inputData.uChar[3];
                reFormatData.uChar[3]=inputData.uChar[2];
                break;
            default:
            break;
        }
    }
    else if(analogFormat.compare(QStringLiteral("Little_Endian_Byte_Swap"))==0)
    {
        switch (bytes)
        {
            case 4:
                reFormatData.uChar[0]=inputData.uChar[2];
                reFormatData.uChar[1]=inputData.uChar[3];
                reFormatData.uChar[2]=inputData.uChar[0];
                reFormatData.uChar[3]=inputData.uChar[1];
                break;
            default:
            break;
        }
    }
    else
        reFormatData.uLongData=inputData.uLongData;

    pstr=QString("%1").arg(reFormatData.uLongData,2*bytes,16,QLatin1Char('0'));
    ui->analogRegistersEdit->setText(pstr);
}


void SettingsDialog::on_actualToHexConvertButton_clicked()              //convert from actual to Register
{
    union UanalogData
    {
    unsigned char byte[4];
    unsigned long int analogRegister;
    float analogFloat;
    }analogData;

    unsigned char uint8;
    unsigned int uint16;
    signed int int16;
    unsigned long uint32;
    signed long  int32;
    QString pstr;
    bool ok;

    QString analogType=ui->analogTypeBox->currentText();
    QString actualData=ui->analogActualValueEdit->text();
    QString registersData=ui->analogRegistersEdit->text();
    if(analogType.compare(QStringLiteral("UINT8"))==0)
    {
        uint16=actualData.toUShort(&ok);
        if(uint16<=255&&ok)
        {
         leftToRightFormatConvert(uint16,1);
        }
        else
        {
           ui->statusEdit->setText("The UINT8 value should be in the range of 0-255");
           ui->analogRegistersEdit->setText("Invalid input");
        }
    }

    else if(analogType.compare(QStringLiteral("INT8"))==0)
    {
        int16=actualData.toShort(&ok);
        //int8=((unsigned char)int16)&0xff;
        if(int16<0)
            {
            uint8=0xff-int16*(-1)+1;
            }
        else uint8=int16;

        if(int16>=-128&&int16<=127&&ok)
        {
        leftToRightFormatConvert(uint8,1);
        }
        else
        {
           ui->statusEdit->setText("The INT8 value should be the range of -128 to 127");
           ui->analogRegistersEdit->setText("Invalid input");
        }
    }
    if(analogType.compare(QStringLiteral("UINT16"))==0)
    {
        uint16=actualData.toUInt(&ok);
        if(uint16<=0xffff&&ok)
        {
        leftToRightFormatConvert(uint16,2);
        }
        else
        {
           ui->statusEdit->setText("The UINT16 value should be in the range of 0-65535");
           ui->analogRegistersEdit->setText("Invalid input");
        }
    }
    if(analogType.compare(QStringLiteral("INT16"))==0)
    {
        int32=actualData.toLong(&ok);
        if(int32<0)
            {
            uint32=(unsigned int)(0xffffffff+int32+1);
            }
        else uint32=int32;

        if(ok&&int32>=-32768&&int32<=32767)
        {
            leftToRightFormatConvert(uint32,2);
        }

        else
        {        
            ui->statusEdit->setText("The INT16 value should be in the range of -32768-32767");
            ui->analogRegistersEdit->setText("Invalid input");
        }        
    }
    if(analogType.compare(QStringLiteral("UINT32"))==0)
    {
        uint32=actualData.toULong(&ok);
        if(uint32<=0xffffffff&&ok)
        {
            leftToRightFormatConvert(uint32,4);
        }
        else
        {
               ui->statusEdit->setText("The UINT32 value should be in the range of 0-4294967295");
               ui->analogRegistersEdit->setText("Invalid input");
        }
    }
    if(analogType.compare(QStringLiteral("INT32"))==0)
    {
        int32=actualData.toLong(&ok);
        if(int32<0)
        {
            uint32=0xffffffff-int32*(-1)+1;
        }
       else uint32=int32;
       if(int32>=-2147483648&&int32<=0x7fffffff&&ok)
       {
           leftToRightFormatConvert(uint32,4);
       }
       else
       {
            ui->statusEdit->setText("The INT32 value should be in the range of -2147483648 to 2147483647");
            ui->analogRegistersEdit->setText("Invalid input");
       }
    }
    if(analogType.compare(QStringLiteral("FLOAT32"))==0)
    {
        analogData.analogFloat=actualData.toFloat(&ok);

       if(ok)
       {
            leftToRightFormatConvert(analogData.analogRegister,4);
       }
       else
       {
            ui->analogRegistersEdit->setText("Invalid input");
       }
    }
}

unsigned long SettingsDialog::ByteSwap(unsigned long oriData,int bytes)
{
    QString pstr;
    QString analogFormat=ui->analogFormatBox->currentText();

    union UreOrderData
    {
        unsigned char uChar[4];
        char sChar[4];
        unsigned short int  uint[2];
        int sint[2];
        unsigned long uLongData;
        long sLongData;
        float float32;
    };

    UreOrderData reFormatData;
    UreOrderData inputData;
    inputData.uLongData=oriData;
    if(bytes==2)inputData.uLongData&=0xffff;

    if(analogFormat.compare(QStringLiteral("Little_Endian"))==0)
    {
        switch (bytes)
        {
            case 2:
                reFormatData.uChar[0]=inputData.uChar[1];
                reFormatData.uChar[1]=inputData.uChar[0];
                break;
            case 4:
                reFormatData.uChar[0]=inputData.uChar[3];
                reFormatData.uChar[1]=inputData.uChar[2];
                reFormatData.uChar[2]=inputData.uChar[1];
                reFormatData.uChar[3]=inputData.uChar[0];
                break;
            default:
            break;
        }
    }
    else if(analogFormat.compare(QStringLiteral("Big_Endian_Byte_Swap"))==0)
    {
        switch (bytes)
        {
            case 4:
                reFormatData.uChar[0]=inputData.uChar[1];
                reFormatData.uChar[1]=inputData.uChar[0];
                reFormatData.uChar[2]=inputData.uChar[3];
                reFormatData.uChar[3]=inputData.uChar[2];
                break;
            default:
            break;
        }
    }
    else if(analogFormat.compare(QStringLiteral("Little_Endian_Byte_Swap"))==0)
    {
        switch (bytes)
        {
            case 4:
                reFormatData.uChar[0]=inputData.uChar[2];
                reFormatData.uChar[1]=inputData.uChar[3];
                reFormatData.uChar[2]=inputData.uChar[0];
                reFormatData.uChar[3]=inputData.uChar[1];
                break;
            default:
            break;
        }
    }
    else
        reFormatData.uLongData=inputData.uLongData;

    return reFormatData.uLongData;
}
void SettingsDialog::on_hexToActualConvertButton_clicked()
{
    unsigned long int uint32_1,uint32;
    long int int32_1,int32;
    bool minus_flag;
    union UreOrderData
    {
        unsigned char uChar[4];
        char sChar[4];
        unsigned short int  uint[2];
        int sint[2];
        unsigned long uLongData;
        long sLongData;
        float float32;
    };

    UreOrderData reFormatData;
    UreOrderData inputData;
    bool ok;
    int factor;

    QString analogType=ui->analogTypeBox->currentText();
    QString analogFormat=ui->analogFormatBox->currentText();
    QString actualData=ui->analogActualValueEdit->text();
    QString registersData=ui->analogRegistersEdit->text();

    if(analogType.compare(QStringLiteral("UINT8"))==0||analogType.compare(QStringLiteral("UINT16"))==0||analogType.compare(QStringLiteral("UINT32"))==0)
    {
        uint32_1=registersData.toULong(&ok,16);
        if(analogType.compare(QStringLiteral("UINT16"))==0)uint32=ByteSwap(uint32_1,2);
        else if(analogType.compare(QStringLiteral("UINT32"))==0)uint32=ByteSwap(uint32_1,4);
        else uint32=uint32_1;
        actualData = QString("%1").arg(uint32);
        if (!ok)
        {
            ui->analogActualValueEdit->setText("Invalid");
            ui->statusEdit->setText("Please input valid Hexadecimal.");
        }
        else
        ui->analogActualValueEdit->setText(actualData);
    }
    if(analogType.compare(QStringLiteral("INT8"))==0||analogType.compare(QStringLiteral("INT16"))==0||analogType.compare(QStringLiteral("INT32"))==0)
    {
        if(analogType.compare(QStringLiteral("INT8"))==0)factor=0x7f;
        if(analogType.compare(QStringLiteral("INT16"))==0)factor=0x7fff;
        if(analogType.compare(QStringLiteral("INT32"))==0)factor=0x7fffffff;
        int32_1=registersData.toULong(&ok,16);

        if(analogType.compare(QStringLiteral("INT16"))==0)int32=ByteSwap(int32_1,2);
        else if(analogType.compare(QStringLiteral("INT32"))==0)int32=ByteSwap(int32_1,4);
        else int32=int32_1;
        minus_flag=false;

        if((analogType.compare(QStringLiteral("INT8"))==0)&&(!(registersData.at(0)>='0'&&registersData.at(0)<='7')))
        {
            minus_flag=true;
        }
        if((analogType.compare(QStringLiteral("INT16"))==0)&&(analogFormat.compare(QStringLiteral("Big_Endian"))==0)&&(!(registersData.at(0)>='0'&&registersData.at(0)<='7')))
        {
            minus_flag=true;
        }
        if((analogType.compare(QStringLiteral("INT16"))==0)&&(analogFormat.compare(QStringLiteral("Little_Endian"))==0)&&(!(registersData.at(2)>='0'&&registersData.at(2)<='7')))
        {
            minus_flag=true;
        }
        if((analogType.compare(QStringLiteral("INT32"))==0)&&(analogFormat.compare(QStringLiteral("Big_Endian"))==0)&&(!(registersData.at(0)>='0'&&registersData.at(0)<='7')))
        {
            minus_flag=true;
        }
        if((analogType.compare(QStringLiteral("INT32"))==0)&&(analogFormat.compare(QStringLiteral("Little_Endian"))==0)&&(!(registersData.at(6)>='0'&&registersData.at(6)<='7')))
        {
            minus_flag=true;
        }
        if((analogType.compare(QStringLiteral("INT32"))==0)&&(analogFormat.compare(QStringLiteral("Big_Endian_Byte_Swap"))==0)&&(!(registersData.at(2)>='0'&&registersData.at(2)<='7')))
        {
            minus_flag=true;
        }
        if((analogType.compare(QStringLiteral("INT32"))==0)&&(analogFormat.compare(QStringLiteral("Little_Endian_Byte_Swap"))==0)&&(!(registersData.at(4)>='0'&&registersData.at(4)<='7')))
        {
            minus_flag=true;
        }

        if(minus_flag==true)        //非正数
        {
          int32 = int32 & factor;   //清除符号位
          int32 = ~int32;           //反码
          int32 &= factor;          //清除左边多余位
          int32+=1;                 //加1
          int32 *= -1;                //符号位
        }

        actualData = QString("%1").arg(int32);
        if (!ok)
        {
            ui->analogActualValueEdit->setText("Invalid");
            ui->statusEdit->setText("Please input valid Hexadecimal.");
        }
        else
        ui->analogActualValueEdit->setText(actualData);
    }

    if(analogType.compare(QStringLiteral("FLOAT32"))==0)
    {  
        inputData.uLongData=registersData.toULong(&ok,16);
        reFormatData.uLongData=ByteSwap(inputData.uLongData,4);
        actualData = QString("%1").arg(reFormatData.float32);
        if (!ok)
        {
            ui->analogActualValueEdit->setText("Invalid");
            ui->statusEdit->setText("Please input valid Hexadecimal.");
        }
        else
        ui->analogActualValueEdit->setText(actualData);
    }
}


void SettingsDialog::on_analogTypeBox_currentIndexChanged(const QString &arg1)
{
    if(arg1.compare(QStringLiteral("UINT8"))==0||arg1.compare(QStringLiteral("INT8"))==0)ui->analogFormatBox->setEnabled(false);
    else if(arg1.compare(QStringLiteral("UINT16"))==0||arg1.compare(QStringLiteral("INT16"))==0)
    {
        ui->analogFormatBox->setEnabled(true);
        QVariant v(0);
        ui->analogFormatBox->setItemData(2, v, Qt::UserRole - 1);
        ui->analogFormatBox->setItemData(3, v, Qt::UserRole - 1);
    }
    else
    {
        ui->analogFormatBox->setEnabled(true);
        QVariant v(1|32);
        ui->analogFormatBox->setItemData(2, v, Qt::UserRole - 1);
        ui->analogFormatBox->setItemData(3, v, Qt::UserRole - 1);
    }
    if(arg1.compare(QStringLiteral("UINT8"))==0||arg1.compare(QStringLiteral("INT8"))==0)
    {
        ui->analogRegistersEdit->setMaxLength(2);
        ui->analogRegistersEdit->setText("00");
    }
    else if(arg1.compare(QStringLiteral("UINT16"))==0||arg1.compare(QStringLiteral("INT16"))==0)
    {
        ui->analogRegistersEdit->setMaxLength(4);
        ui->analogRegistersEdit->setText("0000");
    }
    else if(arg1.compare(QStringLiteral("UINT32"))==0||arg1.compare(QStringLiteral("INT32"))==0||arg1.compare(QStringLiteral("FLOAT32"))==0)
    {
        ui->analogRegistersEdit->setMaxLength(8);
        ui->analogRegistersEdit->setText("00000000");
    }
    else
    {
        ui->analogRegistersEdit->setMaxLength(16);
        ui->analogRegistersEdit->setText("");
    }
}

void SettingsDialog::on_diCheck_0_stateChanged(int arg1)
{
    if(arg1)ui->diCheck_0->setText("ON");
    if(!arg1)ui->diCheck_0->setText("OFF");
}

void SettingsDialog::on_diCheck_1_stateChanged(int arg1)
{
    if(arg1)ui->diCheck_1->setText("ON");
    if(!arg1)ui->diCheck_1->setText("OFF");
}

void SettingsDialog::on_diCheck_2_stateChanged(int arg1)
{
    if(arg1)ui->diCheck_2->setText("ON");
    if(!arg1)ui->diCheck_2->setText("OFF");
}

void SettingsDialog::on_diCheck_3_stateChanged(int arg1)
{
    if(arg1)ui->diCheck_3->setText("ON");
    if(!arg1)ui->diCheck_3->setText("OFF");
}

void SettingsDialog::on_diCheck_4_stateChanged(int arg1)
{
    if(arg1)ui->diCheck_4->setText("ON");
    if(!arg1)ui->diCheck_4->setText("OFF");
}

void SettingsDialog::on_diCheck_5_stateChanged(int arg1)
{
    if(arg1)ui->diCheck_5->setText("ON");
    if(!arg1)ui->diCheck_5->setText("OFF");
}

void SettingsDialog::on_diCheck_6_stateChanged(int arg1)
{
    if(arg1)ui->diCheck_6->setText("ON");
    if(!arg1)ui->diCheck_6->setText("OFF");
}

void SettingsDialog::on_diCheck_7_stateChanged(int arg1)
{
    if(arg1)ui->diCheck_7->setText("ON");
    if(!arg1)ui->diCheck_7->setText("OFF");
}

void SettingsDialog::on_diCheck_8_stateChanged(int arg1)
{
    if(arg1)ui->diCheck_8->setText("ON");
    if(!arg1)ui->diCheck_8->setText("OFF");
}

void SettingsDialog::on_diCheck_9_stateChanged(int arg1)
{
    if(arg1)ui->diCheck_9->setText("ON");
    if(!arg1)ui->diCheck_9->setText("OFF");
}

void SettingsDialog::on_diCheck_10_stateChanged(int arg1)
{
    if(arg1)ui->diCheck_10->setText("ON");
    if(!arg1)ui->diCheck_10->setText("OFF");
}

void SettingsDialog::on_diCheck_11_stateChanged(int arg1)
{
    if(arg1)ui->diCheck_11->setText("ON");
    if(!arg1)ui->diCheck_11->setText("OFF");
}

void SettingsDialog::on_diCheck_12_stateChanged(int arg1)
{
    if(arg1)ui->diCheck_12->setText("ON");
    if(!arg1)ui->diCheck_12->setText("OFF");
}

void SettingsDialog::on_diCheck_13_stateChanged(int arg1)
{
    if(arg1)ui->diCheck_13->setText("ON");
    if(!arg1)ui->diCheck_13->setText("OFF");
}

void SettingsDialog::on_diCheck_14_stateChanged(int arg1)
{
    if(arg1)ui->diCheck_14->setText("ON");
    if(!arg1)ui->diCheck_14->setText("OFF");
}

void SettingsDialog::on_diCheck_15_stateChanged(int arg1)
{
    if(arg1)ui->diCheck_15->setText("ON");
    if(!arg1)ui->diCheck_15->setText("OFF");
}

void SettingsDialog::on_coilCheck_0_stateChanged(int arg1)
{
    if(arg1)ui->coilCheck_0->setText("ON");
    if(!arg1)ui->coilCheck_0->setText("OFF");
}

void SettingsDialog::on_coilCheck_1_stateChanged(int arg1)
{
    if(arg1)ui->coilCheck_1->setText("ON");
    if(!arg1)ui->coilCheck_1->setText("OFF");
}

void SettingsDialog::on_coilCheck_2_stateChanged(int arg1)
{
    if(arg1)ui->coilCheck_2->setText("ON");
    if(!arg1)ui->coilCheck_2->setText("OFF");
}

void SettingsDialog::on_coilCheck_3_stateChanged(int arg1)
{
    if(arg1)ui->coilCheck_3->setText("ON");
    if(!arg1)ui->coilCheck_3->setText("OFF");
}

void SettingsDialog::on_coilCheck_4_stateChanged(int arg1)
{
    if(arg1)ui->coilCheck_4->setText("ON");
    if(!arg1)ui->coilCheck_4->setText("OFF");
}

void SettingsDialog::on_coilCheck_5_stateChanged(int arg1)
{
    if(arg1)ui->coilCheck_5->setText("ON");
    if(!arg1)ui->coilCheck_5->setText("OFF");
}

void SettingsDialog::on_coilCheck_6_stateChanged(int arg1)
{
    if(arg1)ui->coilCheck_6->setText("ON");
    if(!arg1)ui->coilCheck_6->setText("OFF");
}

void SettingsDialog::on_coilCheck_7_stateChanged(int arg1)
{
    if(arg1)ui->coilCheck_7->setText("ON");
    if(!arg1)ui->coilCheck_7->setText("OFF");
}

void SettingsDialog::on_coilCheck_8_stateChanged(int arg1)
{
    if(arg1)ui->coilCheck_8->setText("ON");
    if(!arg1)ui->coilCheck_8->setText("OFF");
}

void SettingsDialog::on_coilCheck_9_stateChanged(int arg1)
{
    if(arg1)ui->coilCheck_9->setText("ON");
    if(!arg1)ui->coilCheck_9->setText("OFF");
}

void SettingsDialog::on_coilCheck_10_stateChanged(int arg1)
{
    if(arg1)ui->coilCheck_10->setText("ON");
    if(!arg1)ui->coilCheck_10->setText("OFF");
}

void SettingsDialog::on_coilCheck_11_stateChanged(int arg1)
{
    if(arg1)ui->coilCheck_11->setText("ON");
    if(!arg1)ui->coilCheck_11->setText("OFF");
}

void SettingsDialog::on_coilCheck_12_stateChanged(int arg1)
{
    if(arg1)ui->coilCheck_12->setText("ON");
    if(!arg1)ui->coilCheck_12->setText("OFF");
}

void SettingsDialog::on_coilCheck_13_stateChanged(int arg1)
{
    if(arg1)ui->coilCheck_13->setText("ON");
    if(!arg1)ui->coilCheck_13->setText("OFF");
}

void SettingsDialog::on_coilCheck_14_stateChanged(int arg1)
{
    if(arg1)ui->coilCheck_14->setText("ON");
    if(!arg1)ui->coilCheck_14->setText("OFF");
}

void SettingsDialog::on_coilCheck_15_stateChanged(int arg1)
{
    if(arg1)ui->coilCheck_15->setText("ON");
    if(!arg1)ui->coilCheck_15->setText("OFF");
}

unsigned exceptionCode[]={
MODBUS_ILLEGAL_DATA_FUNCTION,
MODBUS_ILLEGAL_DATA_ADDRESS,
MODBUS_ILLEGAL_DATA_VALUE,
MODBUS_SLAVE_DEVICE_FAIL,
MODBUS_ACKNOWLEDGE,
MODBUS_SLAVE_DEVICE_BUSY,
MODBUS_NEGATIVE_ACKNOWLEDGE,
MODBUS_MEMORY_PARITY_ERROR,
MODBUS_RTU_UNAVAILABLE,
MODBUS_TARGET_DEVICE_FAIL
};

void SettingsDialog::on_ExceptionCodecomboBox_currentIndexChanged(int index)
{
    currentSettings.smodbus.exceptionCode=exceptionCode[index];
}

void SettingsDialog::on_setExceptionCode_stateChanged(int arg1)
{

    if(arg1)
    {
        currentSettings.smodbus.exceptionMode=true;
        currentSettings.smodbus.exceptionCode=exceptionCode[ui->ExceptionCodecomboBox->currentIndex()];
    }
    else currentSettings.smodbus.exceptionMode=false;

}
