#include "WdgVariantEditor.h"
#include "ui_WdgVariantEditor.h"

WdgVariantEditor::WdgVariantEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgVariantEditor),
    _readOnly(false),
    _byteArrayType(HeXType),
    _blockUpdate(false) {
    ui->setupUi(this);

    ui->cbBool->installEventFilter(this);

    _listAddAction    = ui->lwList->addAction(QIcon(":/Icons/Add.ico"),      tr("Add"));
    _listEditAction   = ui->lwList->addAction(QIcon(":/Icons/Edit.ico"),     tr("Edit"));
    _listDeleteAction = ui->lwList->addAction(QIcon(":/Icons/Delete.ico"),   tr("Delete"));
    _listUpAction     = ui->lwList->addAction(QIcon(":/Icons/MoveUp.ico"),   tr("Up"));
    _listDownAction   = ui->lwList->addAction(QIcon(":/Icons/MoveDown.ico"), tr("Down"));

    ActionShortcutMapper::map(_listEditAction, "variantEditor.listItem.edit");
    ActionShortcutMapper::map(_listDeleteAction, "variantEditor.listItem.delete");

    _listEditAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _listDeleteAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    ui->lwList->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(_listAddAction,    &QAction::triggered,      this,              &WdgVariantEditor::onListAdd);
    connect(_listEditAction,   &QAction::triggered,      this,              &WdgVariantEditor::onListEdit);
    connect(_listDeleteAction, &QAction::triggered,      this,              &WdgVariantEditor::onListDelete);
    connect(_listUpAction,     &QAction::triggered,      this,              &WdgVariantEditor::onListUp);
    connect(_listDownAction,   &QAction::triggered,      this,              &WdgVariantEditor::onListDown);

    connect(ui->pbListAdd,     &QPushButton::clicked,    _listAddAction,    &QAction::trigger);
    connect(ui->pbListEdit,    &QPushButton::clicked,    _listEditAction,   &QAction::trigger);
    connect(ui->pbListDelete,  &QPushButton::clicked,    _listDeleteAction, &QAction::trigger);
    connect(ui->pbListUp,      &QPushButton::clicked,    _listUpAction,     &QAction::trigger);
    connect(ui->pbListDown,    &QPushButton::clicked,    _listDownAction,   &QAction::trigger);

    connect(_listAddAction,    &QAction::enabledChanged, ui->pbListAdd,     &QPushButton::setEnabled);
    connect(_listEditAction,   &QAction::enabledChanged, ui->pbListEdit,    &QPushButton::setEnabled);
    connect(_listDeleteAction, &QAction::enabledChanged, ui->pbListDelete,  &QPushButton::setEnabled);
    connect(_listUpAction,     &QAction::enabledChanged, ui->pbListUp,      &QPushButton::setEnabled);
    connect(_listDownAction,   &QAction::enabledChanged, ui->pbListDown,    &QPushButton::setEnabled);

    connect(ui->lwList->selectionModel(), &QItemSelectionModel::selectionChanged, this, &WdgVariantEditor::onListSelectionChanged);
    onListSelectionChanged();

    const QList<QLocale> locales = QLocale::matchingLocales(
        QLocale::AnyLanguage,
        QLocale::AnyScript,
        QLocale::AnyCountry
    );

    for (const QLocale &locale : locales) {
        QString name = locale.name();
        QString text = QString("%1 (%2)").arg(locale.nativeLanguageName(), name);
        ui->cbLocale->addItem(text, name);
    }

    connect(ui->cbByteArrayType, &QComboBox::currentIndexChanged, this, &WdgVariantEditor::onByteArrayTypeChanged);

    // TODO: Connect all editor widgets to valueChanged signal!
    connect(ui->cbBool,        &QCheckBox::checkStateChanged,          this, &WdgVariantEditor::updateValue);
    connect(ui->sbInt,         &QSpinBox::valueChanged,                this, &WdgVariantEditor::updateValue);
    connect(ui->dsbFloat,      &QDoubleSpinBox::valueChanged,          this, &WdgVariantEditor::updateValue);
    connect(ui->leChar,        &QLineEdit::textChanged,                this, &WdgVariantEditor::updateValue);
    connect(ui->pteString,     &QPlainTextEdit::textChanged,           this, &WdgVariantEditor::updateValue);
    connect(ui->pteByteArray,  &QPlainTextEdit::textChanged,           this, &WdgVariantEditor::updateValue);
    connect(ui->deDate,        &QDateEdit::dateChanged,                this, &WdgVariantEditor::updateValue);
    connect(ui->teTime,        &QTimeEdit::timeChanged,                this, &WdgVariantEditor::updateValue);
    connect(ui->dteDateTime,   &QDateTimeEdit::dateTimeChanged,        this, &WdgVariantEditor::updateValue);

    connect(ui->dsbPointX,     &QDoubleSpinBox::valueChanged,          this, &WdgVariantEditor::updateValue);
    connect(ui->dsbPointY,     &QDoubleSpinBox::valueChanged,          this, &WdgVariantEditor::updateValue);

    connect(ui->dsbSizeWidth,  &QDoubleSpinBox::valueChanged,          this, &WdgVariantEditor::updateValue);
    connect(ui->dsbSizeHeight, &QDoubleSpinBox::valueChanged,          this, &WdgVariantEditor::updateValue);

    connect(ui->dsbRectX,      &QDoubleSpinBox::valueChanged,          this, &WdgVariantEditor::updateValue);
    connect(ui->dsbRectY,      &QDoubleSpinBox::valueChanged,          this, &WdgVariantEditor::updateValue);
    connect(ui->dsbRectWidth,  &QDoubleSpinBox::valueChanged,          this, &WdgVariantEditor::updateValue);
    connect(ui->dsbRectHeight, &QDoubleSpinBox::valueChanged,          this, &WdgVariantEditor::updateValue);

    connect(ui->kseKeySequence, &QKeySequenceEdit::keySequenceChanged, this, &WdgVariantEditor::updateValue);
}

