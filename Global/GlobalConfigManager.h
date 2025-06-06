#ifndef GLOBALCONFIGMANAGER_H
#define GLOBALCONFIGMANAGER_H

#include "Global/GlobalConfigManagerSignals.h"

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QFile>

struct GlobalConfigItem {
public:
    GlobalConfigItem(const QString &id, const int &index = 0) : _id(id), _index(index) {}
    GlobalConfigItem(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0) : _index(index) {
        _id = jsonObject.value("id").toString();
    }

    bool operator==(const GlobalConfigItem &other) const {
        return id() == other.id();
    }

    QString id() const {
        return _id;
    }

    int index() const {
        return _index;
    }

private:
    QString _id;
    int _index;
};

template <typename Derived, typename T>
class GlobalConfigManager : public GlobalConfigManagerSignals {
protected:
    explicit GlobalConfigManager(QObject *parent) : GlobalConfigManagerSignals(parent) {}

public:

    /**
     * @brief Returns the derived signleton instance
     * @return The singleton instance
     */
    static Derived *instance() {
        static Derived instance(nullptr);
        return &instance;
    };

    /**
     * @brief Initializes the instance
     */
    static void init() {
        qDebug() << "Initializing GlobalConfigManager for" << typeid(T).name();
        instance(); // TODO: Remove
    }

    /**
     * @brief Returns a list of all items inside the handler.
     * @return The item list
     */
    static QList<T> items() { return _items.values(); }

    /**
     * @brief Returns a map of all items inside the handler using the item IDs as keys.
     * @return The item map
     */
    static QMap<QString, T> itemsMap() { return _items; }

    /**
     * @brief Returns a list of all item IDs inside the handler
     * @return
     */
    static QStringList itemIDs() { return _items.keys();}

    /**
     * @brief Checks if an item specified by the given ID exists inside the handler.
     * @param id The item ID to check for
     * @return Whether the item exists or not
     */
    static bool itemExists(const QString &id) { return _items.contains(id); }

    /**
     * @brief Returns the item specified by the given ID.
     * @param id The ID of the item to find
     * @return The item or a default constructed value (or nullptr for pointer types) if the item doesn't exist.
     */
    static T item(const QString &id) {
        if constexpr(std::is_pointer<T>::value)
            return _items.value(id, nullptr);
        else
            return _items.value(id, T());
    }

    /**
     * @brief Adds a new item to the handler.
     *
     * Invalid items or items with an invalid/empty ID won't be added.
     *
     * If the Item is a QObject-Pointer, the handler takes ownership of the item.
     * @param item The item to add
     * @return Whether the item was successfully added to the handler
     */
    static bool addItem(T item) {
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
                item->setParent(instance());

            const QString id = item->id();

            _items.insert(id, item);
            if(_initializied)
                emit instance()->itemAdded(id);
            qDebug() << "Item added:" << id << item->index();
        } else {
            if(item.id().isEmpty()) {
                qWarning() << "Didn't add item. No valid ID found!";
                return false;
            }

            const QString id = item.id();

            _items.insert(id, item);
            if(_initializied)
                emit instance()->itemAdded(id);
            qDebug() << "Item added:" << id << item.index();
        }
        return true;
    }

    /**
     * @brief Removes the item specified by the given id from the container and returns it.
     *
     * This function does nothing and returns a default constructed value (or nullptr for poitners) if the given item id doesn't exist.
     *
     *If the item is a QObject-pinter, the handler relinquishes the ownership of the item. You should take or detlete it.
     * @param id The id of the item to remove
     * @return The removed item.
     */
    static T removeItem(const QString &id) {
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
        emit instance()->GlobalConfigManagerSignals::itemRemoved(id);
        qDebug().noquote() << "Item removed:" << id;
        return item;
    }


    GlobalConfigManager(const GlobalConfigManager &) = delete;
    GlobalConfigManager &operator=(const GlobalConfigManager &) = delete;

