#ifndef SINGLETON_H
#define SINGLETON_H

#include <QObject>

/**
 * @brief The Singleton class template provides a way to implement the singleton design pattern.
 * 
 * This template class ensures that only one instance of the Derived class exists.
 * It inherits from QObject, allowing the derived class to utilize Qt's signal/slot mechanism.
 * 
 * @tparam Derived The class that should be a singleton.
 */
template <typename Derived>
class Singleton : public QObject {
    Q_DISABLE_COPY_MOVE(Singleton)

protected:
    /**
     * @brief Protected constructor to prevent direct instantiation.
     * 
     * The constructor is protected to prevent creating instances of the Singleton class directly.
     */
    Singleton() : QObject() {}

public:
    /**
     * @brief Provides access to the singleton instance of the Derived class.
     * 
     * This function creates a static instance of the Derived class the first time it is called,
     * and returns the same instance on subsequent calls.
     * 
     * @return A pointer to the singleton instance of the Derived class.
     */
    static Derived *instance() {
        static Derived instance;
        return &instance;
    }
};

#endif // SINGLETON_H
