#pragma once

#include "VersionFile.h"
#include "Library.h"
#include <QJsonDocument>

#include "multimc_minecraft_export.h"

class MULTIMC_MINECRAFT_EXPORT MojangVersionFormat
{
friend class OneSixVersionFormat;
protected:
	// does not include libraries
	static void readVersionProperties(const QJsonObject& in, VersionFile* out);
	// does not include libraries
	static void writeVersionProperties(const VersionFile* in, QJsonObject& out);
public:
	// version files / profile patches
	static VersionFilePtr versionFileFromJson(const QJsonDocument &doc, const QString &filename);
	static QJsonDocument versionFileToJson(const VersionFilePtr &patch);

	// libraries
	static LibraryPtr libraryFromJson(const QJsonObject &libObj, const QString &filename);
	static QJsonObject libraryToJson(Library *library);
};