WdgVariantEditor::~WdgVariantEditor() {
    delete ui;
}

QVariant WdgVariantEditor::value() {
    return _value;
}

QMetaType WdgVariantEditor::type() {
    return _type;
}

bool WdgVariantEditor::readOnly() {
    return _readOnly;
}

void WdgVariantEditor::setValue(const QVariant &value) {
    _value = value;
    _type = value.metaType();

    // load
    loadValue();

    emit valueChanged(_value, _type);
}

void WdgVariantEditor::setReadOnly(const bool &readOnly) {
    _readOnly = readOnly;

    ui->sbInt->setReadOnly(readOnly);
    ui->dsbFloat->setReadOnly(readOnly);
    ui->leChar->setReadOnly(readOnly);
    ui->pteString->setReadOnly(readOnly);
    ui->pteByteArray->setReadOnly(readOnly);
    ui->deDate->setReadOnly(readOnly);
    ui->teTime->setReadOnly(readOnly);
    ui->dteDateTime->setReadOnly(readOnly);
    ui->pbSelectColor->setVisible(readOnly);

    loadValue();

    emit readOnlyChanged(readOnly);
}

bool WdgVariantEditor::eventFilter(QObject *obj, QEvent *event) {
    if (_readOnly && obj == ui->cbBool) {
        if (event->type() == QEvent::MouseButtonPress ||
            event->type() == QEvent::MouseButtonRelease ||
            event->type() == QEvent::MouseButtonDblClick) {
            return true;
        }

        if (event->type() == QEvent::KeyPress ||
            event->type() == QEvent::KeyRelease) {
            return true;
        }

        if (event->type() == QEvent::FocusIn) {
            ui->cbBool->clearFocus();
            return true;
        }
    }

    return QWidget::eventFilter(obj, event);
}

