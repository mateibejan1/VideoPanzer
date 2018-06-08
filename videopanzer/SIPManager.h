#ifndef SIP_MANAGER_H
#define SIP_MANAGER_H

#include <pjsua2.hpp>
#include <QObject>
#include <QHash>

extern "C" {
#include <pjlib.h>
#include <pjlib-util.h>
#include <pjmedia.h>
#include <pjmedia-codec.h>
#include <pjsip.h>
#include <pjsip_simple.h>
#include <pjsip_ua.h>
#include <pjsua-lib/pjsua.h>
}

class SIPAccount;
class SIPCall;

class SIPManager : public QObject
{
    Q_OBJECT

public:
    explicit SIPManager(pjsip_transport_type_e transportType, quint16 port, QObject* parent = nullptr);
    virtual ~SIPManager() override;

    pjsua_acc_id createAccount(const QString& idUri, const QString& registrarUri, const QString& user, const QString& password);
    void registerAccount(pjsua_acc_id accountId);
    void unregisterAccount(pjsua_acc_id accountId);

    void makeCall(pjsua_acc_id accountId, const QString& number);
    void acceptCall(pjsua_call_id callId);
    void hangupCall(pjsua_call_id callId);
    void ring(pjsua_acc_id accountId, pjsua_call_id callId);

    void onRegStateStarted(bool status);
    void onRegStateChanged(bool status);
    void onCallStateChanged(const pj::CallInfo& callInfo);
    void onCallMediaStateChanged(const pj::CallInfo& callInfo);

signals:
    void regStateStarted(bool status);
    void regStateChanged(bool status);
    void callStateChanged(const pj::CallInfo& callInfo);
    void callMediaStateChanged(const pj::CallInfo& callInfo);

private:
    pj::Endpoint m_SIPEndpoint;
};

#endif // SIP_MANAGER_H