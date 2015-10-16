#include "FTBImportProvider.h"

#include <QAbstractListModel>
#include <QDir>
#include <QXmlStreamReader>
#include <QSet>
#include <QUrl>
#include <QDebug>
#include <QRegularExpression>

#include "settings/SettingsObject.h"
#include "wonko/WonkoIndex.h"
#include "wonko/WonkoVersionList.h"
#include "wonko/WonkoVersion.h"
#include "settings/INISettingsObject.h"
#include "minecraft/OneSixInstance.h"
#include "InstanceList.h"
#include "icons/IconList.h"
#include "FileSystem.h"
#include "Env.h"
#include "Json.h"

struct FTBRecord
{
	QString dirName;
	QString name;
	QString logo;
	QString iconKey;
	QString mcVersion;
	QString description;
	QDir instanceDir;
	QString templateDir;

	QString iconFileName() const
	{
		return QDir(templateDir).absoluteFilePath(logo);
	}

	bool operator==(const FTBRecord other) const
	{
		return instanceDir == other.instanceDir;
	}
};
inline uint qHash(FTBRecord record)
{
	return qHash(record.instanceDir.absolutePath());
}

class FTBPacksModel : public QAbstractListModel
{
public:
	explicit FTBPacksModel(const SettingsObjectPtr &settings, QObject *parent = nullptr)
		: QAbstractListModel(parent), m_settings(settings)
	{
		m_candidates = discoverFTBInstances().toList().toVector();
	}

	int rowCount(const QModelIndex &parent) const override
	{
		return m_candidates.size();
	}
	QVariant data(const QModelIndex &index, int role) const override
	{
		if (!index.isValid() || index.row() < 0 || index.row() >= m_candidates.size())
		{
			return QVariant();
		}

		const FTBRecord &record = m_candidates.at(index.row());
		switch (role)
		{
		case Qt::DisplayRole: return record.name;
		case Qt::DecorationRole: return "web:" + QUrl::fromLocalFile(record.iconFileName()).toString();
		case BaseImportProvider::IconPathRole: return record.iconFileName();
		case BaseImportProvider::IconNameRole: return record.iconKey;
		default: return QVariant();
		}
	}

	const FTBRecord &record(const QModelIndex &index) const
	{
		return m_candidates.at(index.row());
	}

private:
	QVector<FTBRecord> m_candidates;
	SettingsObjectPtr m_settings;

	QSet<FTBRecord> discoverFTBInstances()
	{
		QSet<FTBRecord> records;
		QDir dir = QDir(m_settings->get("FTBLauncherLocal").toString());
		QDir dataDir = QDir(m_settings->get("FTBRoot").toString());
		if (!dataDir.exists())
		{
			qDebug() << "The FTB directory specified does not exist. Please check your settings";
			return records;
		}
		else if (!dir.exists())
		{
			qDebug() << "The FTB launcher data directory specified does not exist. Please check "
						"your settings";
			return records;
		}
		dir.cd("ModPacks");
		auto allFiles = dir.entryList(QDir::Readable | QDir::Files, QDir::Name);
		for (auto filename : allFiles)
		{
			if (!filename.endsWith(".xml"))
				continue;
			auto fpath = dir.absoluteFilePath(filename);
			QFile f(fpath);
			qDebug() << "Discovering FTB instances -- " << fpath;
			if (!f.open(QFile::ReadOnly))
				continue;

			// read the FTB packs XML.
			QXmlStreamReader reader(&f);
			while (!reader.atEnd())
			{
				switch (reader.readNext())
				{
				case QXmlStreamReader::StartElement:
				{
					if (reader.name() == "modpack")
					{
						QXmlStreamAttributes attrs = reader.attributes();
						FTBRecord record;
						record.dirName = attrs.value("dir").toString();
						record.instanceDir = dataDir.absoluteFilePath(record.dirName);
						record.templateDir = dir.absoluteFilePath(record.dirName);
						qDebug() << dataDir.absolutePath() << record.instanceDir.absolutePath() << record.dirName;
						if (!record.instanceDir.exists())
							continue;
						record.name = attrs.value("name").toString();
						record.logo = attrs.value("logo").toString();
						QString logo = record.logo;
						record.iconKey = logo.remove(QRegularExpression("\\..*"));
						auto customVersions = attrs.value("customMCVersions");
						if (!customVersions.isNull())
						{
							QMap<QString, QString> versionMatcher;
							QString customVersionsStr = customVersions.toString();
							QStringList list = customVersionsStr.split(';');
							for (auto item : list)
							{
								auto segment = item.split('^');
								if (segment.size() != 2)
								{
									qCritical() << "FTB: Segment of size < 2 in "
												<< customVersionsStr;
									continue;
								}
								versionMatcher[segment[0]] = segment[1];
							}
							auto actualVersion = attrs.value("version").toString();
							if (versionMatcher.contains(actualVersion))
							{
								record.mcVersion = versionMatcher[actualVersion];
							}
							else
							{
								record.mcVersion = attrs.value("mcVersion").toString();
							}
						}
						else
						{
							record.mcVersion = attrs.value("mcVersion").toString();
						}
						record.description = attrs.value("description").toString();
						records.insert(record);
					}
					break;
				}
				case QXmlStreamReader::EndElement:
					break;
				case QXmlStreamReader::Characters:
					break;
				default:
					break;
				}
			}
			f.close();
		}
		return records;
	}
};

