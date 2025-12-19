#ifndef WDGBUSSTOPPLATFORMEDITOR_H
#define WDGBUSSTOPPLATFORMEDITOR_H

#include <QWidget>
#include <QCheckBox>
#include "src/projectdata/BusstopPlatform.h"

namespace Ui {
class WdgBusstopPlatformEditor;
}

class WdgBusstopPlatformEditor : public QWidget {
    Q_OBJECT

public:
    explicit WdgBusstopPlatformEditor(QWidget *parent = nullptr);
    ~WdgBusstopPlatformEditor();

    BusstopPlatform *platform() const;
    void setPlatform(BusstopPlatform *platform);

public slots:
    void focusName();

protected:
    void setFlags(const BusstopPlatformFlags &flags);
    BusstopPlatformFlags readCheckboxes() const;

    void blockCheckboxSignals(const bool &block);

protected slots:
    void onNameChanged(const QString &name);
    void onCommentChanged();
    void onFlagChanged(QCheckBox *checkBox, const bool &checked);

private:
    Ui::WdgBusstopPlatformEditor *ui;

    BusstopPlatform *_platform;
    QList<QCheckBox *> _flagCheckboxes;
};

#endif // WDGBUSSTOPPLATFORMEDITOR_H
