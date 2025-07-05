#ifndef WDGCOLORSELECTOR_H
#define WDGCOLORSELECTOR_H

#include <QWidget>

namespace Ui {
class WdgColorSelector;
}

class WdgColorSelector : public QWidget {
    Q_OBJECT

public:
    explicit WdgColorSelector(QWidget *parent = nullptr);
    ~WdgColorSelector();

    QColor color() const;
    void setColor(const QColor &newColor);

private slots:
    void on_pbSelect_clicked();

signals:
    void colorChanged(const QColor &color);

private:
    Ui::WdgColorSelector *ui;

    QColor _color;
};

#endif // WDGCOLORSELECTOR_H
