#ifndef CMDGENERAL_H
#define CMDGENERAL_H

#include <QUndoCommand>

class Command : public QUndoCommand {
public:
    Command(const QString &text) : QUndoCommand(text, nullptr) {}
};

#endif // CMDGENERAL_H
