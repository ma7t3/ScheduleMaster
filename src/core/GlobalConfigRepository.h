#ifndef GLOBALCONFIGREPOSITORY_H
#define GLOBALCONFIGREPOSITORY_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

namespace ScheduleMaster::Core {

struct GlobalConfigItem {
public:
    GlobalConfigItem(const QString &id, const int &index = 0) : _id(id), _index(index) {}
    GlobalConfigItem(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0);

    bool operator==(const GlobalConfigItem &other) const;
    QString id() const;
    int index() const;

private:
    QString _id;
    int _index;
};

class GlobalConfigRepository : public QObject {
    Q_OBJECT
public:
    explicit GlobalConfigRepository(QObject *parent = nullptr);

protected:
    static QJsonDocument loadSingleConfigResource(const QString &resource);
    static QJsonArray    loadMultiConfigResource(const QString &resource);
    static QJsonDocument parseJsonFile(const QString &fileName);
    static QJsonValue    resolveTranslatedStrings(QJsonObject jsonObject);
    static QJsonArray    resolveTranslatedStrings(QJsonArray jsonArray);

signals:
    void itemAdded(const QString &id);
    void itemRemoved(const QString &id);
};

template <typename T, typename Derived>
class GlobalConfigRepositoryCRTP : public GlobalConfigRepository {
protected:
    explicit GlobalConfigRepositoryCRTP(QObject *parent) : GlobalConfigRepository(parent) {}

public:
    QList<T> items() const { return _items.values(); }

    QList<T> sortedItems() const {
        QList<T> list = _items.values();
        std::sort(list.begin(), list.end(), [](const T &a, const T &b) {
            return a.index() < b.index();
        });
        return list;
    }

    QMap<QString, T> itemsMap() const { return _items; }

    QStringList itemIDs() const { return _items.keys();}

    bool itemExists(const QString &id) const { return _items.contains(id); }

    T item(const QString &id) const {
        if constexpr(std::is_pointer<T>::value)
            return _items.value(id, nullptr);
        else
            return _items.value(id, T());
    }

    bool addItem(T item) {
        if constexpr(std::is_pointer<T>::value) {
            if(!item) {
                qWarning() << "Didn't add item. Item is nullptr!";
                return false;
            }

            if(item->id().isEmpty()) {
                qWarning() << "Didn't add item. No valid ID found!";
                return false;
            }

            if constexpr(std::is_base_of<QObject, std::remove_pointer<T>>::value)
                item->setParent(this);

            const QString id = item->id();

            _items.insert(id, item);
            emit itemAdded(id);
            qDebug() << "Item added:" << id;
        } else {
            if(item.id().isEmpty()) {
                qWarning() << "Didn't add item. No valid ID found!";
                return false;
            }

            const QString id = item.id();

            _items.insert(id, item);
            emit itemAdded(id);
            qDebug() << "Item added:" << id;
        }
        return true;
    }

    T removeItem(const QString &id) {
        if(!_items.contains(id)) {
            qWarning() << "Cannot remove item" << id << "from the handler because it doesn' exist.";
            if(std::is_pointer<T>::value)
                return nullptr;
            else
                return T();
        }

        T item = _items.value(id);

        if constexpr(std::is_base_of<QObject, std::remove_pointer<T>>::value)
            item->setParent(nullptr);

        _items.remove(id);
        emit itemRemoved(id);
        qDebug().noquote() << "Item removed:" << id;
        return item;
    }

protected:
    void loadItems(const QString &resourceID) {
        qInfo().noquote() << "Loading" << resourceID;
        const QJsonArray jsonData = loadMultiConfigResource(resourceID);
        int index = 0;
        for(const QJsonValue &val : jsonData) {
            const QJsonObject obj = val.toObject();
            if constexpr(std::is_pointer<T>::value) {
                T t = new T(obj, index);
                if(t->id().isEmpty())
                    continue;

                _items.insert(t->id(), t);
            } else {
                T t(obj, index);
                if(t.id().isEmpty())
                    continue;

                addItem(t);
            }

            index++;
        }
    }

    QMap<QString, T> _items;
};

}

#endif // GLOBALCONFIGREPOSITORY_H
