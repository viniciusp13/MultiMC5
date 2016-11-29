#include "CoreModFolderPage.h"
#include <minecraft/VersionFilterData.h>

CoreModFolderPage::CoreModFolderPage(BaseInstance *inst, std::shared_ptr<ModList> mods,
									 QString id, QString iconName, QString displayName,
									 QString helpPage, QWidget *parent)
	: ModFolderPage(inst, mods, id, iconName, displayName, helpPage, parent)
{
}

bool CoreModFolderPage::shouldDisplay() const
{
	if (ModFolderPage::shouldDisplay())
	{
		auto inst = dynamic_cast<OneSixInstance *>(m_inst);
		if (!inst)
			return true;
		auto version = inst->getMinecraftProfile();
		if (!version)
			return true;
		if(!version->versionPatch("net.minecraftforge"))
		{
			return false;
		}
		if(!version->versionPatch("net.minecraft"))
		{
			return false;
		}
		if(version->versionPatch("net.minecraft")->getReleaseDateTime() < g_VersionFilterData.legacyCutoffDate)
		{
			return true;
		}
	}
	return false;
}
