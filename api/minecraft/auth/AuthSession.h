#pragma once

#include <QString>
#include <QMultiMap>
#include <memory>
#include <auth/BaseAuthSession.h>

#include "multimc_minecraft_export.h"

namespace Minecraft
{
struct User
{
	QString id;
	QMultiMap<QString, QString> properties;
};

class MULTIMC_MINECRAFT_EXPORT AuthSession: public BaseAuthSession
{
public:
	AuthSession()
	{
	}
	virtual ~AuthSession()
	{
	}

	BaseAuthSession::Status getStatus() override
	{
		return status;
	}

	bool supportsLogin() override
	{
		return true;
	}
	bool loginRequired() override
	{
		return wants_online;
	}
	void setLoginRequired(bool required) override
	{
		wants_online = required;
	}

	bool supportsOfflineName() override
	{
		return true;
	}
	bool setOfflineName(const QString &offline_name) override;
	QString offlineName() override
	{
		if(status == PlayableOffline)
		{
			return player_name;
		}
		return QString();
	}

	QString userName() override
	{
		return username;
	}

	QString serializeUserProperties();

	Status status = Undetermined;

	User u;

	// client token
	QString client_token;
	// account user name
	QString username;
	// combined session ID
	QString session;
	// volatile auth token
	QString access_token;
	// profile name
	QString player_name;
	// profile ID
	QString uuid;
	// 'legacy' or 'mojang', depending on account type
	QString user_type;
	// Did the auth server reply?
	bool auth_server_online = false;
	// Did the user request online mode?
	bool wants_online = true;
};

typedef std::shared_ptr<AuthSession> AuthSessionPtr;
}
