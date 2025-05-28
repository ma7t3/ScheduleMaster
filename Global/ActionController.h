#ifndef ACTIONCONTROLLER_H
#define ACTIONCONTROLLER_H

#include <QObject>
#include <QAction>
#include <QCommandLinkButton>
#include <QToolButton>

class ActionController : public QObject {
    Q_OBJECT
private:
    explicit ActionController(QObject *parent);

public:
    static void init();
    static ActionController *instance();

    ActionController& operator=(const ActionController&) = delete;
    ActionController(const ActionController&) = delete;

    static QAction *createAction(const QString &actionID, QObject *parent = nullptr);
    static QPushButton *createPushButton(const QString &actionID, QWidget *parent = nullptr);
    static QToolButton *createToolButton(const QString &actionID, QWidget *parent = nullptr);
    static QCommandLinkButton *createCommandLinkButton(const QString &actionID, QWidget *parent = nullptr);

    template <typename T>
    static T *add(T *widget, const QString &actionID);

    template <typename T>
    static T *remove(T *widget);

protected slots:
    void onIconSetChanged();
    void onActionShortcutChanged(const QString &keyboardShortcutID, const QKeySequence shortcut);

signals:

private:
    static inline QHash<QAction *, QString> _actions;
    static inline QHash<QAbstractButton *, QString> _buttons;
};

#endif // ACTIONCONTROLLER_H
