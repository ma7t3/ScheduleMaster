#ifndef WDGPUBLISHEDLINES_H
#define WDGPUBLISHEDLINES_H

#include <QWidget>
#include <QUndoStack>
#include <QListWidgetItem>

#include "ProjectData/projectdata.h"

namespace Ui {
class WdgPublishedLines;
}

class WdgPublishedLines : public QWidget
{
    Q_OBJECT

public:
    explicit WdgPublishedLines(QWidget *parent = nullptr, ProjectData *projectData = nullptr, QUndoStack *undoStack = nullptr);
    ~WdgPublishedLines();

    PublishedLine *currentLine() const;

public slots:
    void setCurrentLine(PublishedLine *newCurrentLine);

    void actionNew();
    void actionEdit();
    void actionDelete();

    void actionDirectionNew();
    void actionDirectionEdit();
    void actionDirectionDelete();

    void refreshLineList();
    void refreshCurrentLine();

signals:
    void currentLineChanged(PublishedLine *);

private slots:
    void on_lwLines_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_pbFilePathBrowse_clicked();

    void on_lwDirections_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    Ui::WdgPublishedLines *ui;

    ProjectData *projectData;
    QUndoStack *undoStack;
    QList<PublishedLine *> m_listReference;
    QList<PublishedLineDirection *> m_directionsListReference;
    PublishedLine *m_currentLine;
    PublishedLineDirection *m_currentLineDirection;

    bool refreshing = false;
    bool refreshingCurrentLine = false;
};

#endif // WDGPUBLISHEDLINES_H
