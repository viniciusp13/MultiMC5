#include "AuthSession.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStringList>

namespace Minecraft
{
QString AuthSession::serializeUserProperties()
{
	QJsonObject userAttrs;
	for (auto key : u.properties.keys())
	{
		auto array = QJsonArray::fromStringList(u.properties.values(key));
		userAttrs.insert(key, array);
	}
	QJsonDocument value(userAttrs);
	return value.toJson(QJsonDocument::Compact);

}

bool AuthSession::setOfflineName(const QString& offline_name)
{
	if (status != PlayableOffline && status != PlayableOnline)
	{
		return false;
	}
	session = "-";
	player_name = offline_name;
	status = PlayableOffline;
	return true;
}
}
