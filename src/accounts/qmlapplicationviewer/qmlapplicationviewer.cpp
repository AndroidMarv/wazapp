/*
  This file was generated by the Qt Quick Application wizard of Qt Creator.
  QmlApplicationViewer is a convenience class containing mobile device specific
  code such as screen orientation handling. Also QML paths and debugging are
  handled here.
  It is recommended not to modify this file, since newer versions of Qt Creator
  may offer an updated version of it.
*/

/***************************************************************************
**
** Copyright (c) 2012, Tarek Galal <tarek@wazapp.im>
**
** This file is part of Wazapp, an IM application for Meego Harmattan
** platform that allows communication with Whatsapp users.
**
** Wazapp is free software: you can redistribute it and/or modify it under
** the terms of the GNU General Public License as published by the
** Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Wazapp is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Wazapp. If not, see http://www.gnu.org/licenses/.
**
****************************************************************************/

#include "qmlapplicationviewer.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtGui/QApplication>
#include <QtDeclarative/QDeclarativeComponent>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeContext>
#include <QTimer>
#include <QThread>

#include "utilities.h";
using namespace WA_UTILITIES::Utilities;

#include <qplatformdefs.h> // MEEGO_EDITION_HARMATTAN

#ifdef HARMATTAN_BOOSTER
#include <MDeclarativeCache>
#endif

#if defined(QMLJSDEBUGGER) && QT_VERSION < 0x040800

#include <qt_private/qdeclarativedebughelper_p.h>

#if !defined(NO_JSDEBUGGER)
#include <jsdebuggeragent.h>
#endif
#if !defined(NO_QMLOBSERVER)
#include <qdeclarativeviewobserver.h>
#endif


// Enable debugging before any QDeclarativeEngine is created
struct QmlJsDebuggingEnabler
{
    QmlJsDebuggingEnabler()
    {
        QDeclarativeDebugHelper::enableDebugging();
    }
};

// Execute code in constructor before first QDeclarativeEngine is instantiated
static QmlJsDebuggingEnabler enableDebuggingHelper;

#endif // QMLJSDEBUGGER

class QmlApplicationViewerPrivate
{
    QString mainQmlFile;
    friend class QmlApplicationViewer;
    static QString adjustPath(const QString &path);
};

QString QmlApplicationViewerPrivate::adjustPath(const QString &path)
{
#ifdef Q_OS_UNIX
#ifdef Q_OS_MAC
    if (!QDir::isAbsolutePath(path))
        return QString::fromLatin1("%1/../Resources/%2")
                .arg(QCoreApplication::applicationDirPath(), path);
#else
    const QString pathInInstallDir =
            QString::fromLatin1("%1/../%2").arg(QCoreApplication::applicationDirPath(), path);
    if (QFileInfo(pathInInstallDir).exists())
        return pathInInstallDir;
#endif
#endif
    return path;
}

QmlApplicationViewer::QmlApplicationViewer(QWidget *parent)
    : QDeclarativeView(parent)
    , d(new QmlApplicationViewerPrivate())
{
      debug_data = new QString();
    connect(engine(), SIGNAL(quit()), SLOT(close()));
    setResizeMode(QDeclarativeView::SizeRootObjectToView);
    // Qt versions prior to 4.8.0 don't have QML/JS debugging services built in
#if defined(QMLJSDEBUGGER) && QT_VERSION < 0x040800
#if !defined(NO_JSDEBUGGER)
    new QmlJSDebugger::JSDebuggerAgent(engine());
#endif
#if !defined(NO_QMLOBSERVER)
    new QmlJSDebugger::QDeclarativeViewObserver(this, this);
#endif
#endif

        handler = new SmsHandler();
        handler->moveToThread(&handlerThread);
        handler->connect(&handlerThread,SIGNAL(started()),handler,SLOT(run()));

        handlerThread.start();
}

QmlApplicationViewer::~QmlApplicationViewer()
{
    delete d;
}

QmlApplicationViewer *QmlApplicationViewer::create()
{
#ifdef HARMATTAN_BOOSTER
    return new QmlApplicationViewer(MDeclarativeCache::qDeclarativeView(), 0);
#else
    return new QmlApplicationViewer();
#endif
}

void QmlApplicationViewer::setMainQmlFile(const QString &file)
{
     this->rootContext()->setContextProperty("actor",this);
    d->mainQmlFile = QmlApplicationViewerPrivate::adjustPath(file);
    setSource(QUrl::fromLocalFile(d->mainQmlFile));


}


void QmlApplicationViewer::addImportPath(const QString &path)
{
    engine()->addImportPath(QmlApplicationViewerPrivate::adjustPath(path));


}

void QmlApplicationViewer::setOrientation(ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
    // If the version of Qt on the device is < 4.7.2, that attribute won't work
    if (orientation != ScreenOrientationAuto) {
        const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
        if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
            qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
            return;
        }
    }
