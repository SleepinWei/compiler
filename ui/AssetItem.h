#include <QListWidgetItem>

class AssetItem : public QListWidgetItem {

public:
    AssetItem(const QString &name, const QString &fpath);

public:
    QString name;
    QString fpath;
};
