#pragma once

#include "ModFolderPage.h"

class CoreModFolderPage : public ModFolderPage
{
public:
	explicit CoreModFolderPage(BaseInstance *inst, std::shared_ptr<ModList> mods, QString id,
							   QString iconName, QString displayName, QString helpPage = "",
							   QWidget *parent = 0);
	virtual ~CoreModFolderPage()
	{
	}
	virtual bool shouldDisplay() const;
};
