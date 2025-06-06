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

    /**
     * @brief Checks if the singleton instance has been initialized.
     *
     * This function can be used to check if the singleton instance has been initialized.
     * It is useful when the singleton instance needs to be initialized explicitly.
     *
     * @return true if the singleton instance has been initialized, false otherwise.
     */
    static bool initialized() { return _init; }

protected:
    /**
     * @brief Initializes the singleton instance.
     *
     * This function is used to initialize the singleton instance.
     * It is useful when the singleton instance needs to be initialized explicitly.
     */
    static void init() { _init = true; }

private:
    /**
     * @brief A boolean flag indicating whether the singleton instance has been initialized.
     */
    static inline bool _init = false;
};

#endif // SINGLETON_H
