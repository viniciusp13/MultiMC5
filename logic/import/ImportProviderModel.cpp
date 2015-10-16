#include "ImportProviderModel.h"

#include "BaseImportProvider.h"
#include "FTBImportProvider.h"
#include "LegacyImportProvider.h"

ImportProviderModel::ImportProviderModel(const SettingsObjectPtr &settings, QObject *parent)
	: QAbstractListModel(parent)
{
	m_providers.push_back(std::make_unique<FTBImportProvider>(settings));
	m_providers.push_back(std::make_unique<LegacyImportProvider>());
}

int ImportProviderModel::rowCount(const QModelIndex &parent) const
{
	return m_providers.size();
}

QVariant ImportProviderModel::data(const QModelIndex &index, int role) const
{
	BaseImportProvider *bip = provider(index);
	switch (role)
	{
	case Qt::DisplayRole: return bip->name();
	case Qt::DecorationRole: return bip->iconName();
	case PointerRole: return QVariant::fromValue(bip);
	default: return QVariant();
	}
}

BaseImportProvider *ImportProviderModel::provider(const QModelIndex &index) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= m_providers.size())
	{
		return nullptr;
	}
	return m_providers.at(index.row()).get();
}
