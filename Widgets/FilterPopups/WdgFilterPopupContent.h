#ifndef WDGFILTERPOPUPCONTENT_H
#define WDGFILTERPOPUPCONTENT_H

#include <QWidget>

class WdgFilterPopupContent : public QWidget {
    Q_OBJECT
public:
    explicit WdgFilterPopupContent(QWidget *parent = nullptr);

    virtual bool filterIsEnabled() const = 0;

public slots:
    virtual void reset() = 0;

signals:
    void filterChanged();
};

#endif // WDGFILTERPOPUPCONTENT_H
