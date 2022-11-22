#include "AssetItem.h"

AssetItem::AssetItem(const QString &name, const QString &fpath) :
    QListWidgetItem(nullptr, 1001),
    name(name),
    fpath(fpath)
{
    setData(Qt::DisplayRole, name);
}