#endif // Q_OS_SYMBIAN

    Qt::WidgetAttribute attribute;
    switch (orientation) {
#if QT_VERSION < 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    case ScreenOrientationLockPortrait:
        attribute = static_cast<Qt::WidgetAttribute>(128);
        break;
    case ScreenOrientationLockLandscape:
        attribute = static_cast<Qt::WidgetAttribute>(129);
        break;
    default:
    case ScreenOrientationAuto:
        attribute = static_cast<Qt::WidgetAttribute>(130);
        break;
#else // QT_VERSION < 0x040702
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_LockPortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_LockLandscapeOrientation;
        break;
    default:
    case ScreenOrientationAuto:
        attribute = Qt::WA_AutoOrientation;
        break;
#endif // QT_VERSION < 0x040702
    };
    setAttribute(attribute, true);
}

void QmlApplicationViewer::showExpanded()
{
#if defined(Q_OS_SYMBIAN) || defined(MEEGO_EDITION_HARMATTAN) || defined(Q_WS_SIMULATOR)
    showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
}

QApplication *createApplication(int &argc, char **argv)
{
#ifdef HARMATTAN_BOOSTER
    return MDeclarativeCache::qApplication(argc, argv);
#else
    return new QApplication(argc, argv);
#endif
}








/************************ My Additions *****************************/

void QmlApplicationViewer::onFail(QString reason)
{
    //@@TODO transform into user friendly message

    QString friendlyReason;


    if(reason == "fail-too-recent")
        friendlyReason = "Too recent tries. Please try again later";
    else
        friendlyReason = "Failed. Server said: "+reason;


    emit registrationFailed(friendlyReason);
}


void QmlApplicationViewer::voiceCodeRequest(QVariant cc, QVariant phoneNumber){

    debug("CODE INIT "+cc.toString()+phoneNumber.toString());

    curr_num = phoneNumber.toString();
    curr_cc = cc.toString();
    curr_cc = curr_cc.replace("+","");

    this->cc = curr_cc;
    this->phoneNumber = curr_num;



    WACodeRequest *wcr;
    wcr = new WACodeRequest(curr_cc,phoneNumber.toString(),"voice");
    connect(wcr,SIGNAL(success()),this,SIGNAL(voiceCodeRequested()));
    connect(wcr,SIGNAL(fail(QString)),this,SLOT(onFail(QString)));
    connect(this,SIGNAL(codeRequestCancelled()),wcr,SLOT(quit()));



    wcr->start();


    //QTimer::singleShot(2000,handler,SLOT(processIncomingSMS()));
    //emit statusUpdated(QVariant("reg_b"));
   // emit statusUpdated(QVariant("reg_c"));
    //emit statusUpdated(QVariant("reg_d"));
}

void QmlApplicationViewer::regRequest(QVariant code){


    debug("REGCODE INIT "+this->cc+this->phoneNumber);

    WARegRequest *wrr;
    wrr = new WARegRequest(this->cc,this->phoneNumber);
    connect(wrr,SIGNAL(success(QString)),this,SLOT(onSuccess(QString)));
    connect(wrr,SIGNAL(fail(QString)),this,SLOT(onFail(QString)));
    connect(this,SIGNAL(codeRequestCancelled()),wrr,SLOT(quit()));

    wrr->go(code.toString());

    //QTimer::singleShot(2000,handler,SLOT(processIncomingSMS()));
    //emit statusUpdated(QVariant("reg_b"));
   // emit statusUpdated(QVariant("reg_c"));
    //emit statusUpdated(QVariant("reg_d"));
}

void QmlApplicationViewer::registerAccount(QVariant cc, QVariant phoneNumber)
{
    emit statusUpdated(QVariant("reg_a"));
    isActive = true;

    debug("REG INIT "+cc.toString()+phoneNumber.toString());

    curr_num = phoneNumber.toString();
    curr_cc = cc.toString();
    curr_cc = curr_cc.replace("+","");

    this->cc = curr_cc;
    this->phoneNumber = curr_num;

    connect(handler,SIGNAL(gotCode(QString)),this,SLOT(codeReceived(QString)));
    connect(this,SIGNAL(codeRequestCancelled()),handler,SLOT(stopListener()));


    WACodeRequest *wcr;
    wcr = new WACodeRequest(curr_cc,phoneNumber.toString());
    connect(wcr,SIGNAL(success()),this,SLOT(codeRequested()));
    connect(wcr,SIGNAL(success(QString)), this, SLOT(codeReceived(QString)));
    connect(wcr,SIGNAL(fail(QString)),this,SLOT(onFail(QString)));
    connect(this,SIGNAL(codeRequestCancelled()),wcr,SLOT(quit()));

    //connect(handler,SIGNAL(initialized()),wcr,SLOT(start()));

    WARegRequest *wrr;
    wrr = new WARegRequest(curr_cc,phoneNumber.toString());
    connect (handler,SIGNAL(gotCode(QString)),wrr,SLOT(go(QString)));
    connect(wcr,SIGNAL(success(QString)), wrr, SLOT(go(QString)));
    connect(wrr,SIGNAL(success(QString)),this,SLOT(onSuccess(QString)));
    connect(wrr,SIGNAL(fail(QString)),this,SLOT(onFail(QString)));
    connect(this,SIGNAL(codeRequestCancelled()),wcr,SLOT(quit()));


    /*if(!handler->managerStarted){
        connect(handler,SIGNAL(managerCreated()),handler,SLOT(initManager()));
    }
    else{
        handler->initManager();
    }*/

    wcr->start();

    //QTimer::singleShot(2000,handler,SLOT(processIncomingSMS()));
    //emit statusUpdated(QVariant("reg_b"));
   // emit statusUpdated(QVariant("reg_c"));
    //emit statusUpdated(QVariant("reg_d"));
}


