#include "BaseImportProvider.h"

BaseImportProvider::BaseImportProvider(const QString &name, const QString &iconName)
	: QObject(), m_name(name), m_iconName(iconName)
{
}

QAbstractItemModel *BaseImportProvider::candidatesModel()
{
	if (!m_candidatesModel)
	{
		m_candidatesModel = createCandidatesModel();
	}
	return m_candidatesModel.get();
}
