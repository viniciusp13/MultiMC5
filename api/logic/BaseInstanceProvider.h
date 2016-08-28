#pragma once

#include "settings/SettingsObject.h"

class MULTIMC_LOGIC_EXPORT BaseInstanceProvider
{
public:
	virtual ~BaseInstanceProvider()
	{
	}
	virtual void initialize(SettingsObjectPtr globalSettings) = 0;
	virtual void loadInstances(SettingsObjectPtr globalSettings, QMap<QString, QString> &groupMap, QList<InstancePtr> &tempList) = 0;
};
