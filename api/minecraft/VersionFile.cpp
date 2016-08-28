#include <QJsonArray>
#include <QJsonDocument>

#include <QDebug>

#include "VersionFile.h"
#include "Library.h"
#include "MinecraftProfile.h"
#include "JarMod.h"
#include "ParseUtils.h"

#include "VersionBuildError.h"
#include <Version.h>

bool VersionFile::isMinecraftVersion()
{
	return fileId == "net.minecraft";
}

bool VersionFile::hasJarMods()
{
	return !jarMods.isEmpty();
}

void VersionFile::applyTo(MinecraftProfile *profile)
{
	auto theirVersion = profile->getMinecraftVersion();
	if (!theirVersion.isNull() && !dependsOnMinecraftVersion.isNull())
	{
		if (QRegExp(dependsOnMinecraftVersion, Qt::CaseInsensitive, QRegExp::Wildcard).indexIn(theirVersion) == -1)
		{
			throw MinecraftVersionMismatch(fileId, dependsOnMinecraftVersion, theirVersion);
		}
	}
	profile->applyMinecraftVersion(minecraftVersion);
	profile->applyMainClass(mainClass);
	profile->applyAppletClass(appletClass);
	profile->applyMinecraftArguments(minecraftArguments);
	if (isMinecraftVersion())
	{
		profile->applyMinecraftVersionType(type);
	}
	profile->applyMinecraftAssets(mojangAssetIndex);
	profile->applyTweakers(addTweakers);

	profile->applyJarMods(jarMods);
	profile->applyTraits(traits);

	for (auto library : libraries)
	{
		profile->applyLibrary(library);
	}
	profile->applyProblemSeverity(getProblemSeverity());
	auto iter = mojangDownloads.begin();
	while(iter != mojangDownloads.end())
	{
		profile->applyMojangDownload(iter.key(), iter.value());
		iter++;
	}
}
