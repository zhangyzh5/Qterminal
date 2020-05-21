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

#include <QApplication>
#include <QSplashScreen>
#include <QTextCodec>
#include <QElapsedTimer>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec *tc=QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(tc);
    //加载并显示启动画面
    QSplashScreen *splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/images/Splash.png"));
    splash->setDisabled(true); //禁用用户的输入事件响应
    splash->show();

    splash->showMessage(QObject::tr("Loading configuration...."),Qt::AlignLeft|Qt::AlignBottom,Qt::blue);
    //同时创建主视图对象
    QElapsedTimer t;
    t.start();
    while(t.elapsed()<1000)
        QCoreApplication::processEvents();MainWindow w;
    w.show();
    splash->finish(&w);
    delete splash;
    return a.exec();
}
