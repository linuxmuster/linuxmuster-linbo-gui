/****************************************************************************
 ** Modern Linbo GUI
 ** Copyright (C) 2020-2021  Dorian Zedler <dorian@itsblue.de>
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Affero General Public License as published
 ** by the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Affero General Public License for more details.
 **
 ** You should have received a copy of the GNU Affero General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#include "linbodialog.h"

LinboDialog::LinboDialog(QWidget* parent) : QWidget(parent)
{
    this->_busy = false;
    this->_firstChild = nullptr;
    this->setWindowFlags(Qt::Widget);
    this->raise();
    this->setVisible(false);

    this->_initColors();
    this->_initModalWidget();
    this->_initOpacityEffectForWidget(this);
    this->_initToolbar();
    this->_initBottomToolbar();
}

void LinboDialog::_initColors() {
    QPalette pal = palette();
    pal.setColor(QPalette::Window, gTheme->color(LinboTheme::BackgroundColor));
    this->setAutoFillBackground(true);
    this->setPalette(pal);
    this->setStyleSheet(gTheme->insertValues("QLabel{color: %TextColor;}"));
}

void LinboDialog::_initModalWidget() {
    this->_modalOverlayWidget = new ModalOverlay(this->parentWidget());
    this->_modalOverlayWidget->setVisible(false);
    this->_modalOverlayWidget->stackUnder(this);
    connect(this->_modalOverlayWidget, &ModalOverlay::clicked, this, &LinboDialog::autoClose);
    connect(this->_modalOverlayWidget, &ModalOverlay::clicked, this, &LinboDialog::closedByUser);
}

void LinboDialog::_initOpacityEffectForWidget(QWidget* widget) {
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(widget);
    effect->setOpacity(0);
    effect->setEnabled(false);
    widget->setGraphicsEffect(effect);

    QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(200);
    animation->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
    connect(animation, &QPropertyAnimation::finished, this, &LinboDialog::_handleAnimationFinished);

    this->_animatedWidgets.append(_AnimatedWidget{true, widget, effect, animation});
}

void LinboDialog::_initToolbar() {
    this->_titleLabel = new QLabel(this->objectName());
    this->_titleLabel->setAlignment(Qt::AlignCenter);
    this->_closeButton = new LinboToolButton(LinboTheme::CancelIcon);
    connect(this->_closeButton, &LinboToolButton::clicked, this, &LinboDialog::autoClose);
    connect(this->_closeButton, &LinboToolButton::clicked, this, &LinboDialog::closedByUser);

    this->_toolBarWidget = new QWidget(this->parentWidget());
    this->_toolBarWidget->setAutoFillBackground(true);
    this->_toolBarWidget->setPalette(this->palette());
    this->_toolBarWidget->setStyleSheet(gTheme->insertValues("QLabel { color: %TextColor;} "));
    this->_toolBarWidget->hide();
    this->_initOpacityEffectForWidget(this->_toolBarWidget);

    this->_toolBarLayout = new QHBoxLayout(this->_toolBarWidget);
    this->_toolBarLayout->setContentsMargins(0,0,0,0);
    this->_toolBarLayout->addWidget(this->_titleLabel);
    this->_toolBarLayout->setAlignment(this->_titleLabel, Qt::AlignCenter);
    this->_toolBarLayout->addStretch();
    this->_toolBarLayout->addWidget(this->_closeButton);
    this->_toolBarLayout->setAlignment(this->_closeButton, Qt::AlignCenter);
}

void LinboDialog::_initBottomToolbar() {
    this->_bottomToolBarWidget = new QWidget(this->parentWidget());
    this->_bottomToolBarWidget->setAutoFillBackground(true);
    this->_bottomToolBarWidget->setPalette(this->palette());
    this->_bottomToolBarWidget->setStyleSheet(gTheme->insertValues("QLabel { color: %TextColor;} "));
    this->_bottomToolBarWidget->hide();
    this->_initOpacityEffectForWidget(this->_bottomToolBarWidget);

    this->_bottomToolBarLayout = new QHBoxLayout(this->_bottomToolBarWidget);
    this->_bottomToolBarLayout->setContentsMargins(0,0,0,0);
}

void LinboDialog::open() {
    this->setVisibleAnimated(true);
    emit this->opened();
}

void LinboDialog::close() {
    this->setVisibleAnimated(false);
}

void LinboDialog::autoClose() {
    this->close();
}

void LinboDialog::addToolButton(LinboToolButton* toolButton) {
    this->_toolButtons.append(toolButton);
    this->_bottomToolBarLayout->addWidget(toolButton);
    this->repaint();
}

void LinboDialog::setTitle(QString title) {
    this->_title = title;
    this->_titleLabel->setText(title);
}

QString LinboDialog::title() {
    return this->_title;
}

void LinboDialog::resizeEvent(QResizeEvent *e) {
    QWidget::resizeEvent(e);

    int rowHeight = gTheme->size(LinboTheme::RowHeight);
    int margins = gTheme->size(LinboTheme::Margins);
    int toolBarHeight = rowHeight + margins ;

    this->_resizeToolBar(rowHeight, margins, toolBarHeight);
    this->_resizeBottomToolBar(rowHeight, margins, toolBarHeight);
}

void LinboDialog::_resizeToolBar(int rowHeight, int margins, int toolBarHeight) {
    this->_toolBarWidget->setGeometry(this->geometry().x(), this->geometry().y() - toolBarHeight, this->geometry().width(), toolBarHeight);
    this->_toolBarLayout->setContentsMargins(margins, margins * 0.5, margins * 0.5, 0);

    QFont titleFont = this->_titleLabel->font();
    titleFont.setPixelSize(gTheme->size(LinboTheme::RowFontSize) * 1.5);
    this->_titleLabel->setFont(titleFont);
    this->_titleLabel->setFixedHeight(rowHeight);
    this->_titleLabel->setFixedWidth(this->width() - margins);

    this->_closeButton->setFixedHeight(rowHeight);
    this->_closeButton->setFixedWidth(rowHeight);

    if(this->_toolBarWidget->isEnabled()) {
        QMargins contentMargins = this->contentsMargins();
        contentMargins.setTop(0);
        this->setContentsMargins(contentMargins);
    }
}

void LinboDialog::_resizeBottomToolBar(int rowHeight, int margins, int toolBarHeight) {
    Q_UNUSED(rowHeight)
    this->_bottomToolBarWidget->setGeometry(this->geometry().x(), this->geometry().y() + this->height(), this->geometry().width(), toolBarHeight);
    this->_bottomToolBarLayout->setSpacing(margins);
    this->_bottomToolBarLayout->setContentsMargins(margins, 0, margins, margins);

    for(LinboToolButton* toolButton : this->_toolButtons)
        toolButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

void LinboDialog::paintEvent(QPaintEvent *e) {
    this->_updateTabOrder();
    return QWidget::paintEvent(e);
}

void LinboDialog::_updateTabOrder() {
    WidgetPair pair{nullptr, nullptr};
    pair = this->_updateTabOrderChildren(pair);
    pair = this->_updateTabOrderBottomToolBar(pair);
    this->_closeTabOrderCircle(pair);
}

LinboDialog::WidgetPair LinboDialog::_updateTabOrderChildren(WidgetPair currentPair) {
    QWidgetList widgets = this->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);

    for(QWidget* widget : widgets) {
        currentPair = this->_updateWidgetTabOrder(widget, currentPair);
    }

    this->_firstChild = currentPair.first;
    if(this->_firstChild == nullptr)
        this->_firstChild = this->_closeButton;

    return currentPair;
}

LinboDialog::WidgetPair LinboDialog::_updateTabOrderBottomToolBar(WidgetPair currentPair) {
    for(LinboToolButton* widget: this->_toolButtons) {
        currentPair = this->_updateWidgetTabOrder(widget, currentPair);
    }

    if(currentPair.second != nullptr)
        QWidget::setTabOrder(currentPair.second, this->_closeButton);

    return currentPair;
}

void LinboDialog::_closeTabOrderCircle(WidgetPair currentPair) {
    // This is a hack to create a cricle for tabbing inside a dialog
    // This does not work for the first child, though. So there is still a hole in the circle
    connect(this->_closeButton, &LinboPushButton::defocused, this, [=](Qt::FocusReason reason) {
        if(reason == Qt::TabFocusReason)
            currentPair.first->setFocus();
        else if(reason == Qt::BacktabFocusReason)
            currentPair.second->setFocus();
    });
}

LinboDialog::WidgetPair LinboDialog::_updateWidgetTabOrder(QWidget* widget, WidgetPair currentPair) {
    if(widget->focusPolicy() == Qt::NoFocus)
        return currentPair;

    if(currentPair.second != nullptr)
        QWidget::setTabOrder(currentPair.second, widget);

    if(currentPair.first == nullptr)
        currentPair.first = widget;

    currentPair.second = widget;
    return currentPair;
}

void LinboDialog::keyPressEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_Escape)
        this->autoClose();
}

void LinboDialog::setVisibleAnimated(bool visible) {
    if(this->isVisible() == visible || this->_busy)
        return;

    this->_busy = true;
    this->_modalOverlayWidget->setVisibleAnimated(visible);

    this->_updateToolbarsEnabled();
    this->_setAllAnimatedWidgetsVisible(visible);
}

void LinboDialog::_updateToolbarsEnabled() {
    this->_toolBarWidget->setEnabled(!this->isFrameless());
    this->_bottomToolBarWidget->setEnabled(this->_toolButtons.length() > 0);
}

void LinboDialog::_setAllAnimatedWidgetsVisible(bool visible) {
    for(const _AnimatedWidget& widget : this->_animatedWidgets) {
        if(widget.widget->isEnabled())
            this->_setAnimatedWidgetVisible(visible, widget);
    }
}

void LinboDialog::_setAnimatedWidgetVisible(bool visible, _AnimatedWidget widget) {
    if(visible)
        this->_showAnimatedWidget(widget);
    else
        this->_hideAnimatedWidget(widget);
}

void LinboDialog::_hideAnimatedWidget(_AnimatedWidget widget) {
    widget.effect->setEnabled(true);
    widget.animation->setStartValue(1);
    widget.animation->setEndValue(0);
    widget.animation->start();
}

void LinboDialog::_showAnimatedWidget(_AnimatedWidget widget) {
    widget.widget->show();
    widget.effect->setEnabled(true);
    widget.animation->setStartValue(0);
    widget.animation->setEndValue(1);
    widget.animation->start();
}


LinboDialog::_AnimatedWidget LinboDialog::_findAnimatedWidget(QPropertyAnimation* animation) {
    for(const _AnimatedWidget& widget : this->_animatedWidgets) {
        if(widget.animation == animation)
            return widget;
    }
    return _AnimatedWidget{false, nullptr, nullptr, nullptr};
}

void LinboDialog::_handleAnimationFinished() {
    QPropertyAnimation* animation = (QPropertyAnimation*)this->sender();
    _AnimatedWidget widget = this->_findAnimatedWidget(animation);
    if(!widget.isValid)
        return;

    if(widget.effect->opacity() == 0)
        widget.widget->hide();

    widget.effect->setEnabled(false);
    this->_busy = false;

    if(this->_firstChild != nullptr)
        this->_firstChild->setFocus();
}

bool LinboDialog::isFrameless() {
    return (this->windowFlags() & Qt::FramelessWindowHint) == Qt::FramelessWindowHint;
}

void LinboDialog::centerInParent() {
    int width = this->width();
    int extraHeight = 0;

    if(!this->isFrameless())
        extraHeight = this->_toolBarWidget->height();

    if(this->_toolButtons.length() > 0)
        extraHeight += this->_bottomToolBarWidget->height();

    this->move((this->parentWidget()->width() - width) / 2, (this->parentWidget()->height() - this->height() - extraHeight) / 2 + this->_toolBarWidget->height());

    if(!this->isFrameless())
        this->_toolBarWidget->move(this->x(), this->y() - extraHeight / 2);

    if(this->_toolButtons.length() > 0)
        this->_bottomToolBarWidget->move(this->x(), this->y() + this->height());
}

// -----------------
// - Modal overlay -
// -----------------

ModalOverlay::ModalOverlay(QWidget* parent) : QWidget(parent) {
    this->setGeometry(0, 0, parent->width(), parent->height());
    this->setAutoFillBackground(true);

    this->_opacityAnimation = new QPropertyAnimation(this, "color");
    this->_opacityAnimation->setDuration(400);
    this->_opacityAnimation->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
}

QColor ModalOverlay::color() {
    return this->_color;
}

void ModalOverlay::setColor(QColor color) {
    if(this->_color != color) {
        this->_color = color;
        QPalette pal = palette();
        pal.setColor(QPalette::Window, color);
        this->setPalette(pal);
        emit this->colorChanged(color);
    }
}

void ModalOverlay::setVisibleAnimated(bool visible) {
    if(this->isVisible() == visible)
        return;

    if(visible)
        this->_show();
    else
        this->_hide();
}

void ModalOverlay::_show() {
    disconnect(this->_opacityAnimation, &QPropertyAnimation::finished, this, &LinboDialog::hide);
    this->_opacityAnimation->setEndValue(ModalOverlay::_VISIBLE_COLOR);
    this->setColor(ModalOverlay::_INVISIBLE_COLOR);
    this->setVisible(true);
    this->_opacityAnimation->setStartValue(this->color());
    this->_opacityAnimation->start();
}

void ModalOverlay::_hide() {
    connect(this->_opacityAnimation, &QPropertyAnimation::finished, this, &LinboDialog::hide);
    this->_opacityAnimation->setEndValue(ModalOverlay::_INVISIBLE_COLOR);
}

void ModalOverlay::mouseReleaseEvent (QMouseEvent* event) {
    Q_UNUSED(event)
    emit this->clicked();
}
