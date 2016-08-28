#pragma once
#include <QModelIndex>
#include <DesktopServices.h>
#include <QWidget>
#include <Mod.h>

bool lastfirst(QModelIndexList &list, int &first, int &last);

void showWebsiteForMod(QWidget *parentDlg, Mod &m);
