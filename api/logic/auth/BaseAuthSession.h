#pragma once

#include <QString>
#include <memory>

#include "multimc_logic_export.h"

class MULTIMC_LOGIC_EXPORT BaseAuthSession
{
public:
	BaseAuthSession()
	{
	}
	virtual ~BaseAuthSession()
	{
	}

	virtual bool supportsLogin() = 0;
	virtual void setLoginRequired(bool required) = 0;
	virtual bool loginRequired() = 0;

	virtual bool supportsOfflineName() = 0;
	virtual bool setOfflineName(const QString &offline_name) = 0;
	virtual QString offlineName() = 0;

	virtual QString userName() = 0;

	enum Status
	{
		Undetermined,
		RequiresPassword,
		PlayableOffline,
		PlayableOnline
	};

	virtual Status getStatus() = 0;
};

typedef std::shared_ptr<BaseAuthSession> AuthSessionPtr;