void WdgVariantEditor::loadColor() {
    QColor color = _value.value<QColor>();
    ui->lColor->setStyleSheet("background-color: " + color.name() + "; color: " + Global::contrastColor(color).name());
    ui->lColor->setText(color.name());
}

void WdgVariantEditor::loadStringList() {
    const QStringList list = _value.toStringList();
    ui->lwList->clear();

    for(const QString &string : list) {
        const QStringList parts = string.split("\n");
        ui->lwList->addItem(parts.first() + (parts.count() > 1 ? "..." : ""));
    }

    emit valueChanged(_value, _type);
}

void WdgVariantEditor::loadValue() {
    _blockUpdate = true;

    int typeID = _type.id();

    int decimalCount = (
        typeID == QMetaType::QPointF ||
        typeID == QMetaType::QSizeF ||
        typeID == QMetaType::QLineF ||
        typeID == QMetaType::QRectF ||
        typeID == QMetaType::QPolygonF
    ) ? 6 : 0;

    switch(_type.id()) {
    case QMetaType::Bool:
        openPage(BoolPage);
        ui->cbBool->setChecked(_value.toBool());
        break;

    case QMetaType::Int: case QMetaType::Long: case QMetaType::LongLong: case QMetaType::Short:
    case QMetaType::UInt: case QMetaType::ULong: case QMetaType::ULongLong: case QMetaType::UShort:
        openPage(IntPage);
        ui->sbInt->setValue(_value.toLongLong());
        break;

    case QMetaType::Float: case QMetaType::Float16: case QMetaType::Double:
        openPage(FloatPage);
        ui->dsbFloat->setValue(_value.toDouble());
        break;

    case QMetaType::Char: case QMetaType::Char16: case QMetaType::Char32:
    case QMetaType::UChar: case QMetaType::SChar:
        openPage(CharPage);
        ui->leChar->setText(_value.toChar());
        break;

    case QMetaType::QString:
        openPage(StringPage);
        ui->pteString->setPlainText(_value.toString());
        break;

    case QMetaType::QByteArray:
        openPage(ByteArrayPage);
        if(_byteArrayType == Utf8Type)
            ui->pteByteArray->setPlainText(QString::fromUtf8(_value.toByteArray()));
        else
            ui->pteByteArray->setPlainText(byteArrayToHex(_value.toByteArray()));
        break;

    case QMetaType::QDate:
        openPage(DatePage);
        ui->deDate->setDate(_value.toDate());
        break;

    case QMetaType::QTime:
        openPage(TimePage);
        ui->teTime->setTime(_value.toTime());
        break;

    case QMetaType::QDateTime:
        openPage(DateTimePage);
        ui->dteDateTime->setDateTime(_value.toDateTime());
        break;
        ui->dsbSizeWidth->setDecimals(0); ui->dsbSizeHeight->setDecimals(0);

    case QMetaType::QSize: case QMetaType::QSizeF:
        openPage(SizePage);
        ui->dsbSizeWidth->setDecimals(decimalCount); ui->dsbSizeHeight->setDecimals(decimalCount);
        ui->dsbSizeWidth->setValue(_value.toSizeF().width());
        ui->dsbSizeHeight->setValue(_value.toSizeF().height());
        break;

    case QMetaType::QRect: case QMetaType::QRectF:
        openPage(RectPage);
        ui->dsbRectX->setDecimals(decimalCount); ui->dsbRectY->setDecimals(decimalCount);
        ui->dsbRectWidth->setDecimals(decimalCount); ui->dsbRectHeight->setDecimals(decimalCount);
        ui->dsbRectX->setValue(_value.toRectF().x());
        ui->dsbRectY->setValue(_value.toRectF().y());
        ui->dsbRectWidth->setValue(_value.toRectF().width());
        ui->dsbRectHeight->setValue(_value.toRectF().height());
        break;

    case QMetaType::QPoint: case QMetaType::QPointF:
        openPage(PointPage);
        ui->dsbPointX->setDecimals(decimalCount); ui->dsbPointY->setDecimals(decimalCount);
        ui->dsbPointX->setValue(_value.toPointF().x());
        ui->dsbPointY->setValue(_value.toPointF().y());
        break;

    case QMetaType::QColor:
        openPage(ColorPage);
        loadColor();
        break;

    case QMetaType::QLocale:
        openPage(LocalePage);
        ui->cbLocale->setCurrentIndex(ui->cbLocale->findData(_value.toLocale().name()));
        break;

    case QMetaType::QStringList:
        openPage(ListPage);
        loadStringList();
        break;

    case QMetaType::QKeySequence:
        openPage(KeySequencePage);
        ui->kseKeySequence->setKeySequence(_value.value<QKeySequence>());
        break;

    default:
        openPage(InvalidPage);
    }

    _blockUpdate = false;
}

