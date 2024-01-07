#ifndef LINEDIRECTION_H
#define LINEDIRECTION_H

#include "ProjectData/ProjectDataItem.h"

// ABGESCHLOSSEN!

class LineDirection : public virtual ProjectDataItem {
    Q_OBJECT
public:
    LineDirection(const QString &id, const QString &description);
    LineDirection(const LineDirection &);
    LineDirection operator=(const LineDirection &);

    QString description() const;
    void setDescription(const QString &);

protected:
    void copy(const LineDirection &);

private:
    QString _description;
};

#endif // LINEDIRECTION_H