FTBImportProvider::FTBImportProvider(const SettingsObjectPtr &settings)
	: BaseImportProvider("Feed the Beast", "icon:ftb"), m_settings(settings)
{
}

void FTBImportProvider::import(const QModelIndex &index, const InstancePtr &inst) const
{
	std::shared_ptr<OneSixInstance> instance = std::dynamic_pointer_cast<OneSixInstance>(inst);

	const FTBRecord &record = dynamic_cast<const FTBPacksModel *>(index.model())->record(index);

	// TODO: legacy-style packs
	this_does_not_exist();

	QStringList libraryNames;
	// create patch file
	{
		using namespace Json;

		qDebug()<< "Creating patch file for FTB instance...";
		QJsonObject root = requireObject(requireDocument(record.instanceDir.absoluteFilePath("minecraft/pack.json")));
		QJsonArray outLibs;
		for (QJsonObject libObj : ensureIsArrayOf<QJsonObject>(root, "libraries"))
		{
			libObj.insert("MMC-hint", QString("local"));
			libObj.insert("insert", QString("prepend"));
			libraryNames.append(requireString(libObj, "name"));
			outLibs.append(libObj);
		}
		root.remove("libraries");
		root.remove("id");

		// HACK HACK HACK HACK
		// A workaround for a problem in MultiMC, triggered by a historical problem in FTB,
		// triggered by Mojang getting their library versions wrong in 1.7.10
		if (instance->intendedVersionId() == "1.7.10")
		{
			auto insert = [&outLibs, &libraryNames](QString name)
			{
				QJsonObject libObj;
				libObj.insert("insert", QString("replace"));
				libObj.insert("name", name);
				libraryNames.append(name);
				outLibs.prepend(libObj);
			};
			insert("com.google.guava:guava:16.0");
			insert("org.apache.commons:commons-lang3:3.2.1");
		}
		root.insert("+libraries", outLibs);
		root.insert("order", 1);
		root.insert("fileId", QString("org.multimc.ftb.pack.json"));
		root.insert("name", instance->name());
		root.insert("mcVersion", instance->intendedVersionId());
		root.insert("version", instance->intendedVersionId());
		FS::write(instance->instanceRoot() + "/patches/ftb.json", toText(root));
	}
	// copy libraries
	{
		qDebug() << "Copying FTB libraries";
		const QDir librariesSource = QDir(m_settings->get("FTBRoot").toString() + "/libraries");
		for (const QString &library : libraryNames)
		{
			OneSixLibrary *lib = new OneSixLibrary(library);
			const QString out = QDir::current().absoluteFilePath("libraries/" + lib->storageSuffix());
			if (QFile::exists(out))
			{
				continue;
			}
			if (!FS::ensureFilePathExists(out))
			{
				qCritical() << "Couldn't create folder structure for" << out;
			}
			if (!QFile::copy(librariesSource.absoluteFilePath(lib->storageSuffix()), out))
			{
				qCritical() << "Couldn't copy" << lib->rawName();
			}
		}
	}
	// copy other files
	{
		FS::copyPath(record.instanceDir.absoluteFilePath("minecraft"), instance->minecraftRoot(), false);
	}

	ENV.icons()->installIcons(QStringList() << record.iconFileName());
	instance->setNotes(record.description);
}

WonkoVersionPtr FTBImportProvider::baseVersion(const QModelIndex &index) const
{
	const QString version = dynamic_cast<const FTBPacksModel *>(index.model())->record(index).mcVersion;
	return ENV.wonkoIndex()->getListGuaranteed("net.minecraft")->version(version);
}

std::unique_ptr<QAbstractItemModel> FTBImportProvider::createCandidatesModel() const
{
	return std::make_unique<FTBPacksModel>(m_settings);
}