void WdgVariantEditor::openPage(const Page &page) {
    ui->stackedWidget->setCurrentIndex(page);
}

void WdgVariantEditor::onByteArrayTypeChanged(const int &type) {
    // 0 = Hex; 1 = UTF-8
    _byteArrayType = static_cast<ByteArrayType>(type);
    loadValue();
}

void WdgVariantEditor::updateValue() {
    if(_blockUpdate)
        return;

    switch(_type.id()) {
    case QMetaType::Bool:
        _value = QVariant::fromValue(ui->cbBool->isChecked());
        break;

    case QMetaType::Int: case QMetaType::Long: case QMetaType::LongLong: case QMetaType::Short:
    case QMetaType::UInt: case QMetaType::ULong: case QMetaType::ULongLong: case QMetaType::UShort:
        _value = QVariant::fromValue(ui->sbInt->value());
        break;

    case QMetaType::Float: case QMetaType::Float16: case QMetaType::Double:
        _value = QVariant::fromValue(ui->dsbFloat->value());
        break;

    case QMetaType::Char: case QMetaType::Char16: case QMetaType::Char32:
    case QMetaType::UChar: case QMetaType::SChar:
        _value = QVariant::fromValue(ui->leChar->text().isEmpty() ? QChar() : QChar(ui->leChar->text().at(0)));
        break;

    case QMetaType::QString:
        _value = QVariant::fromValue<QString>(ui->pteString->toPlainText());
        break;

    case QMetaType::QByteArray:
        openPage(ByteArrayPage);
        if(_byteArrayType == Utf8Type)
            _value = ui->pteByteArray->toPlainText().toUtf8();
        else
            _value = hexToByteArray(ui->pteByteArray->toPlainText());
        break;

    case QMetaType::QDate:
        _value = QVariant::fromValue(ui->deDate->date());
        break;

    case QMetaType::QTime:
        _value = QVariant::fromValue(ui->teTime->time());
        break;

    case QMetaType::QDateTime:
        _value = QVariant::fromValue(ui->dteDateTime->dateTime());
        break;

    case QMetaType::QSize:
        break;
        _value = QVariant::fromValue(QSize(ui->dsbSizeWidth->value(), ui->dsbSizeHeight->value()));

    case QMetaType::QSizeF:
        _value = QVariant::fromValue(QSizeF(ui->dsbSizeHeight->value(), ui->dsbSizeHeight->value()));
        break;

    case QMetaType::QRect:
        _value = QVariant::fromValue(QRect(ui->dsbRectX->value(), ui->dsbRectY->value(), ui->dsbRectWidth->value(), ui->dsbRectHeight->value()));
        break;

    case QMetaType::QRectF:
        _value = QVariant::fromValue(QRectF(ui->dsbRectX->value(), ui->dsbRectY->value(), ui->dsbRectWidth->value(), ui->dsbRectHeight->value()));
        break;

    case QMetaType::QPoint:
        _value = QVariant::fromValue(QPoint(ui->dsbPointX->value(), ui->dsbPointY->value()));
        break;

    case QMetaType::QPointF:
        _value = QVariant::fromValue(QPointF(ui->dsbPointX->value(), ui->dsbPointY->value()));
        break;

    case QMetaType::QLocale:
        _value = QVariant::fromValue(QLocale(ui->cbLocale->currentData().toString()));
        break;

    case QMetaType::QKeySequence:
        _value = QVariant::fromValue(ui->kseKeySequence->keySequence());
        break;

    default: break;
    }

    emit valueChanged(_value, _type);
}

