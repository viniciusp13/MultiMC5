#pragma once

#include <BaseInstance.h>
#include <BaseInstanceProvider.h>

#include "multimc_minecraft_export.h"

// Pseudo-plugin for FTB related things. Super derpy!
class MULTIMC_MINECRAFT_EXPORT FTBInstanceProvider : public BaseInstanceProvider
{
public:
	FTBInstanceProvider();
	virtual ~FTBInstanceProvider()
	{
	}
	void initialize(SettingsObjectPtr globalSettings) override;
	void loadInstances(SettingsObjectPtr globalSettings, QMap<QString, QString> &groupMap, QList<InstancePtr> &tempList) override;
};
