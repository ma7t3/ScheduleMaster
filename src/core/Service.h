#ifndef SERVICE_H
#define SERVICE_H

template<typename Derived>
class Service {
protected:
    explicit Service() { _instance = static_cast<Derived *>(this); }

public:
    static Derived *instance() { return _instance; }

private:
    static inline Derived *_instance = nullptr;
};

#endif // SERVICE_H
