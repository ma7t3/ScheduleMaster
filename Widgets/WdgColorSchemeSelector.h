#ifndef WDGCOLORSCHEMESELECTOR_H
#define WDGCOLORSCHEMESELECTOR_H

#include <QWidget>

namespace Ui {
class WdgColorSchemeSelector;
}

class WdgColorSchemeSelector : public QWidget {
    Q_OBJECT

public:
    explicit WdgColorSchemeSelector(QWidget *parent = nullptr);
    ~WdgColorSchemeSelector();

    Qt::ColorScheme colorScheme() const;

public slots:
    void setColorScheme(const Qt::ColorScheme &colorScheme);

protected slots:
    void onColorSchemeChanged();

signals:
    void colorSchemeChanged(Qt::ColorScheme);

private:
    Ui::WdgColorSchemeSelector *ui;
};

#endif // WDGCOLORSCHEMESELECTOR_H
