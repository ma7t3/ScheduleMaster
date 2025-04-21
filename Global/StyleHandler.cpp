#include "StyleHandler.h"

StyleHandler::StyleHandler() :
    QObject(nullptr) {}

StyleHandler::~StyleHandler() {}

StyleHandler *StyleHandler::instance() {
    static StyleHandler instance;
    return &instance;
}

void StyleHandler::init() {
    QStringList keys = QStyleFactory::keys();

    // when making ScheduleMaster available for other operating systems, this needs to be more platform-specific
    _systemStyleName = keys.contains("windowsvista") ? "windowsvista" : !keys.isEmpty() ? keys.first() : "";
}

void StyleHandler::registerStyleClass(const QString &id, QStyle *style) {
    _styleClasses.insert(id, style);
}

void StyleHandler::applyStyle(const QString &id) {
    if(!GlobalConfig::styleExists(id)) {
        qWarning().noquote() << "Cannot apply style " << id << " because it wasn't found.";
        return;
    }

    Style style = GlobalConfig::style(id);

    switch(style.type) {
    case Style::StyleClassType:
        // TODO
        break;
    case Style::StyleSheetType:
        // TODO
        break;
    case Style::StyleFactoryType:
        // TODO
        break;

    case Style::SystemDefaultType:
        // TODO
        break;
    case Style::InvalidType: break;
    }
}

void StyleHandler::applyAccentColor(const QString &id) {
    // TODO
}
