#pragma once

#include <QAbstractListModel>
#include <memory>

#include "multimc_logic_export.h"

class BaseImportProvider;
using SettingsObjectPtr = std::shared_ptr<class SettingsObject>;

class MULTIMC_LOGIC_EXPORT ImportProviderModel : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit ImportProviderModel(const SettingsObjectPtr &settings, QObject *parent = nullptr);

	enum
	{
		PointerRole = Qt::UserRole
	};

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;

	BaseImportProvider *provider(const QModelIndex &index) const;

private:
	std::vector<std::unique_ptr<BaseImportProvider>> m_providers;
};
