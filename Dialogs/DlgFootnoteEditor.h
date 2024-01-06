#ifndef DLGFOOTNOTEEDITOR_H
#define DLGFOOTNOTEEDITOR_H

#include <QDialog>

#include "ProjectData/projectdata.h"

namespace Ui {
class DlgFootnoteEditor;
}

class DlgFootnoteEditor : public QDialog {
    Q_OBJECT

public:
    explicit DlgFootnoteEditor(QWidget *parent = nullptr, ProjectData *projectData = nullptr);
    ~DlgFootnoteEditor();

    void setCreateNewMode(bool create);

    QString identifier() const;
    void setIdentifier(const QString &newIdentifier);

    QString description() const;
    void setDescription(const QString &newDescription);

private:
    Ui::DlgFootnoteEditor *ui;
    ProjectData *projectData;
};

#endif // DLGFOOTNOTEEDITOR_H