protected:
    /**
     * @brief Loads all items of the resource directly into the _items.
     * @param resourceID The resource ID to read the items from.
     */
    static void loadItems(const QString &resourceID) {
        qInfo().noquote() << "Loading" << resourceID;
        const QJsonArray jsonData = loadMultiConfigResource(resourceID);
        int index = 0;
        for(const QJsonValue &val : jsonData) {
            const QJsonObject obj = val.toObject();
            if constexpr(std::is_pointer<T>::value) {
                T t = new T(obj, index);
                if(t->id().isEmpty())
                    continue;

                qInfo() << t->id();
                _items.insert(t->id(), t);
            } else {
                T t(obj, index);
                if(t.id().isEmpty())
                    continue;

                qInfo() << t.id();
                addItem(t);
            }

            index++;
        }
    }

    /**
     * @brief Loads a single config resource file and returns the parsed JSON document.
     *
     * This always automatically resolves translated strings.
     * @param resource The resource name (without the .json extension)
     * @return The parsed JSON document or a null document if the parsing fails
     */
    static QJsonDocument loadSingleConfigResource(const QString &resource) {
        return parseJsonFile(":/Config/" + resource + ".json");
    }

    /**
     * @brief Loads a multi config resource file and returns a JSON array containing all parsed items.
     *
     * This always automatically resolves translated strings.
     * @param resource The resource/directory name
     * @return A JSON array containing all parsed items.
     */
    static QJsonArray loadMultiConfigResource(const QString &resource) {
        QJsonArray data;

        const QDir dir(":/Config/" + resource);
        const QStringList entrys = dir.entryList();
        for(const QString &entry : entrys) {
            const QJsonDocument doc = parseJsonFile(dir.path() + "/" + entry);
            if(doc.isNull())
                continue;

            const QJsonArray array = doc.array();
            for(const QJsonValue &val : array)
                data.append(val);
        }

        return data;
    }

    /**
     * @brief Reads a JSON file and returns the parsed JSON document.
     *
     * It automatically resolves translated strings.
     * @param fileName The file name
     * @return The parsed JSON document or a null document if the parsing fails
     */
    static QJsonDocument parseJsonFile(const QString &fileName) {
        QFile f(fileName);
        if(!f.open(QIODevice::ReadOnly)) {
            qWarning().noquote() << "Error while reading resource configuration file: \"" + fileName + "\" (" + f.errorString() + ")";
            return QJsonDocument();
        }

        QJsonParseError error;
        const QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &error);
        f.close();

        if(error.error != QJsonParseError::NoError) {
            qWarning().noquote() << "Error while parsing resource configuration file: \"" + fileName + "\" (" + error.errorString() + ")";
            return QJsonDocument();
        }

        if(doc.isArray())
            return QJsonDocument(resolveTranslatedStrings(doc.array()));
        else if(doc.isObject())
            return QJsonDocument(resolveTranslatedStrings(doc.object()).toObject());
        else {
            qWarning().noquote() << "Error while loading resource configuration file: \"" + fileName + "\"";
            return QJsonDocument();
        }
    }

    /**
     * @brief Resolves all translated strings inside the JSON object.
     *
     * The return value may still be an object or a string if the given object was a string-translate object which was resolved to a string.
     *
     * This method is executed recursive.
     * @param jsonObject The JSON object to resolve
     * @return The resolved JSON value.
     */
    static QJsonValue resolveTranslatedStrings(QJsonObject jsonObject) {
        if(jsonObject.value("object").toString() == "translated_string") {
            const QLocale l;
            const QString localeName = l.name();

            QString resultString;

            if(jsonObject.contains(localeName))
                resultString = jsonObject.value(localeName).toString();
            else if(jsonObject.contains("en_US"))
                resultString = jsonObject.value("en_US").toString();

            return resultString;
        }

        const QStringList keys = jsonObject.keys();
        for(const QString &key : keys) {
            QJsonValue value = jsonObject.value(key);

            if(value.isObject())
                jsonObject.insert(key, resolveTranslatedStrings(value.toObject()));
            else if(value.isArray())
                jsonObject.insert(key, resolveTranslatedStrings(value.toArray()));
        }

        return jsonObject;
    }

    /**
     * @brief Resolves all translated strings inside the JSON array.
     *
     * This method is executed recursive.
     * @param jsonArray The JSON array to resolve
     * @return The resolved JSON array
     */
    static QJsonArray resolveTranslatedStrings(QJsonArray jsonArray) {
        for(int i = 0; i < jsonArray.count(); i++) {
            const QJsonValue value = jsonArray[i];

            if(value.isObject())
                jsonArray.replace(i, resolveTranslatedStrings(value.toObject()));
            else if(value.isArray())
                jsonArray.replace(i, resolveTranslatedStrings(value.toArray()));
        }

        return jsonArray;
    }

private:
    /// The items inside the handler
    static inline QMap<QString, T> _items;


    static inline bool _initializied = false;
};

#endif // GLOBALCONFIGMANAGER_H
