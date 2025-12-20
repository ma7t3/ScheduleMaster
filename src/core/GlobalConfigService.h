#ifndef GLOBALCONFIGSERVICE_H
#define GLOBALCONFIGSERVICE_H

#include <QObject>

class GlobalConfigService : public QObject {
    Q_OBJECT
public:
    explicit GlobalConfigService(QObject *parent = nullptr);

signals:
};

template<typename RepositoryType>
class GlobalConfigServiceCRTP : public GlobalConfigService {
public:
    explicit GlobalConfigServiceCRTP(QObject *parent = nullptr, const QString &resourceName = "") :
        GlobalConfigService(parent), _repository(new RepositoryType(this, resourceName)) {}

protected:
    RepositoryType *repository() const { return _repository; }

private:
    RepositoryType *_repository;
};

#endif // GLOBALCONFIGSERVICE_H
