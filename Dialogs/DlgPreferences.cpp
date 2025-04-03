#include "DlgPreferences.h"
#include "ui_DlgPreferences.h"

DlgPreferences::DlgPreferences(QWidget *parent) : QDialog(parent),
    ui(new Ui::DlgPreferences) {
    ui->setupUi(this);

    qInfo() << "Loading preferences...";
    addPage(new WdgPreferencesPageGeneral(this));
    addPage(new WdgPreferencesPageAppearance(this));
    addPage(new WdgPreferencesPageLocations(this));
    addPage(new WdgPreferencesPagePlugins(this));
    addPage(new WdgPreferencesPageDebug(this));

    for(int i = 0; i < ui->lwList->count(); i++)
        ui->lwList->item(i)->setSizeHint(QSize(0, 32));

    ui->lwList->setFocus();
    ui->lwList->setCurrentRow(0);
}

DlgPreferences::~DlgPreferences() {
    delete ui;
}

void DlgPreferences::setCurrentPage(const int &index) {
    if(index < 0 || index >= ui->lwList->count())
        return;

    ui->lwList->setCurrentRow(index);
}

void DlgPreferences::setCurrentPage(const QString &id) {
    for(int i = 0; i < _pages.count(); i++)
        if(_pages[i]->id() == id)
            ui->lwList->setCurrentRow(i);
}

void DlgPreferences::addPage(WdgPreferencesPage *page) {
    _pages << page;

    QListWidgetItem *item = new QListWidgetItem(page->icon(), page->name());
    ui->lwList->addItem(item);

    ui->swContent->addWidget(page);
}

void DlgPreferences::savePreferences() {
    qInfo() << "Saving preferences...";

    for(WdgPreferencesPage *page : std::as_const(_pages))
        page->savePreferences();
}

void DlgPreferences::on_lwList_currentItemChanged(QListWidgetItem *current,
                                                  QListWidgetItem *previous) {
    Q_UNUSED(previous);
    ui->swContent->setCurrentIndex(ui->lwList->row(current));
    ui->lTitle->setText(current->text());
    ui->lTitleIcon->setPixmap(current->icon().pixmap(28, 28));
}

void DlgPreferences::on_pbReset_clicked() {
    QMessageBox::warning(this, tr("Not available"), tr("Reset preferences ist not available now."));
}

void DlgPreferences::accept() {
    savePreferences();
    QDialog::accept();
}
