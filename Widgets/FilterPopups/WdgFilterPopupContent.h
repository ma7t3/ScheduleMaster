#ifndef WDGFILTERPOPUPCONTENT_H
#define WDGFILTERPOPUPCONTENT_H

#include <QWidget>

class WdgFilterPopupContent : public QWidget {
    Q_OBJECT
public:
    explicit WdgFilterPopupContent(QWidget *parent = nullptr);

    virtual void reset() = 0;
    virtual bool filterIsEnabled() const = 0;

signals:
    void filterChanged();

signals:
};

#endif // WDGFILTERPOPUPCONTENT_H
