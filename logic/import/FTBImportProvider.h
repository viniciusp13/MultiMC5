#pragma once

#include "BaseImportProvider.h"

using SettingsObjectPtr = std::shared_ptr<class SettingsObject>;

class FTBImportProvider : public BaseImportProvider
{
	Q_OBJECT
public:
	explicit FTBImportProvider(const SettingsObjectPtr &settings);

	void import(const QModelIndex &index, const InstancePtr &instance) const override;
	WonkoVersionPtr baseVersion(const QModelIndex &index) const override;

private:
	SettingsObjectPtr m_settings;

	std::unique_ptr<QAbstractItemModel> createCandidatesModel() const override;
};
