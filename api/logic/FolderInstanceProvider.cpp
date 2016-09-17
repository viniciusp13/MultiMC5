#include "FolderInstanceProvider.h"
#include "FileSystem.h"
#include "settings/INISettingsObject.h"

#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

const static int GROUP_FILE_FORMAT_VERSION = 1;

void FolderInstanceProvider::saveGroupList()
{
	QString groupFileName = m_path + "/instgroups.json";
	QMap<QString, QSet<QString>> groupMap;
	for (auto instance : m_instances)
	{
		QString id = instance->id();
		QString group = instance->group();
		if (group.isEmpty())
			continue;

		// keep a list/set of groups for choosing
		m_groups.insert(group);

		if (!groupMap.count(group))
		{
			QSet<QString> set;
			set.insert(id);
			groupMap[group] = set;
		}
		else
		{
			QSet<QString> &set = groupMap[group];
			set.insert(id);
		}
	}
	QJsonObject toplevel;
	toplevel.insert("formatVersion", QJsonValue(QString("1")));
	QJsonObject groupsArr;
	for (auto iter = groupMap.begin(); iter != groupMap.end(); iter++)
	{
		auto list = iter.value();
		auto name = iter.key();
		QJsonObject groupObj;
		QJsonArray instanceArr;
		groupObj.insert("hidden", QJsonValue(QString("false")));
		for (auto item : list)
		{
			instanceArr.append(QJsonValue(item));
		}
		groupObj.insert("instances", instanceArr);
		groupsArr.insert(name, groupObj);
	}
	toplevel.insert("groups", groupsArr);
	QJsonDocument doc(toplevel);
	try
	{
		FS::write(groupFileName, doc.toJson());
	}
	catch(FS::FileSystemException & e)
	{
		qCritical() << "Failed to write instance group file :" << e.cause();
	}
}

void FolderInstanceProvider::loadGroupList(QMap<QString, QString> &groupMap)
{
	QString groupFileName = m_path + "/instgroups.json";

	// if there's no group file, fail
	if (!QFileInfo(groupFileName).exists())
		return;

	QByteArray jsonData;
	try
	{
		jsonData = FS::read(groupFileName);
	}
	catch (FS::FileSystemException & e)
	{
		qCritical() << "Failed to read instance group file :" << e.cause();
		return;
	}

	QJsonParseError error;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &error);

	// if the json was bad, fail
	if (error.error != QJsonParseError::NoError)
	{
		qCritical() << QString("Failed to parse instance group file: %1 at offset %2")
							.arg(error.errorString(), QString::number(error.offset))
							.toUtf8();
		return;
	}

	// if the root of the json wasn't an object, fail
	if (!jsonDoc.isObject())
	{
		qWarning() << "Invalid group file. Root entry should be an object.";
		return;
	}

	QJsonObject rootObj = jsonDoc.object();

	// Make sure the format version matches, otherwise fail.
	if (rootObj.value("formatVersion").toVariant().toInt() != GROUP_FILE_FORMAT_VERSION)
		return;

	// Get the groups. if it's not an object, fail
	if (!rootObj.value("groups").isObject())
	{
		qWarning() << "Invalid group list JSON: 'groups' should be an object.";
		return;
	}

	// Iterate through all the groups.
	QJsonObject groupMapping = rootObj.value("groups").toObject();
	for (QJsonObject::iterator iter = groupMapping.begin(); iter != groupMapping.end(); iter++)
	{
		QString groupName = iter.key();

		// If not an object, complain and skip to the next one.
		if (!iter.value().isObject())
		{
			qWarning() << QString("Group '%1' in the group list should "
								   "be an object.")
							   .arg(groupName)
							   .toUtf8();
			continue;
		}

		QJsonObject groupObj = iter.value().toObject();
		if (!groupObj.value("instances").isArray())
		{
			qWarning() << QString("Group '%1' in the group list is invalid. "
								   "It should contain an array "
								   "called 'instances'.")
							   .arg(groupName)
							   .toUtf8();
			continue;
		}

		// keep a list/set of groups for choosing
		m_groups.insert(groupName);

		// Iterate through the list of instances in the group.
		QJsonArray instancesArray = groupObj.value("instances").toArray();

		for (QJsonArray::iterator iter2 = instancesArray.begin(); iter2 != instancesArray.end();
			 iter2++)
		{
			groupMap[(*iter2).toString()] = groupName;
		}
	}
}


FolderInstanceProvider::FolderInstanceProvider(QString path)
{
	m_path = path;
	if (!QDir::current().exists(m_path))
	{
		QDir::current().mkpath(m_path);
	}
}

void FolderInstanceProvider::initialize(SettingsObjectPtr globalSettings)
{

}

InstanceList::InstLoadError FolderInstanceProvider::loadInstance(InstancePtr &inst, const QString &instDir)
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
	return InstanceList::NoLoadError;
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
