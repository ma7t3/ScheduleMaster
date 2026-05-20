#include "helpers.h"

namespace ScheduleMaster {

QKeySequence parseKeyboardShortcutConfigString(const QJsonValue &value) {
    QStringList values;
    QKeySequence resultSequence;

    if(value.isArray())
        values = value.toVariant().toStringList();
    else
        values = {value.toString()};

    for(const QString & value : std::as_const(values)) {
        if(value.startsWith("standard:")) {
            QString idPart = value.mid(QString("standard:").length());
            bool ok;
            int standardID = idPart.toInt(&ok);
            if (ok) {
                QKeySequence sequence = QKeySequence(static_cast<QKeySequence::StandardKey>(standardID));
                if(!sequence.toString(QKeySequence::PortableText).isEmpty()) {
                    return sequence;
                } else {
                    resultSequence = QKeySequence();
                }
            }
        }

        QKeySequence sequence(value);
        if (!sequence.toString(QKeySequence::PortableText).isEmpty()) {
            return sequence;
        } else
            resultSequence = QKeySequence();
    }

    return resultSequence;
}

}
