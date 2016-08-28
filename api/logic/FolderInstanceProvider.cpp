#include "FolderInstanceProvider.h"
#include "FileSystem.h"
#include <QDirIterator>

FolderInstanceProvider::FolderInstanceProvider(QString path)
{
	m_path = path;
}

void FolderInstanceProvider::initialize(SettingsObjectPtr globalSettings)
{

}

InstanceList::InstLoadError InstanceList::loadInstance(InstancePtr &inst, const QString &instDir)
{
	auto instanceSettings = std::make_shared<INISettingsObject>(FS::PathCombine(instDir, "instance.cfg"));

	instanceSettings->registerSetting("InstanceType", "Legacy");

	QString inst_type = instanceSettings->get("InstanceType").toString();

	// FIXME: replace with a map lookup, where instance classes register their types
	if (inst_type == "OneSix" || inst_type == "Nostalgia")
	{
		inst.reset(new OneSixInstance(m_globalSettings, instanceSettings, instDir));
	}
	else if (inst_type == "Legacy")
	{
		inst.reset(new LegacyInstance(m_globalSettings, instanceSettings, instDir));
	}
	else
	{
		inst.reset(new NullInstance(m_globalSettings, instanceSettings, instDir));
	}
	inst->init();
	return NoLoadError;
}

void FolderInstanceProvider::loadInstances(SettingsObjectPtr globalSettings, QMap<QString, QString>& groupMap, QList<InstancePtr>& tempList)
{
	QDirIterator iter(m_path, QDir::Dirs | QDir::NoDot | QDir::NoDotDot | QDir::Readable,
						QDirIterator::FollowSymlinks);
	while (iter.hasNext())
	{
		QString subDir = iter.next();
		if (!QFileInfo(FS::PathCombine(subDir, "instance.cfg")).exists())
			continue;
		qDebug() << "Loading MultiMC instance from " << subDir;
		InstancePtr instPtr;
		auto error = loadInstance(instPtr, subDir);
		if(!InstanceList::continueProcessInstance(instPtr, error, subDir, groupMap))
			continue;
		tempList.append(instPtr);
	}
}
