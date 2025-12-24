#ifndef GLOBALCONFIGSERVICE_H
#define GLOBALCONFIGSERVICE_H

#include <QObject>

#include "Service.h"

class GlobalConfigService : public QObject {
    Q_OBJECT
public:
    explicit GlobalConfigService(QObject *parent = nullptr);

signals:
};

template<typename RepositoryType, typename Derived>
class GlobalConfigServiceCRTP : public GlobalConfigService, public Service<Derived> {
public:
    explicit GlobalConfigServiceCRTP(QObject *parent = nullptr, const QString &resourceName = "") :
        GlobalConfigService(parent), _repository(new RepositoryType(this, resourceName)) {}

    void initRepository() { _repository->init(); }

protected:
    RepositoryType *repository() const { return _repository; }

private:
    RepositoryType *_repository;
};

#endif // GLOBALCONFIGSERVICE_H
