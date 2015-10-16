#pragma once

#include "BaseImportProvider.h"

class LegacyImportProvider : public BaseImportProvider
{
public:
	explicit LegacyImportProvider();

	void import(const QModelIndex &index, const InstancePtr &instance) const override;
	WonkoVersionPtr baseVersion(const QModelIndex &index) const override;

private:
	std::unique_ptr<QAbstractItemModel> createCandidatesModel() const override;
};