void QmlApplicationViewer::abraKadabra(){
    this->regRequest("911");
}

void QmlApplicationViewer::onSuccess(QString userId)
{
     emit statusUpdated(QVariant("reg_e"));
    debug("SUCCESSSSSSSSSSSSSSSSSS");

    createAccount(userId);
}

void QmlApplicationViewer::codeReceived(QString code)
{
    debug("GOT THE CODE "+code);
    emit statusUpdated(QVariant("reg_d"));

}

void QmlApplicationViewer::codeRequested()
{
    debug("CODE REQUESTED");
    emit statusUpdated(QVariant("reg_b"));

}

void QmlApplicationViewer::savePushName(QVariant pushName){
    this->account->setValue("pushName",pushName.toString());
    this->account->sync();
}

QString QmlApplicationViewer::getCc()
{
    return "+20";

    /*
    QString code = Utilities::getCountryCode();

    if(code.length() == 3){
        code = code.mid(1);
    }

    code = "+"+code;

    return code;*/
}

void QmlApplicationViewer::init(int initType)
{
    manager = new Accounts::Manager(QString("IM"));
    Accounts::AccountIdList accl = manager->accountList();
    qDebug()<<accl.length();

    switch(initType)
    {
        case 1:
            //createAccount("12345678","20");
        break;
         case 2:
        debug("aho->");
            QString imsi = account->valueAsString("imsi");
            debug(imsi);

        break;

    }

}

void QmlApplicationViewer::stopCodeRequest(){
    qDebug()<<"STOP CODE REQUEST INIT";

    emit codeRequestCancelled();
    handler->isActive = false;
    qDebug()<<"DELETING SMS HANDLER";
    isActive = false;
     //delete handler;
}

void QmlApplicationViewer::debugAccount(Accounts::AccountId id){

    qDebug()<<"START DEBUG ACCT";
    Accounts::Account *a;
    a = manager->account(id);

    qDebug()<<a->displayName();
    qDebug()<<a->valueAsString("username");

   qDebug()<<"END DEBUG ACCT";

}


void QmlApplicationViewer::deleteAccount()
{
    if(account != NULL)
    {
        account->remove();
        account->sync();
        exit(0);
    }
}

void QmlApplicationViewer::createAccount(QString phoneNumber)
{
    qDebug()<<"SHOULD CREATE"<<endl;

    connect(manager,
            SIGNAL(accountCreated(Accounts::AccountId)),
            SLOT(onAccountCreated(Accounts::AccountId)));

    userId = phoneNumber;

    account = manager->createAccount(QString("waxmpp"));
    account->sync();
}

bool QmlApplicationViewer::isNew()
{
    //get IMSI
    //search for accounts having same IMSI
     QString imsi = Utilities::getImsi();
}

void QmlApplicationViewer::onAccountCreated(Accounts::AccountId id)
{
    qDebug()<<"ACCOUNT CREATED";

    account->setValue("name", this->cc+this->phoneNumber);
    account->setValue("username", userId);
    account->setValue("password", Utilities::getChatPassword());
    account->setValue("status", "Hi there I'm using Wazapp");
    account->setValue("imsi", Utilities::getImsi());
    account->setValue("cc", this->cc);
    account->setValue("phoneNumber", this->phoneNumber);
    account->setValue("pushName", this->cc+this->phoneNumber);
    account->setValue("jid", userId+"@s.whatsapp.net");
    account->setEnabled(true);
    account->sync();

    emit(registrationSuccess(userId));
}

void QmlApplicationViewer::debug(QString data)
{
    debug_data->append("::");
    debug_data->append(data);

    qDebug()<<data;

    QString tmp (debug_data->toAscii());
    this->rootContext()->setContextProperty("debug_data",tmp);

}
