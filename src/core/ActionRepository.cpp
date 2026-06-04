#include "ActionRepository.h"

namespace ScheduleMaster::Core {

ActionRepository::ActionRepository(QObject *parent, const QString &dummy) : GlobalConfigRepositoryCRTP(parent, "Actions") {
    Q_UNUSED(dummy)
}

}