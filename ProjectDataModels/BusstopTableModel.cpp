#include "BusstopTableModel.h"

#include "ApplicationInterface.h"

#include <QSortFilterProxyModel>

void BusstopTableModelDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const {

    Busstop *b = busstopAtIndex(index);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    switch(index.column()) {
    case 1:
        paintRoleIcons(painter, option, b);
        break;

    case 2:
        paintPlatforms(painter, option, b);
        break;

    case 3:
        paintLines(painter, option, b);
        break;
    }

    painter->restore();
}

QSize BusstopTableModelDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    switch(index.column()) {
    case 1:
        return QSize(80, -1);

    case 2:
        return calculatePlatformsSizeHint(option, busstopAtIndex(index));

    case 3:
        return QSize(); // TODO
    }
    return QSize();
}

void BusstopTableModelDelegate::paintRoleIcons(QPainter *painter,
                                               const QStyleOptionViewItem &option,
                                               Busstop *busstop) const {
    const QRect rect = option.rect;
    const int height = rect.height();

    QStringList iconIDs = iconIDsList(busstop->flags());

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setClipRect(option.rect);

    int x = 2;
    for(const QString &iconID : std::as_const(iconIDs)) {
        painter->drawPixmap(rect.x() + x, rect.y() + 1, IconController::icon(iconID).pixmap(height - 2, height - 2));
        x += height + 2;
    }
}

void BusstopTableModelDelegate::paintPlatforms(QPainter *painter,
                                               const QStyleOptionViewItem &option,
                                               Busstop *busstop) const {

    QList<BusstopPlatform *> platforms = busstop->platforms().values();
    std::sort(platforms.begin(), platforms.end(), [](BusstopPlatform *a, BusstopPlatform *b) {
        return a->name() < b->name();
    });

    painter->setBrush(QColor(128, 128, 128, 64));

    const int boxHeight = option.rect.height();
    const int itemHight = boxHeight - (2 * BoxVerticalPadding);
    const int y         = option.rect.y() + BoxVerticalPadding;

    int x = option.rect.x() + BoxHorizontalPadding;

    for(BusstopPlatform *platform : std::as_const(platforms)) {
        const QString text = platform->name();
        const bool isDefaultPlatform = busstop->isDefaultPlatform(platform);

        QFont f = option.font;
        f.setBold(isDefaultPlatform);
        painter->setFont(f);

        const QFontMetrics fm(f);

        const int textWidth = fm.size(Qt::TextSingleLine, text).width();
        const int itemWidth = textWidth + (2 * ItemHorizontalPadding);

        const QRect itemRect(x, y, itemWidth, itemHight);

        painter->save();
        painter->setPen(Qt::NoPen);
        if(isDefaultPlatform)
            painter->setBrush(Qt::darkGreen);

        painter->drawRoundedRect(itemRect, ItemBorderRadius, ItemBorderRadius);
        painter->restore();
        painter->save();

        if(isDefaultPlatform)
            painter->setPen(Qt::white);

        painter->drawText(itemRect, Qt::AlignCenter, text);
        painter->restore();

        x += itemWidth + ItemGap;
    }
}

void BusstopTableModelDelegate::paintLines(QPainter *painter, const QStyleOptionViewItem &option,
                                           Busstop *busstop) const {

    // TODO
}

QSize BusstopTableModelDelegate::calculatePlatformsSizeHint(const QStyleOptionViewItem &option, Busstop *busstop) const {
    const QList<BusstopPlatform *> platforms = busstop->platforms().values();

    if(platforms.count() == 0)
        return QSize(-1, -1);

    // init base width
    int width = (2 * BoxHorizontalPadding) + ((platforms.count() - 1) * ItemGap);

    for(BusstopPlatform *platform : std::as_const(platforms)) {
        const QString text = platform->name();
        const bool isDefaultPlatform = busstop->isDefaultPlatform(platform);

        QFont f = option.font;
        f.setBold(isDefaultPlatform);
        const QFontMetrics fm(f);
        const int textWidth = fm.size(Qt::TextSingleLine, text).width();

        width += (textWidth + (2 * ItemHorizontalPadding));
    }

    return QSize(width, -1);
}

Busstop *BusstopTableModelDelegate::busstopAtIndex(const QModelIndex &index) const {
    const QSortFilterProxyModel *proxy = qobject_cast<const QSortFilterProxyModel *>(index.model());
    QModelIndex sourceIndex = proxy->mapToSource(index);
    return static_cast<Busstop *>(sourceIndex.internalPointer());
}

QStringList BusstopTableModelDelegate::iconIDsList(const BusstopFlags &flags) {
    QStringList iconIDs;

    if(flags.testFlag(BusstopFlag::FinalBusstop))
        iconIDs << "busstop-terminus";

    if(flags.testFlag(BusstopFlag::TransferBusstop))
        iconIDs << "busstop-transfer";

    if(flags.testFlag(BusstopFlag::CentralTransferBusstop))
        iconIDs << "busstop-transfer-central";

    if(flags.testFlag(BusstopFlag::InternalBusstop))
        iconIDs << "gear";
    return iconIDs;
}

BusstopTableModel::BusstopTableModel(QObject *parent) :
    UnorderedProjectDataRowModel(parent), _projectData(ApplicationInterface::projectData()) {
    connect(_projectData, &ProjectData::cleared,  this, &BusstopTableModel::reset);

    connect(_projectData, &ProjectData::busstopAdded, this, &BusstopTableModel::onItemAdded);
    connect(_projectData, &ProjectData::busstopRemoved, this, &BusstopTableModel::onItemRemoved);
}

int BusstopTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 4;
}

QVariant BusstopTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch(section) {
    case 0:
        return tr("Name");
    case 1:
        return tr("Roles");
    case 2:
        return tr("Platforms");
    case 3:
        return tr("Lines");
    }

    return QVariant();
}

QVariant BusstopTableModel::data(const QModelIndex &index, int role) const {
    if(role != Qt::DisplayRole && role != Qt::FontRole)
        return QVariant();

    Busstop *b = itemAt(index.row());

    if(role == Qt::FontRole
       && b->flags().testAnyFlags(BusstopFlag::FinalBusstop | BusstopFlag::TransferBusstop
                                  | BusstopFlag::CentralTransferBusstop)) {
        QFont f;
        f.setBold(true);
        return f;
    } else if(role == Qt::FontRole)
        return QVariant();

    switch(index.column()) {
    case 0:
        return b->name();
    case 2:
        return platformsToString(b);
    }
    return QVariant();
}

PDISet<Busstop> BusstopTableModel::fetch() const {
    return _projectData->busstops();
}

QString BusstopTableModel::platformsToString(Busstop *b) {
    QStringList platformStrings;
    const PDISet<BusstopPlatform> platforms = b->platforms();

    for(BusstopPlatform *platform : platforms)
        platformStrings << platform->name();

    platformStrings.sort();
    return platformStrings.join(", ");
}

QString BusstopTableModel::flagsToString(const BusstopFlags &flags) {
    if(flags.testFlag(BusstopFlag::StandardBusstop))
        return "";

    QStringList strings;
    if(flags.testFlag(BusstopFlag::FinalBusstop))
        strings << tr("Final");

    if(flags.testFlag(BusstopFlag::TransferBusstop))
        strings << tr("Transfer");

    if(flags.testFlag(BusstopFlag::CentralTransferBusstop))
        strings << tr("Central Transfer");

    if(flags.testFlag(BusstopFlag::InternalBusstop))
        strings << tr("Internal");

    return strings.join(", ");
}
