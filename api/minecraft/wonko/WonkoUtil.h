/* Copyright 2015 MultiMC Contributors
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

#pragma once

#include "multimc_minecraft_export.h"

#include <QUrl>
#include <QDir>
#include <QString>
#include <memory>

class WonkoIndex;

class MULTIMC_MINECRAFT_EXPORT Wonko
{
public:
	Wonko(QString rootURL)
	{
		m_wonkoRootUrl = rootURL;
	}
	QUrl rootUrl();
	QUrl indexUrl();
	QUrl versionListUrl(const QString &uid);
	QUrl versionUrl(const QString &uid, const QString &version);
	QDir localWonkoDir();

	std::shared_ptr<WonkoIndex> wonkoIndex();

	void setRootUrl(const QString &url)
	{
		m_wonkoRootUrl = url;
	}

private:
	std::shared_ptr<WonkoIndex> m_wonkoIndex;
	QString m_wonkoRootUrl;
};
