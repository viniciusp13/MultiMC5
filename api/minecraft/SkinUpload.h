#pragma once

#include <QFile>
#include <QtNetwork/QtNetwork>
#include <memory>

#include "auth/AuthSession.h"
#include "tasks/Task.h"
#include "multimc_minecraft_export.h"

typedef std::shared_ptr<class SkinUpload> SkinUploadPtr;

class MULTIMC_MINECRAFT_EXPORT SkinUpload : public Task
{
Q_OBJECT
public:
	enum Model
	{
		STEVE,
		ALEX
	};

	// Note this class takes ownership of the file.
	SkinUpload(QObject *parent, std::shared_ptr<Minecraft::AuthSession> session, QByteArray skin, Model model = STEVE);

	virtual ~SkinUpload() {}

private:
	Model m_model;
	QByteArray m_skin;
	std::shared_ptr<Minecraft::AuthSession> m_session;
	std::shared_ptr<QNetworkReply> m_reply;
protected:
	virtual void executeTask();

public slots:

	void downloadError(QNetworkReply::NetworkError);

	void downloadFinished();
};
