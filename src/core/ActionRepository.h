#ifndef ACTIONREPOSITORY_H
#define ACTIONREPOSITORY_H

#include "GlobalConfigRepository.h"
#include "IActionService.h"

namespace ScheduleMaster::Core {

class ActionRepository : public GlobalConfigRepositoryCRTP<ActionConfig> {
    Q_OBJECT
public:
    ActionRepository(QObject *parent, const QString &dummy);
};

}

#endif // ACTIONREPOSITORY_H
