/* Copyright 2013-2015 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <QDir>
#include <QSet>
#include <QFile>
#include <QDirIterator>
#include <QThread>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QXmlStreamReader>
#include <QRegularExpression>
#include <QDebug>

#include "InstanceList.h"
#include "BaseInstance.h"

//FIXME: this really doesn't belong *here*
/*
#include "onesix/OneSixInstance.h"
#include "legacy/LegacyInstance.h"
#include "ftb/FTBPlugin.h"
#include "MinecraftVersion.h"
*/
#include "settings/INISettingsObject.h"
#include "NullInstance.h"
#include "FileSystem.h"
#include "pathmatcher/RegexpMatcher.h"

InstanceList::InstanceList(SettingsObjectPtr globalSettings, QObject *parent)
	: QAbstractListModel(parent)
{
	m_globalSettings = globalSettings;
}

InstanceList::~InstanceList()
{
}

void InstanceList::onExit()
{
	// TODO: notify instance providers about exit (so groups can be saved, etc.)
}

int InstanceList::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_instances.count();
}

QModelIndex InstanceList::index(int row, int column, const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	if (row < 0 || row >= m_instances.size())
		return QModelIndex();
	return createIndex(row, column, (void *)m_instances.at(row).get());
}

QVariant InstanceList::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
	{
		return QVariant();
	}
	BaseInstance *pdata = static_cast<BaseInstance *>(index.internalPointer());
	switch (role)
	{
	case InstancePointerRole:
	{
		QVariant v = qVariantFromValue((void *)pdata);
		return v;
	}
	case InstanceIDRole:
    {
        return pdata->id();
    }
	case Qt::DisplayRole:
	{
		return pdata->name();
	}
	case Qt::ToolTipRole:
	{
		return pdata->instanceRoot();
	}
	case Qt::DecorationRole:
	{
		return pdata->iconKey();
	}
	// HACK: see GroupView.h in gui!
	case GroupRole:
	{
		return pdata->group();
	}
	default:
		break;
	}
	return QVariant();
}

