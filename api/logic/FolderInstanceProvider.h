#pragma once

#include <BaseInstance.h>
#include "InstanceList.h"
#include <BaseInstanceProvider.h>

#include "multimc_logic_export.h"

class MULTIMC_LOGIC_EXPORT FolderInstanceProvider : public BaseInstanceProvider
{
public:
	FolderInstanceProvider(QString path);
	virtual ~FolderInstanceProvider()
	{
	}
	void initialize(SettingsObjectPtr globalSettings) override;
	void loadInstances(SettingsObjectPtr globalSettings, QMap<QString, QString> &groupMap, QList<InstancePtr> &tempList) override;
	InstanceList::InstLoadError loadInstance(InstancePtr &inst, const QString &instDir);

private:
	QString m_path;
};

