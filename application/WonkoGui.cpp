#include "WonkoGui.h"

#include "dialogs/ProgressDialog.h"
#include "wonko/Wonko.h"
#include "wonko/WonkoIndex.h"
#include "wonko/WonkoVersionList.h"
#include "wonko/WonkoVersion.h"
#include "MultiMC.h"

WonkoIndexPtr WonkoGui::ensureIndexLoaded(QWidget *parent)
{
	auto index = MMC->wonko()->wonkoIndex();
	if (!index->isLocalLoaded())
	{
		ProgressDialog(parent).execWithTask(index->localUpdateTask());
		if (!index->isRemoteLoaded() && index->lists().size() == 0)
		{
			ProgressDialog(parent).execWithTask(index->remoteUpdateTask());
		}
	}
	return index;
}

WonkoVersionListPtr WonkoGui::ensureVersionListExists(const QString &uid, QWidget *parent)
{
	auto index = MMC->wonko()->wonkoIndex();
	ensureIndexLoaded(parent);
	if (!index->isRemoteLoaded() && !index->hasUid(uid))
	{
		ProgressDialog(parent).execWithTask(index->remoteUpdateTask());
	}
	return index->getList(uid);
}
WonkoVersionListPtr WonkoGui::ensureVersionListLoaded(const QString &uid, QWidget *parent)
{
	WonkoVersionListPtr list = ensureVersionListExists(uid, parent);
	if (!list)
	{
		return nullptr;
	}
	if (!list->isLocalLoaded())
	{
		ProgressDialog(parent).execWithTask(list->localUpdateTask());
		if (!list->isLocalLoaded())
		{
			ProgressDialog(parent).execWithTask(list->remoteUpdateTask());
		}
	}
	return list->isComplete() ? list : nullptr;
}

WonkoVersionPtr WonkoGui::ensureVersionExists(const QString &uid, const QString &version, QWidget *parent)
{
	WonkoVersionListPtr list = ensureVersionListLoaded(uid, parent);
	if (!list)
	{
		return nullptr;
	}
	return list->getVersion(version);
}
WonkoVersionPtr WonkoGui::ensureVersionLoaded(const QString &uid, const QString &version, QWidget *parent, const UpdateType update)
{
	WonkoVersionPtr vptr = ensureVersionExists(uid, version, parent);
	if (!vptr)
	{
		return nullptr;
	}
	if (!vptr->isLocalLoaded() || update == AlwaysUpdate)
	{
		ProgressDialog(parent).execWithTask(vptr->localUpdateTask());
		if (!vptr->isLocalLoaded() || update == AlwaysUpdate)
		{
			ProgressDialog(parent).execWithTask(vptr->remoteUpdateTask());
		}
	}
	return vptr->isComplete() ? vptr : nullptr;
}
