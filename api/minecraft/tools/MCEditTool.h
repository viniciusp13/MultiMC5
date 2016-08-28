#pragma once

#include <tools/BaseExternalTool.h>

#include "multimc_minecraft_export.h"

class MULTIMC_MINECRAFT_EXPORT MCEditTool : public BaseDetachedTool
{
	Q_OBJECT
public:
	explicit MCEditTool(SettingsObjectPtr settings, InstancePtr instance, QObject *parent = 0);

protected:
	QString getSave() const;
	void runImpl() override;
};

class MULTIMC_MINECRAFT_EXPORT MCEditFactory : public BaseDetachedToolFactory
{
public:
	QString name() const override { return "MCEdit"; }
	void registerSettings(SettingsObjectPtr settings) override;
	BaseExternalTool *createTool(InstancePtr instance, QObject *parent = 0) override;
	bool check(QString *error) override;
	bool check(const QString &path, QString *error) override;
};
