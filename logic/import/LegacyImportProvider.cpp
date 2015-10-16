#include "LegacyImportProvider.h"

#include <QAbstractListModel>

class LegacyInstancesModel : public QAbstractListModel
{
public:
	explicit LegacyInstancesModel(QObject *parent = nullptr)
		: QAbstractListModel(parent)
	{
	}

	int rowCount(const QModelIndex &parent) const override
	{
		return m_candidates.size();
	}
	QVariant data(const QModelIndex &index, int role) const override
	{
		return QVariant();
	}

private:
	QVector<int> m_candidates;
};

LegacyImportProvider::LegacyImportProvider()
	: BaseImportProvider("Legacy MultiMC", "icon:legacy")
{
}

void LegacyImportProvider::import(const QModelIndex &index, const InstancePtr &instance) const
{
}
WonkoVersionPtr LegacyImportProvider::baseVersion(const QModelIndex &index) const
{
	return nullptr;
}

std::unique_ptr<QAbstractItemModel> LegacyImportProvider::createCandidatesModel() const
{
	return std::make_unique<LegacyInstancesModel>();
}
