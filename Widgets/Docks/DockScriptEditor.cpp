#include "DockScriptEditor.h"
#include "ui_DockScriptEditor.h"

#include "Commands/CmdScripting.h"

#include "JavaScriptInterface/ProjectDataJsInterface.h"

#include "ApplicationInterface.h"
#include "ProjectData/ProjectData.h"

#include <QJSEngine>
#include <QUndoStack>

DockScriptEditor::DockScriptEditor(QWidget *parent) :
    DockAbstract(parent), ui(new Ui::DockScriptEditor) {
    ui->setupUi(this);

    connect(ui->pbScriptExecute, &QPushButton::clicked, this, &DockScriptEditor::executeScript);
}

DockScriptEditor::~DockScriptEditor() {
    delete ui;
}

void DockScriptEditor::executeScript() {
    QJSEngine engine;
    ProjectDataJsInterface interface;
    interface.setTarget(ApplicationInterface::projectData());

    engine.globalObject().setProperty("project", engine.newQObject(&interface));
    const QJSValue result = engine.evaluate(ui->pteScript->toPlainText(), "script.js");
    if(result.isError()) {
        qDebug() << "Error while executing script: " << result.property("message").toString()
                 << "in" << result.property("fileName").toString() << ":"
                 << result.property("lineNumber").toString();
    } else {
        qDebug() << "Script finished" << result.toString();
        QList<ProjectDataItem *> items = ApplicationInterface::projectData()->jsModifiedItems();
        if(items.isEmpty())
            return;

        ApplicationInterface::undoStack()->push(new CmdScriptOperation(items));
        for(ProjectDataItem *item : items)
            item->jsInterface()->endOperation();
    }
}
