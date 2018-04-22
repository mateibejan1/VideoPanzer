#include "SIPManager.h"
#include "SIPAccount.h"
#include "SIPCall.h"

#include <QDebug>

SIPManager::SIPManager(pjsip_transport_type_e transportType, quint16 port, QObject* parent) :
    QObject(parent)
{
    try {
        m_SIPEndpoint.libCreate();
        m_SIPEndpoint.libInit(m_SIPEndpointConfig);
        m_transportConfig.port = port;
        m_SIPEndpoint.transportCreate(transportType, m_transportConfig);
        m_SIPEndpoint.libStart();
        qDebug() << "PJLIB has started!";
        auto videoCodecParameter = m_SIPEndpoint.getVideoCodecParam("H264");
        videoCodecParameter.decFmt.width = 1280;
        videoCodecParameter.decFmt.height = 720;
        m_SIPEndpoint.setVideoCodecParam("H264", videoCodecParameter);
    } catch (pj::Error &error) {
        qDebug() << "PJLIB starting failed: " << error.info().c_str();
    }
}

SIPManager::~SIPManager()
{
    try {
        delete m_SIPAccount;
    } catch (pj::Error &error) {
        qDebug() << "PJLIB deleting failed: " << error.info().c_str();
    }
}

void SIPManager::createAccount(const QString &idUri, const QString &registrarUri, const QString &user, const QString &password)
{
    try {
        m_accountConfig.idUri = idUri.toStdString();
        m_accountConfig.regConfig.registrarUri = registrarUri.toStdString();
        pj::AuthCredInfo credentials("digest", "*", user.toStdString(), 0, password.toStdString());
        m_accountConfig.sipConfig.authCreds.push_back(credentials);
        m_accountConfig.callConfig.timerMinSESec = 90;
        m_accountConfig.callConfig.timerSessExpiresSec = 1800;
        m_accountConfig.videoConfig.autoTransmitOutgoing = true;
        m_SIPAccount = new SIPAccount(this);
        m_SIPAccount->create(m_accountConfig);
        qDebug() << "Account creation successful!";

    } catch(pj::Error& error) {
        qDebug() << "Account creation error: " << error.info().c_str();
    }
}

void SIPManager::registerAccount()
{
    try {
        m_SIPAccount->setRegistration(true);
        qDebug() << "Registered account successfully!";

    } catch(pj::Error& error) {
        qDebug() << "Register error: " << error.info().c_str();
    }
}

void SIPManager::unregisterAccount()
{
    try {
        m_SIPAccount->setRegistration(false);
        qDebug() << "Unregistered account successfully!";

    } catch(pj::Error& error) {
        qDebug() << "Unregister error: " << error.info().c_str();
    }
}

void SIPManager::makeCall(const QString &number)
{
    m_SIPCall = new SIPCall(this, *m_SIPAccount);
    pj::CallOpParam callOperationParameter(true);
    try {
        m_SIPCall->makeCall(number.toStdString(), callOperationParameter);
    } catch(pj::Error& error) {
        qDebug() << "Call could not be made: " << error.info().c_str();
    }
}

void SIPManager::acceptCall()
{
    try {
        m_SIPCall = new SIPCall(this, *m_SIPAccount, m_currentCallId);
        pj::CallOpParam callOperationParameter;
        callOperationParameter.statusCode = PJSIP_SC_OK;
        m_SIPCall->answer(callOperationParameter);
    } catch(pj::Error& error) {
        qDebug() << "Accepting failed: " << error.info().c_str();
    }
}

void SIPManager::hangupCall()
{
    try {
        pj::CallInfo callInfo = m_SIPCall->getInfo();
        pj::CallOpParam callOperationParameter;
        if(callInfo.lastStatusCode == PJSIP_SC_RINGING) {
            callOperationParameter.statusCode = PJSIP_SC_BUSY_HERE;
        }
        else {
            callOperationParameter.statusCode = PJSIP_SC_OK;
        }
        m_SIPCall->hangup(callOperationParameter);
    } catch(pj::Error& error) {
            qDebug() << "HangupCall failed" << error.info().c_str();
    }
}

void SIPManager::ring(pjsua_call_id callId)
{
    try {
        m_SIPCall = new SIPCall(this, *m_SIPAccount, callId);
        pj::CallOpParam callOperationParameter;
        callOperationParameter.statusCode = PJSIP_SC_RINGING;
        m_SIPCall->answer(callOperationParameter);
    } catch(pj::Error& error) {
        qDebug() << "Ringing failed: " << error.info().c_str();
    }
}

void SIPManager::emitRegStateStarted(bool status)
{
    emit regStateStarted(status);
}

void SIPManager::emitRegStateChanged(bool status)
{
    emit regStateChanged(status);
}

void SIPManager::onCallStateChanged(pj::CallInfo callInfo, const QString &remoteUri)
{
    if (callInfo.state == PJSIP_INV_STATE_DISCONNECTED) {
        qDebug() << "Deleting current call as result of disconnection!";
        delete m_SIPCall;
        m_SIPCall = nullptr;
    }
    m_currentCallId = callInfo.id;
    emit callStateChanged(callInfo, remoteUri);
}