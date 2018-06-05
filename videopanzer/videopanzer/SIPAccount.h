#ifndef SIP_ACCOUNT_H
#define SIP_ACCOUNT_H

#include "SIPManager.h"
#include <pjsua2.hpp>

class SIPAccount : public pj::Account
{
public:
    explicit SIPAccount(SIPManager* _SIPManager);
    virtual ~SIPAccount() override;

    virtual void onRegState(pj::OnRegStateParam& registerStateParamater) override;
    virtual void onRegStarted(pj::OnRegStartedParam& registerStartedParameter) override;
    virtual void onIncomingCall(pj::OnIncomingCallParam& incomingCallParameter) override;

private:
    SIPManager* m_SIPManager = nullptr;
};

#endif // SIP_ACCOUNT_H
