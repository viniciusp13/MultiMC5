/* Copyright 2013-2015 MultiMC Contributors
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

#include "MultiMC.h"
#include "NewInstanceDialog.h"
#include "ui_NewInstanceDialog.h"

#include "BaseVersion.h"
#include "icons/IconList.h"
#include "tasks/Task.h"
#include "InstanceList.h"
#include "WonkoGui.h"
#include "wonko/WonkoVersionList.h"
#include "wonko/WonkoVersion.h"
#include "import/ImportProviderModel.h"
#include "import/BaseImportProvider.h"
#include "resources/ResourceProxyModel.h"

#include "VersionSelectDialog.h"
#include "ProgressDialog.h"
#include "IconPickerDialog.h"
#include "WonkoGui.h"

#include <QLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QValidator>
#include <QMessageBox>

class UrlValidator : public QValidator
{
public:
	using QValidator::QValidator;

	State validate(QString &in, int &pos) const
	{
		const QUrl url(in);
		if (url.isValid() && !url.isRelative() && !url.isEmpty())
		{
			return Acceptable;
		}
		else if (QFile::exists(in))
		{
			return Acceptable;
		}
		else
		{
			return Intermediate;
		}
	}
};

NewInstanceDialog::NewInstanceDialog(QWidget *parent)
	: QDialog(parent), ui(new Ui::NewInstanceDialog)
{
	ui->setupUi(this);
	resize(minimumSizeHint());
	layout()->setSizeConstraint(QLayout::SetFixedSize);

	m_versionList = Wonko::ensureVersionListExists("net.minecraft", this);
	setSelectedVersion(m_versionList->getRecommended());
	InstIconKey = "default";
	ui->iconButton->setIcon(ENV.icons()->getIcon(InstIconKey));

	ui->modpackEdit->setValidator(new UrlValidator(ui->modpackEdit));

	ui->importProviderBox->setModel(ResourceProxyModel::mixin<QIcon>(new ImportProviderModel(MMC->settings(), this)));

	connect(ui->modpackEdit, &QLineEdit::textChanged, this, &NewInstanceDialog::updateDialogState);
	connect(ui->versionBox, &QRadioButton::clicked, this, &NewInstanceDialog::updateDialogState);
	connect(ui->modpackBox, &QRadioButton::clicked, this, &NewInstanceDialog::updateDialogState);
	connect(ui->importBtn, &QRadioButton::clicked, this, &NewInstanceDialog::updateDialogState);
	connect(ui->versionTextBox, &QLineEdit::textChanged, this, &NewInstanceDialog::updateDialogState);
	connect(ui->importCandidateBox, &QComboBox::currentTextChanged, this, &NewInstanceDialog::updateDialogState);

	auto groups = MMC->instances()->getGroups().toSet();
	auto groupList = QStringList(groups.toList());
	groupList.sort(Qt::CaseInsensitive);
	groupList.removeOne("");
	QString oldValue = MMC->settings()->get("LastUsedGroupForNewInstance").toString();
	groupList.push_front(oldValue);
	groupList.push_front("");
	ui->groupBox->addItems(groupList);
	int index = groupList.indexOf(oldValue);
	if(index == -1)
	{
		index = 0;
	}
	ui->groupBox->setCurrentIndex(index);
	ui->groupBox->lineEdit()->setPlaceholderText(tr("No group"));
	ui->buttonBox->setFocus();

	originalPlaceholderText = ui->instNameTextBox->placeholderText();
	updateDialogState();
	on_importProviderBox_currentIndexChanged();
}

NewInstanceDialog::~NewInstanceDialog()
{
	delete ui;
}

void NewInstanceDialog::updateDialogState()
{
	QString suggestedName;
	if (ui->versionBox->isChecked())
	{
		suggestedName = ui->versionTextBox->text();
	}
	else if (ui->modpackBox->isChecked())
	{
		auto url = QUrl::fromUserInput(ui->modpackEdit->text());
		QFileInfo fi(url.fileName());
		suggestedName = fi.completeBaseName();
	}
	else if (ui->importBtn->isChecked())
	{
		suggestedName = ui->importCandidateBox->currentText();

		const QVariant iconName = ui->importCandidateBox->currentData(BaseImportProvider::IconNameRole);
		if (!iconName.isNull())
		{
			const QVariant path = ui->importCandidateBox->currentData(BaseImportProvider::IconPathRole);
			if (!path.isNull())
			{
				ENV.icons()->addIcon(iconName.toString(), iconName.toString(), path.toString(), MMCIcon::Transient);
			}
			InstIconKey = iconName.toString();
			ui->iconButton->setIcon(ENV.icons()->getIcon(InstIconKey));
		}
	}
	if(suggestedName.isEmpty())
	{
		ui->instNameTextBox->setPlaceholderText(originalPlaceholderText);
	}
	else
	{
		ui->instNameTextBox->setPlaceholderText(suggestedName);
	}
	bool allowOK = !instName().isEmpty() && (
		(ui->versionBox->isChecked() && m_selectedVersion) ||
		(ui->modpackBox->isChecked() && ui->modpackEdit->hasAcceptableInput() ||
		(ui->importBtn->isChecked() && ui->importCandidateBox->currentIndex() != -1))
	);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(allowOK);
}

void NewInstanceDialog::setSelectedVersion(BaseVersionPtr version)
{
	m_selectedVersion = version;

	if (m_selectedVersion)
	{
		ui->versionTextBox->setText(version->name());
	}
	else
	{
		ui->versionTextBox->setText("");
	}
}

QString NewInstanceDialog::instName() const
{
	auto result = ui->instNameTextBox->text();
	if(result.size())
	{
		return result;
	}
	result = ui->instNameTextBox->placeholderText();
	if(result.size() && result != originalPlaceholderText)
	{
		return result;
	}
	return QString();
}

QString NewInstanceDialog::instGroup() const
{
	return ui->groupBox->currentText();
}
QString NewInstanceDialog::iconKey() const
{
	return InstIconKey;
}
QUrl NewInstanceDialog::modpackUrl() const
{
	if (ui->modpackBox->isChecked())
	{
		const QUrl url(ui->modpackEdit->text());
		if (url.isValid() && !url.isRelative() && !url.host().isEmpty())
		{
			return url;
		}
		else
		{
			return QUrl::fromLocalFile(ui->modpackEdit->text());
		}
	}
	else
	{
		return QUrl();
	}
}

BaseVersionPtr NewInstanceDialog::selectedVersion()
{
	if (isImported())
	{
		// FIXME: this shouldn't be here
		Wonko::ensureVersionListLoaded("net.minecraft", this);
		return selectedImportProvider()->baseVersion(currentCandidateIndex());
	}
	else
	{
		return m_selectedVersion;
	}
}

bool NewInstanceDialog::isImported() const
{
	return ui->importBtn->isChecked();
}

void NewInstanceDialog::import(const InstancePtr &instance)
{
	selectedImportProvider()->import(currentCandidateIndex(), instance);
}

void NewInstanceDialog::on_btnChangeVersion_clicked()
{
	VersionSelectDialog vselect(m_versionList.get(), tr("Change Minecraft version"),
								this);
	vselect.exec();
	if (vselect.result() == QDialog::Accepted)
	{
		BaseVersionPtr version = vselect.selectedVersion();
		if (version)
			setSelectedVersion(version);
	}
}

void NewInstanceDialog::on_iconButton_clicked()
{
	IconPickerDialog dlg(this);
	dlg.execWithSelection(InstIconKey);

	if (dlg.result() == QDialog::Accepted)
	{
		InstIconKey = dlg.selectedIconKey;
		ui->iconButton->setIcon(ENV.icons()->getIcon(InstIconKey));
	}
}

void NewInstanceDialog::on_instNameTextBox_textChanged(const QString &arg1)
{
	updateDialogState();
}

void NewInstanceDialog::on_importProviderBox_currentIndexChanged()
{
	if (ui->importProviderBox->currentIndex() == -1)
	{
		ui->importCandidateBox->setModel(nullptr);
	}
	else
	{
		BaseImportProvider *provider = selectedImportProvider();
		if (!m_importModels.contains(provider->name()))
		{
			m_importModels.insert(provider->name(), ResourceProxyModel::mixin<QIcon>(provider->candidatesModel()));
		}
		ui->importCandidateBox->setModel(m_importModels.value(provider->name()));
	}
}

BaseImportProvider *NewInstanceDialog::selectedImportProvider() const
{
	return ui->importProviderBox->currentData(ImportProviderModel::PointerRole).value<BaseImportProvider *>();
}
QModelIndex NewInstanceDialog::currentCandidateIndex() const
{
	const QModelIndex proxyIndex = ui->importCandidateBox->model()->index(ui->importCandidateBox->currentIndex(), 0);
	return m_importModels.value(selectedImportProvider()->name())->mapToSource(proxyIndex);
}

void NewInstanceDialog::on_modpackBtn_clicked()
{
	const QUrl url = QFileDialog::getOpenFileUrl(this, tr("Choose modpack"), modpackUrl(), tr("Zip (*.zip)"));
	if (url.isValid())
	{
		if (url.isLocalFile())
		{
			ui->modpackEdit->setText(url.toLocalFile());
		}
		else
		{
			ui->modpackEdit->setText(url.toString());
		}
	}
}
