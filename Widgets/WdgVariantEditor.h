#ifndef WDGVARIANTEDITOR_H
#define WDGVARIANTEDITOR_H

#include <QWidget>
#include <QDialog>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QColorDialog>
#include <QListWidgetItem>

#include "Global/Global.h"
#include "Global/ActionShortcutMapper.h"

namespace Ui {
class WdgVariantEditor;
}

class WdgVariantEditor : public QWidget {
    Q_OBJECT

public:
    explicit WdgVariantEditor(QWidget *parent = nullptr);
    ~WdgVariantEditor();

    enum Page {
        BoolPage,
        IntPage,
        FloatPage,
        CharPage,
        StringPage,
        ByteArrayPage,
        DatePage,
        TimePage,
        DateTimePage,
        PointPage,
        SizePage,
        RectPage,
        ColorPage,
        LocalePage,
        FontPage,
        KeySequencePage,
        ListPage,
        MapPage,
        ReadOnlyTextPage,
        ReadOnlyGraphicPage,
        InvalidPage
    };

    enum ByteArrayType {
        HeXType,
        Utf8Type
    };

    QVariant value();
    QMetaType type();
    bool readOnly();

    static QVariant dialogEditor(const QVariant &value, const bool &readOnly = false, bool *ok = nullptr, QWidget *parent = nullptr);

public slots:
    void setValue(const QVariant &value);
    void setReadOnly(const bool &readOnly);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

    void loadColor();
    void loadStringList();

    void loadValue();
    void openPage(const Page &page);
    QString byteArrayToHex(const QByteArray &array);
    QByteArray hexToByteArray(const QString &hex);


protected slots:
    void onByteArrayTypeChanged(const int &type);
    void updateValue();

signals:
    void valueChanged(QVariant value, QMetaType type);
    void readOnlyChanged(bool readOnly);

private slots:
    void on_pbSelectColor_clicked();

    void onListSelectionChanged();
    void onListAdd();
    void onListEdit();
    void onListDelete();
    void onListUp();
    void onListDown();

    void moveItem(const int &row, const int &offset);

    void on_lwList_itemDoubleClicked(QListWidgetItem *item);

private:
    QAction *_listAddAction, *_listEditAction, *_listDeleteAction, *_listUpAction, *_listDownAction;
    Ui::WdgVariantEditor *ui;
    QVariant _value;
    QMetaType _type;
    bool _readOnly;
    ByteArrayType _byteArrayType;

    bool _blockUpdate;

};

#endif // WDGVARIANTEDITOR_H
