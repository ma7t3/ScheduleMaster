#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QGuiApplication>
#include <QScreen>
#include <QAbstractButton>
#include <QLineEdit>

#include "Widgets/FilterPopups/WdgFilterPopup.h"
#include "Widgets/FilterPopups/WdgFilterPopupContent.h"
#include "Widgets/WdgFilterBanner.h"

template<typename FilterPopupType>
class SortFilterProxyModel : public QSortFilterProxyModel {
public:
    explicit SortFilterProxyModel(QAbstractButton *popupButton, QObject *parent = nullptr) :
        QSortFilterProxyModel(parent),
        _filterPopup(popupButton ? new WdgFilterPopup(popupButton) : nullptr),
        _filterPopupContent(_filterPopup ? new FilterPopupType(_filterPopup) : nullptr),
        _filterPopupButton(popupButton), _filterBanner(nullptr), _filterBannerClosed(false),
        _quickSearchEdit(nullptr), _contentWidgetSet(false) {

        if(_filterPopupButton)
            connect(_filterPopupButton,
                    &QAbstractButton::clicked,
                    this,
                    &SortFilterProxyModel::showPopup);

        if(_filterPopupContent)
            connect(_filterPopupContent, &WdgFilterPopupContent::filterChanged, this, [this](){
                invalidateFilter();
                updateFilterBannerVisibility();
            });

        setSortLocaleAware(true);
        setFilterCaseSensitivity(Qt::CaseInsensitive);
    }

    void setFilterBanner(WdgFilterBanner *banner) {
        if(_filterBanner)
            _filterBanner->disconnect(this);

        _filterBanner = banner;
        connect(_filterBanner,
                &WdgFilterBanner::clearFilterRequested,
                this,
                &SortFilterProxyModel::resetFilter);

        connect(_filterBanner, &WdgFilterBanner::closed, this, [this](){
            _filterBannerClosed = true;
        });

        updateFilterBannerVisibility();
    }

    void setQuickSearchEdit(QLineEdit *lineEdit) {
        if(_quickSearchEdit)
            _quickSearchEdit->disconnect(this);

        _quickSearchEdit = lineEdit;
        connect(_quickSearchEdit,
                &QLineEdit::textChanged,
                this,
                [this](const QString &filterString){
                    setFilterWildcard(filterString);
                    updateFilterBannerVisibility();
                });

        updateFilterBannerVisibility();
    }

protected:
    void showPopup() {
        if(!_filterPopupButton || !_filterPopup || !_filterPopupContent)
            return;

        if(!_contentWidgetSet) {
            _filterPopup->setContentWidget(_filterPopupContent);
            _filterPopup->resize(_filterPopupContent->size());
            _contentWidgetSet = true;
        }

        QPoint pos = _filterPopupButton->mapToGlobal(QPoint(0, _filterPopupButton->height()));

        QScreen *screen = QGuiApplication::screenAt(pos);
        if(!screen)
            screen = QGuiApplication::primaryScreen();
        const QRect screenGeometry = screen->availableGeometry();

        const QRect popupRect(pos, _filterPopup->size());

        if(popupRect.right() > screenGeometry.right()) {
            pos.setX(screenGeometry.right() - _filterPopup->width());
        }

        if(popupRect.bottom() > screenGeometry.bottom()) {
            pos.setY(pos.y() - _filterPopup->height() - _filterPopupButton->height());
        }

        _filterPopup->move(pos);
        _filterPopup->show();
    }

    void resetFilter() {
        if(_quickSearchEdit)
            _quickSearchEdit->clear();

        if(_filterPopupContent)
            _filterPopupContent->reset();
    };

    void updateFilterBannerVisibility() {
        if(!_filterBanner)
            return;

        const bool shouldShow = _quickSearchEdit ? (!_quickSearchEdit->text().isEmpty()
                                                    || _filterPopupContent->filterIsEnabled())
                                                 : _filterPopupContent->filterIsEnabled();

        if(shouldShow && !_filterBannerClosed) {
            _filterBanner->show();
        } else if(!shouldShow) {
            _filterBanner->hide();
            _filterBannerClosed = false;
        }
    }

    FilterPopupType *filterPopup() const { return _filterPopupContent; }

private:
    WdgFilterPopup *_filterPopup;
    FilterPopupType *_filterPopupContent;
    QAbstractButton *_filterPopupButton;
    WdgFilterBanner *_filterBanner;
    bool _filterBannerClosed;
    QLineEdit *_quickSearchEdit;
    bool _contentWidgetSet;
};

#endif // SORTFILTERPROXYMODEL_H
