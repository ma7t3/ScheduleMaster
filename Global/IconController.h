#ifndef ICONCONTROLLER_H
#define ICONCONTROLLER_H

#include "Global/IconSetManager.h"

#include <QObject>

class IconController : public QObject {
    Q_OBJECT
protected:
    explicit IconController(QObject *parent);

public:
    static IconController *instance();

    static void init();

    IconController(const IconController &other) = delete;
    IconController& operator=(const IconController &other) = delete;

    static void applyIconSet(const QString &iconSetID = IconSetManager::currentIconSet());

    static QIcon icon(const QString &iconID);

protected slots:
    void onCurrentIconSetChanged(const QString &iconSetID);

    static QString createFilePath(const QString &iconID, const IconSetConfig &config);

signals:

private:
    static inline QString _currentIconSetID;
};

#endif // ICONCONTROLLER_H
