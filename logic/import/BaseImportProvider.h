#pragma once

#include <QString>
#include <QAbstractItemModel>
#include <memory>

#include "multimc_logic_export.h"

class QModelIndex;

using InstancePtr = std::shared_ptr<class BaseInstance>;
using WonkoVersionPtr = std::shared_ptr<class WonkoVersion>;

class MULTIMC_LOGIC_EXPORT BaseImportProvider : public QObject
{
	Q_OBJECT
public:
	explicit BaseImportProvider(const QString &name, const QString &iconName);
	virtual ~BaseImportProvider() {}

	QString name() const { return m_name; }
	QString iconName() const { return m_iconName; }

	enum
	{
		IconPathRole = Qt::UserRole,
		IconNameRole
	};
	QAbstractItemModel *candidatesModel();

	virtual void import(const QModelIndex &index, const InstancePtr &instance) const = 0;
	virtual WonkoVersionPtr baseVersion(const QModelIndex &index) const = 0;

protected:
	virtual std::unique_ptr<QAbstractItemModel> createCandidatesModel() const = 0;

private:
	QString m_name;
	QString m_iconName;
	std::unique_ptr<QAbstractItemModel> m_candidatesModel;
};
