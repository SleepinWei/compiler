#include <QListWidgetItem>

class AssetItem : public QListWidgetItem {

public:
    AssetItem(const QString &name, const QString &fpath, bool bigitm = false);

public:
    QString name;
    QString fpath;
    bool big;
};
