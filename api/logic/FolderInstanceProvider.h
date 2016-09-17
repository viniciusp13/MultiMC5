#pragma once

#include <BaseInstance.h>
#include "InstanceList.h"
#include <BaseInstanceProvider.h>

#include "multimc_logic_export.h"

class MULTIMC_LOGIC_EXPORT FolderInstanceProvider : public QObject, public BaseInstanceProvider
{
Q_OBJECT
public:
	FolderInstanceProvider(QString path);
	virtual ~FolderInstanceProvider()
	{
	}
	void initialize(SettingsObjectPtr globalSettings) override;
	void loadInstances(SettingsObjectPtr globalSettings, QMap<QString, QString> &groupMap, QList<InstancePtr> &tempList) override;
	InstanceList::InstLoadError loadInstance(InstancePtr &inst, const QString &instDir);

public slots:
	void on_InstFolderChanged(const Setting &setting, QVariant value);

private:
	void saveGroupList();
	void loadGroupList(QMap<QString, QString> &groupMap);

private:
	QString m_path;
	QList<InstancePtr> m_instances;
	QSet<QString> m_groups;
};