Qt::ItemFlags InstanceList::flags(const QModelIndex &index) const
{
	Qt::ItemFlags f;
	if (index.isValid())
	{
		f |= (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	}
	return f;
}

void InstanceList::groupChanged()
{
	// save the groups. save all of them.
	// saveGroupList();
}

QStringList InstanceList::getGroups()
{
	return m_groups.toList();
}

void InstanceList::deleteGroup(const QString& name)
{
	for(auto & instance: m_instances)
	{
		auto instGroupName = instance->group();
		if(instGroupName == name)
		{
			instance->setGroupPost(QString());
		}
	}
}

InstanceList::InstListError InstanceList::loadList()
{
	// load the instance groups
	QMap<QString, QString> groupMap;
	// loadGroupList(groupMap);

	QList<InstancePtr> tempList;
	for(auto & provider: m_instanceProviders)
	{
		provider->loadInstances(m_globalSettings, groupMap, tempList);
	}

	beginResetModel();
	m_instances.clear();
	for(auto inst: tempList)
	{
		connect(inst.get(), SIGNAL(propertiesChanged(BaseInstance *)), this,
				SLOT(propertiesChanged(BaseInstance *)));
		connect(inst.get(), SIGNAL(groupChanged()), this, SLOT(groupChanged()));
		connect(inst.get(), SIGNAL(nuked(BaseInstance *)), this,
				SLOT(instanceNuked(BaseInstance *)));
		m_instances.append(inst);
	}
	endResetModel();
	emit dataIsInvalid();
	return NoError;
}

/// Clear all instances. Triggers notifications.
/*
void InstanceList::clear()
{
	beginResetModel();
	saveGroupList();
	m_instances.clear();
	endResetModel();
	emit dataIsInvalid();
}
*/

/// Add an instance. Triggers notifications, returns the new index
int InstanceList::add(InstancePtr t)
{
	beginInsertRows(QModelIndex(), m_instances.size(), m_instances.size());
	m_instances.append(t);
	t->setParent(this);
	connect(t.get(), SIGNAL(propertiesChanged(BaseInstance *)), this,
			SLOT(propertiesChanged(BaseInstance *)));
	connect(t.get(), SIGNAL(groupChanged()), this, SLOT(groupChanged()));
	connect(t.get(), SIGNAL(nuked(BaseInstance *)), this, SLOT(instanceNuked(BaseInstance *)));
	endInsertRows();
	return count() - 1;
}

bool InstanceList::addInstanceProvider(BaseInstanceProvider* provider)
{
	m_instanceProviders.append(std::shared_ptr<BaseInstanceProvider>(provider));
	return true;
}

InstancePtr InstanceList::getInstanceById(QString instId) const
{
	if(instId.isEmpty())
		return InstancePtr();
	for(auto & inst: m_instances)
	{
		if (inst->id() == instId)
		{
			return inst;
		}
	}
	return InstancePtr();
}

QModelIndex InstanceList::getInstanceIndexById(const QString &id) const
{
	return index(getInstIndex(getInstanceById(id).get()));
}

int InstanceList::getInstIndex(BaseInstance *inst) const
{
	int count = m_instances.count();
	for (int i = 0; i < count; i++)
	{
		if (inst == m_instances[i].get())
		{
			return i;
		}
	}
	return -1;
}

bool InstanceList::continueProcessInstance(InstancePtr instPtr, const int error,
										   const QDir &dir, QMap<QString, QString> &groupMap)
{
	if (error != InstanceList::NoLoadError && error != InstanceList::NotAnInstance)
	{
		QString errorMsg = QString("Failed to load instance %1: ")
							   .arg(QFileInfo(dir.absolutePath()).baseName())
							   .toUtf8();

		switch (error)
		{
		default:
			errorMsg += QString("Unknown instance loader error %1").arg(error);
			break;
		}
		qCritical() << errorMsg.toUtf8();
		return false;
	}
	else if (!instPtr)
	{
		qCritical() << QString("Error loading instance %1. Instance loader returned null.")
							.arg(QFileInfo(dir.absolutePath()).baseName())
							.toUtf8();
		return false;
	}
	else
	{
		auto iter = groupMap.find(instPtr->id());
		if (iter != groupMap.end())
		{
			instPtr->setGroupInitial((*iter));
		}
		qDebug() << "Loaded instance " << instPtr->name() << " from " << dir.absolutePath();
		return true;
	}
}

InstanceList::InstCreateError
InstanceList::createInstance(InstancePtr &inst, BaseVersionPtr version, const QString &instDir)
{
/*
	QDir rootDir(instDir);

	qDebug() << instDir.toUtf8();
	if (!rootDir.exists() && !rootDir.mkpath("."))
	{
		qCritical() << "Can't create instance folder" << instDir;
		return InstanceList::CantCreateDir;
	}

	if (!version)
	{
		qCritical() << "Can't create instance for non-existing MC version";
		return InstanceList::NoSuchVersion;
	}

	auto instanceSettings = std::make_shared<INISettingsObject>(FS::PathCombine(instDir, "instance.cfg"));
	instanceSettings->registerSetting("InstanceType", "Legacy");

	auto minecraftVersion = std::dynamic_pointer_cast<MinecraftVersion>(version);
	if(minecraftVersion)
	{
		instanceSettings->set("InstanceType", "OneSix");
		inst.reset(new OneSixInstance(m_globalSettings, instanceSettings, instDir));
		inst->setIntendedVersionId(version->descriptor());
		inst->init();
		return InstanceList::NoCreateError;
	}
	return InstanceList::NoSuchVersion;
*/
	return InstanceList::CantCreateDir;
}

InstanceList::InstCreateError
InstanceList::copyInstance(InstancePtr &newInstance, InstancePtr &oldInstance, const QString &instDir, bool copySaves)
{
	/*
	QDir rootDir(instDir);
	std::unique_ptr<IPathMatcher> matcher;
	if(!copySaves)
	{
		auto matcherReal = new RegexpMatcher("[.]?minecraft/saves");
		matcherReal->caseSensitive(false);
		matcher.reset(matcherReal);
	}

	qDebug() << instDir.toUtf8();
	FS::copy folderCopy(oldInstance->instanceRoot(), instDir);
	folderCopy.followSymlinks(false).blacklist(matcher.get());
	if (!folderCopy())
	{
		FS::deletePath(instDir);
		return InstanceList::CantCreateDir;
	}

	INISettingsObject settings_obj(FS::PathCombine(instDir, "instance.cfg"));
	settings_obj.registerSetting("InstanceType", "Legacy");
	QString inst_type = settings_obj.get("InstanceType").toString();

	oldInstance->copy(instDir);

	auto error = loadInstance(newInstance, instDir);

	switch (error)
	{
	case NoLoadError:
		return NoCreateError;
	case NotAnInstance:
		rootDir.removeRecursively();
		return CantCreateDir;
	default:
	case UnknownLoadError:
		rootDir.removeRecursively();
		return UnknownCreateError;
	}
	*/
	return UnknownCreateError;
}

void InstanceList::instanceNuked(BaseInstance *inst)
{
	int i = getInstIndex(inst);
	if (i != -1)
	{
		beginRemoveRows(QModelIndex(), i, i);
		m_instances.removeAt(i);
		endRemoveRows();
	}
}

void InstanceList::propertiesChanged(BaseInstance *inst)
{
	int i = getInstIndex(inst);
	if (i != -1)
	{
		emit dataChanged(index(i), index(i));
	}
}
