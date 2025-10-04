#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QGuiApplication>
#include <QScreen>
#include <QAbstractButton>
#include <QLineEdit>
#include <QDockWidget>

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
        _filterPopupButton(popupButton), _filterBanner(nullptr), _clearFilterAction(nullptr), _filterBannerClosed(false),
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
                updateClearFilterActionEnabled();
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

    void setClearFilterAction(QAction *action) {
        if(_clearFilterAction)
            _clearFilterAction->disconnect(this);

        _clearFilterAction = action;
        connect(_clearFilterAction, &QAction::triggered, this, [this](){
            if(_filterPopupContent)
                _filterPopupContent->reset();

            if(_quickSearchEdit)
                _quickSearchEdit->clear();
        });

        updateClearFilterActionEnabled();
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
                    updateClearFilterActionEnabled();
                });

        updateFilterBannerVisibility();
        updateClearFilterActionEnabled();
    }

    FilterPopupType *filterPopup() const { return _filterPopupContent; }

protected:
    void showPopup() {
        if(!_filterPopupButton || !_filterPopup || !_filterPopupContent)
            return;

        if(_filterPopup->isVisible())
            return;

        if(!_contentWidgetSet) {
            _filterPopup->setContentWidget(_filterPopupContent);
            _filterPopup->resize(_filterPopupContent->size());
            _contentWidgetSet = true;
        }

        QObject *dockParent = _filterPopupButton;
        while(!dynamic_cast<QDockWidget *>(dockParent))
            dockParent = dockParent->parent();

        const bool dockIsVisible = static_cast<QDockWidget *>(dockParent)->isVisible();
        _filterPopup->setWindowFlags(dockIsVisible ? Qt::Popup : Qt::Tool);

        if(dockIsVisible)
            _filterPopup->showPopup(_filterPopupButton->mapToGlobal(QPoint(0, _filterPopupButton->height())));
        else
            _filterPopup->showTool(QCursor::pos());
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

    void updateClearFilterActionEnabled() {
        if(!_clearFilterAction)
            return;

        _clearFilterAction->setEnabled(
            _quickSearchEdit
                ? (!_quickSearchEdit->text().isEmpty() || _filterPopupContent->filterIsEnabled())
                : _filterPopupContent->filterIsEnabled());
    }

private:
    WdgFilterPopup *_filterPopup;
    FilterPopupType *_filterPopupContent;
    QAbstractButton *_filterPopupButton;
    WdgFilterBanner *_filterBanner;
    QAction *_clearFilterAction;
    bool _filterBannerClosed;
    QLineEdit *_quickSearchEdit;
    bool _contentWidgetSet;
};

#endif // SORTFILTERPROXYMODEL_H
