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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtGlobal>

#include <QMainWindow>
#include <QBitArray>
#include <QtSerialPort/QSerialPort>
#include <QDateTime>
#define MAX_IO_BUFFER 10000
QT_BEGIN_NAMESPACE
#define SEND 1
#define RECEIVE 0
void put_into_buffer(QByteArray data,int length,int Rx_Tx);

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class SettingsDialog;
class SelProtocolDialog;
class IEC101_104;
class IEC103;
class CModbus;
class DNP3;
class MyThread;
class IEC_GOOSE;

struct myio_buffer
{
    bool valid_flag;
    int send_receive;
    int buffer_length;
    QDateTime datetime;
    QByteArray data;
};
//int current_ptr;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void about();
    void writeData(const QByteArray &data);
    void writeData_serial();
    void readData();
    void serialBuf_Process();
    void receiveData(myio_buffer &buffer);
    void check_io_buffer();
    void handleError(QSerialPort::SerialPortError error);

private:
    void initActionsConnections();

private:
    Ui::MainWindow *ui;
    SettingsDialog *settings;
    IEC101_104 *iec101_104;
    IEC103 *iec103;
    MyThread *myThread;
    DNP3 *dnp3;
    CModbus *modbus;
    IEC_GOOSE *iecgoose;
    SelProtocolDialog *selection;
    QSerialPort *serial;
    bool serialRxTimerFlag;
    int serialFrameInteval;
    QTimer *serialRxTimeout;
    QByteArray io_buffer[MAX_IO_BUFFER];
    int buffer_ptr,buffer_valid[MAX_IO_BUFFER];
};

#endif // MAINWINDOW_H
