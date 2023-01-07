#include "AssetItem.h"

AssetItem::AssetItem(const QString &name, const QString &fpath, bool bigitm) :
    QListWidgetItem(nullptr, 1001),
    name(name),
    fpath(fpath),
    big(bigitm)
{
    setData(Qt::DisplayRole, name);
}

