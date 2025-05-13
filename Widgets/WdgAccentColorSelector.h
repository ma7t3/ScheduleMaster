#ifndef WDGACCENTCOLORSELECTOR_H
#define WDGACCENTCOLORSELECTOR_H

#include <QWidget>

class QToolButton;

namespace Ui {
class WdgAccentColorSelector;
}

class WdgAccentColorSelector : public QWidget
{
    Q_OBJECT

public:
    explicit WdgAccentColorSelector(QWidget *parent = nullptr);
    ~WdgAccentColorSelector();

    QString accentColorID() const;
    QColor accentColor() const;
    void setAccentColor(const QString &id);

protected:
    QString generateStyleSheet(const QString &id, const bool &selected = false) const;

    void updateButtons();

signals:
    void currentAccentColorChanged(QString id);

private:
    Ui::WdgAccentColorSelector *ui;

    QStringList _accentColorIDs;
    QList<QToolButton *> _buttons;
    int _currentColor;
};

#endif // WDGACCENTCOLORSELECTOR_H