QString WdgVariantEditor::byteArrayToHex(const QByteArray &array) {
    QStringList values;
    for(char byte : array)
        values << QString("%1").arg(static_cast<unsigned char>(byte), 2, 16, QLatin1Char('0'));

    return values.join(" ").toUpper();
}

QByteArray WdgVariantEditor::hexToByteArray(const QString &hex) {
    QString cleaned = hex;
    cleaned.remove(QRegularExpression("\\s"));

    QByteArray result;

    if (cleaned.length() % 2 != 0) {
        qWarning() << "Invalid hex string length! Length must be even. The last character will be ignored";
        cleaned.chop(1);
    }

    for (int i = 0; i < cleaned.length(); i += 2) {
        QString byteStr = cleaned.mid(i, 2);
        bool ok;
        int byteVal = byteStr.toInt(&ok, 16);
        if (ok) {
            result.append(static_cast<char>(byteVal));
        } else {
            qWarning() << "Invalid hex pair: " << qUtf8Printable(byteStr);
            result.append(static_cast<char>(0));
        }
    }

    return result;
}

QVariant WdgVariantEditor::dialogEditor(const QVariant &value, const bool &readOnly, bool *ok, QWidget *parent) {
    QDialog dlg;
    dlg.setWindowTitle(tr("Edit value"));
    dlg.resize(450, 300);

    WdgVariantEditor editor(parent);

    editor.setReadOnly(readOnly);
    editor.setValue(value);

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, &dlg);
    layout->addWidget(&editor);

    QLabel *infoLabel = new QLabel(tr("<strong>Hint:</strong> Use <code>Enter</code> for new line, <code>Ctrl+Enter</code> to finish the dialog"), &dlg);
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    layout->addWidget(buttonBox);

    QAction *acceptAction = dlg.addAction("");
    acceptAction->setShortcut(QKeySequence({Qt::CTRL|Qt::Key_Return}));

    connect(acceptAction, &QAction::triggered, &dlg, &QDialog::accept);

    connect(buttonBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    dlg.setLayout(layout);
    int resultCode = dlg.exec();
    if(ok && resultCode == QDialog::Accepted)
        *ok = true;
    else if(ok)
        *ok = false;

    return editor.value();
}

void WdgVariantEditor::on_pbSelectColor_clicked() {
    _value = QVariant::fromValue(QColorDialog::getColor(_value.value<QColor>()));
    loadColor();
    emit valueChanged(_value, _type);
}

void WdgVariantEditor::onListSelectionChanged() {
    const int selectionCount = ui->lwList->selectionModel()->selectedRows(0).count();

    _listEditAction->setEnabled(selectionCount == 1);
    _listDeleteAction->setEnabled(selectionCount > 0);
    _listUpAction->setEnabled(selectionCount == 1 && ui->lwList->currentRow() > 0);
    _listDownAction->setEnabled(selectionCount == 1 && ui->lwList->currentRow() <= ui->lwList->count());
}

void WdgVariantEditor::onListAdd() {
    QListWidgetItem *currentItem = ui->lwList->currentItem();

    int row = currentItem ? ui->lwList->row(currentItem) : -1;
    int insertRow = row + 1;

    QVariant defaultValue;

    if(_type.id() == QMetaType::QStringList) {
        defaultValue = QString("");
    } else {
        // TODO: Handle QVariant-List
    }

    bool ok;
    QVariant newValue = WdgVariantEditor::dialogEditor(defaultValue, _readOnly, &ok, this);
    if(!ok)
        return;

    if(_type.id() == QMetaType::QStringList) {
        QString string = newValue.toString();
        QStringList stringParts = string.split("\n");
        ui->lwList->insertItem(insertRow, stringParts.first() + (stringParts.count() > 1 ? "..." : ""));
        ui->lwList->selectionModel()->clearSelection();
        ui->lwList->setCurrentRow(insertRow);

        QStringList list = _value.toStringList();
        list.insert(insertRow, string);
        _value = list;

    } else {
        // TODO: Handle QVariant-List
    }

    emit valueChanged(_value, _type);
}

void WdgVariantEditor::onListEdit() {
    QListWidgetItem *currentItem = ui->lwList->currentItem();
    if(!currentItem)
        return;

    int row = ui->lwList->row(currentItem);

    QVariant oldValue;

    if(_type.id() == QMetaType::QStringList) {
        oldValue = _value.toStringList().at(row);
    } else {
        // TODO: Handle QVariant-List
    }

    bool ok;
    QVariant newValue = WdgVariantEditor::dialogEditor(oldValue, _readOnly, &ok, this);
    if(!ok)
        return;

    if(_type.id() == QMetaType::QStringList) {
        QString newString = newValue.toString();
        QStringList newStringParts = newString.split("\n");
        currentItem->setText(newStringParts.first() + (newStringParts.count() > 1 ? "..." : ""));
        QStringList list = _value.toStringList();
        list[row] = newString;
        _value = list;
    } else {
        // TODO: Handle QVariant-List
    }

    emit valueChanged(_value, _type);
}

void WdgVariantEditor::onListDelete() {
    const QList<QListWidgetItem *> items = ui->lwList->selectedItems();

    if(QMessageBox::warning(this, tr("Delete item"), tr("<p><b>Do you really want to delete these %n item(s) from the list?</b></p>", "", items.count()), QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes)
        return;

    if(_type.id() == QMetaType::QStringList) {
        QStringList list = _value.toStringList();

        for(QListWidgetItem *item : items) {
            int row = ui->lwList->row(item);
            list.remove(row);
            delete ui->lwList->takeItem(row);
        }

        _value = list;
    } else {
        // TODO: Handle QVariant-List
    }

    emit valueChanged(_value, _type);
}

void WdgVariantEditor::onListUp() {
    QListWidgetItem *currentItem = ui->lwList->currentItem();
    if(!currentItem)
        return;

    int row = ui->lwList->row(currentItem);
    moveItem(row, -1);
}

void WdgVariantEditor::onListDown() {
    QListWidgetItem *currentItem = ui->lwList->currentItem();
    if(!currentItem)
        return;

    int row = ui->lwList->row(currentItem);
    moveItem(row, 1);
}

void WdgVariantEditor::moveItem(const int &row, const int &offset) {
    QListWidgetItem *item = ui->lwList->item(row);
    const int newRow = row + offset;

    if(newRow < 0 || newRow >= ui->lwList->count())
        return;

    ui->lwList->takeItem(row);
    ui->lwList->insertItem(newRow, item);
    ui->lwList->selectionModel()->clearSelection();
    ui->lwList->setCurrentRow(newRow);

    if(_type.id() == QMetaType::QStringList) {
        QStringList list = _value.toStringList();
        QString string = list.at(row);
        list.remove(row);
        list.insert(newRow, string);
        _value = list;
    } else {
        // TODO: Handle QVariant-List
    }

    emit valueChanged(_value, _type);
}

void WdgVariantEditor::on_lwList_itemDoubleClicked(QListWidgetItem *item) {
    Q_UNUSED(item);
    onListEdit();
}
