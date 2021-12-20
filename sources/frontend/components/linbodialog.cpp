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

// TODO: clean redundant code
LinboDialog::LinboDialog(QWidget* parent) : QWidget(parent)
{
    this->_busy = false;
    this->_firstChild = nullptr;
    this->setWindowFlags(Qt::Widget);
    this->_modalOverlayWidget = new ModalOverlay(parent);
    this->_modalOverlayWidget->setVisible(false);
    connect(this->_modalOverlayWidget, &ModalOverlay::clicked, this, &LinboDialog::autoClose);
    connect(this->_modalOverlayWidget, &ModalOverlay::clicked, this, &LinboDialog::closedByUser);

    this->_opacityEffect = new QGraphicsOpacityEffect(this);
    this->_opacityEffect->setOpacity(0);
    this->_opacityEffect->setEnabled(false);
    this->setGraphicsEffect(this->_opacityEffect);

    this->_opacityEffectAnimation = new QPropertyAnimation(this->_opacityEffect, "opacity");
    this->_opacityEffectAnimation->setDuration(200);
    this->_opacityEffectAnimation->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
    connect(this->_opacityEffectAnimation, &QPropertyAnimation::finished, this, &LinboDialog::_animationFinished);
    connect(this->_opacityEffectAnimation, &QPropertyAnimation::finished, this, &LinboDialog::closedByUser);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, gTheme->getColor(LinboTheme::BackgroundColor));
    this->setAutoFillBackground(true);
    this->setPalette(pal);
    this->setStyleSheet("QLabel { color: " + gTheme->getColor(LinboTheme::TextColor).name() + ";} ");

    this->raise();
    this->setVisible(false);

    //
    // - ToolBar -
    //

    this->_titleLabel = new QLabel(this->objectName());
    this->_titleLabel->setAlignment(Qt::AlignCenter);
    this->_closeButton = new LinboToolButton(LinboTheme::CancelIcon);
    connect(this->_closeButton, &LinboToolButton::clicked, this, &LinboDialog::autoClose);

    this->_toolBarWidget = new QWidget(parent);
    this->_toolBarWidget->setAutoFillBackground(true);
    this->_toolBarWidget->setPalette(pal);
    this->_toolBarWidget->setStyleSheet("QLabel { color: " + gTheme->getColor(LinboTheme::TextColor).name() + ";} ");

    this->_toolBarWidget->hide();

    this->_toolBarLayout = new QHBoxLayout(this->_toolBarWidget);
    this->_toolBarLayout->setContentsMargins(0,0,0,0);
    this->_toolBarLayout->addWidget(this->_titleLabel);
    this->_toolBarLayout->setAlignment(this->_titleLabel, Qt::AlignCenter);
    this->_toolBarLayout->addStretch();
    this->_toolBarLayout->addWidget(this->_closeButton);
    this->_toolBarLayout->setAlignment(this->_closeButton, Qt::AlignCenter);

    this->_toolBarOpacityEffect = new QGraphicsOpacityEffect(this->_toolBarWidget);
    this->_toolBarOpacityEffect->setOpacity(0);
    this->_toolBarOpacityEffect->setEnabled(false);
    this->_toolBarWidget->setGraphicsEffect(this->_toolBarOpacityEffect);

    this->_toolBarOopacityEffectAnimation = new QPropertyAnimation(this->_toolBarOpacityEffect, "opacity");
    this->_toolBarOopacityEffectAnimation->setDuration(200);
    this->_toolBarOopacityEffectAnimation->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));

    //
    // - Bottom tool bar -
    //

    this->_bottomToolBarWidget = new QWidget(parent);
    this->_bottomToolBarWidget->setAutoFillBackground(true);
    this->_bottomToolBarWidget->setPalette(pal);
    this->_bottomToolBarWidget->setStyleSheet("QLabel { color: " + gTheme->getColor(LinboTheme::TextColor).name() + ";} ");

    this->_bottomToolBarWidget->hide();

    this->_bottomToolBarLayout = new QHBoxLayout(this->_bottomToolBarWidget);
    this->_bottomToolBarLayout->setContentsMargins(0,0,0,0);

    this->_bottomToolBarOpacityEffect = new QGraphicsOpacityEffect(this->_bottomToolBarWidget);
    this->_bottomToolBarOpacityEffect->setOpacity(0);
    this->_bottomToolBarOpacityEffect->setEnabled(false);
    this->_bottomToolBarWidget->setGraphicsEffect(this->_bottomToolBarOpacityEffect);

    this->_bottomToolBarOopacityEffectAnimation = new QPropertyAnimation(this->_bottomToolBarOpacityEffect, "opacity");
    this->_bottomToolBarOopacityEffectAnimation->setDuration(200);
    this->_bottomToolBarOopacityEffectAnimation->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
}

void LinboDialog::open() {
    this->setVisibleAnimated(true);
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

    int rowHeight = gTheme->getSize(LinboTheme::RowHeight);
    int margins = gTheme->getSize(LinboTheme::Margins);
    int toolBarHeight = rowHeight + margins ;

    this->_toolBarWidget->setGeometry(this->geometry().x(), this->geometry().y() - toolBarHeight, this->geometry().width(), toolBarHeight);
    this->_toolBarLayout->setContentsMargins(margins, margins * 0.5, margins * 0.5, 0);

    QFont titleFont = this->_titleLabel->font();
    titleFont.setPixelSize(gTheme->getSize(LinboTheme::RowFontSize) * 1.5);
    this->_titleLabel->setFont(titleFont);
    this->_titleLabel->setFixedHeight(rowHeight);
    this->_titleLabel->setFixedWidth(this->width() - margins);

    this->_closeButton->setFixedHeight(rowHeight);
    this->_closeButton->setFixedWidth(rowHeight);

    this->_bottomToolBarWidget->setGeometry(this->geometry().x(), this->geometry().y() + this->height(), this->geometry().width(), toolBarHeight);
    this->_bottomToolBarLayout->setSpacing(margins);
    this->_bottomToolBarLayout->setContentsMargins(margins, 0, margins, margins);

    for(LinboToolButton* toolButton : this->_toolButtons)
        toolButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    if(!this->isFrameless()) {
        QMargins contentMargins = this->contentsMargins();
        contentMargins.setTop(0);
        this->setContentsMargins(contentMargins);
    }
}


void LinboDialog::paintEvent(QPaintEvent *e) {
    QWidgetList widgets = this->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);

    QWidget* latestWidget = nullptr;
    QWidget* firstWidget = nullptr;

    for(QWidget* widget : widgets) {
        if(widget->focusPolicy() != Qt::NoFocus) {
            if(latestWidget != nullptr && widget->parent() == this)
                QWidget::setTabOrder(latestWidget, widget);

            if(firstWidget == nullptr)
                firstWidget = widget;

            latestWidget = widget;
        }
    }

    this->_firstChild = firstWidget;
    if(this->_firstChild == nullptr)
        this->_firstChild = this->_closeButton;

    for(LinboToolButton* widget: this->_toolButtons) {
        if(widget->focusPolicy() != Qt::NoFocus) {
            if(latestWidget != nullptr)
                QWidget::setTabOrder(latestWidget, widget);

            if(firstWidget == nullptr)
                firstWidget = widget;

            latestWidget = widget;
        }
    }

    QWidget::setTabOrder(this->_toolButtons.last(), this->_closeButton);
    connect(this->_closeButton, &LinboPushButton::defocused, this, [=](Qt::FocusReason reason) {
        if(reason == Qt::TabFocusReason)
            firstWidget->setFocus();
        else if(reason == Qt::BacktabFocusReason)
            latestWidget->setFocus();
    });

    return QWidget::paintEvent(e);
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

    if(visible) {
        this->show();
        emit this->opened();
        if(!this->isFrameless())
            this->_toolBarWidget->show();

        if(this->_toolButtons.length() > 0)
            this->_bottomToolBarWidget->show();
    }

    this->_opacityEffect->setEnabled(true);
    this->_opacityEffectAnimation->setStartValue(visible ? 0:1);
    this->_opacityEffectAnimation->setEndValue(visible ? 1:0);
    this->_opacityEffectAnimation->start();

    if(!this->isFrameless()) {
        this->_toolBarOpacityEffect->setEnabled(true);
        this->_toolBarOopacityEffectAnimation->setStartValue(visible ? 0:1);
        this->_toolBarOopacityEffectAnimation->setEndValue(visible ? 1:0);
        this->_toolBarOopacityEffectAnimation->start();
    }

    if(this->_toolButtons.length() > 0) {
        this->_bottomToolBarOpacityEffect->setEnabled(true);
        this->_bottomToolBarOopacityEffectAnimation->setStartValue(visible ? 0:1);
        this->_bottomToolBarOopacityEffectAnimation->setEndValue(visible ? 1:0);
        this->_bottomToolBarOopacityEffectAnimation->start();
    }
}

void LinboDialog::_animationFinished() {
    if(this->_opacityEffect->opacity() == 0) {
        this->hide();
        this->_toolBarWidget->hide();
        this->_bottomToolBarWidget->hide();
    }
    this->_opacityEffect->setEnabled(false);
    this->_toolBarOpacityEffect->setEnabled(false);
    this->_bottomToolBarOpacityEffect->setEnabled(false);
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

    this->opacityAnimation = new QPropertyAnimation(this, "color");
    this->opacityAnimation->setDuration(400);
    this->opacityAnimation->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
}

QColor ModalOverlay::getColor() {
    return this->color;
}

void ModalOverlay::setColor(QColor color) {
    if(this->color != color) {
        this->color = color;
        QPalette pal = palette();
        pal.setColor(QPalette::Window, color);
        this->setPalette(pal);
        emit this->colorChanged(color);
    }
}

void ModalOverlay::setVisibleAnimated(bool visible) {
    if(this->isVisible() == visible)
        return;

    if(visible) {
        disconnect(this->opacityAnimation, &QPropertyAnimation::finished, this, &LinboDialog::hide);
        this->opacityAnimation->setEndValue(ModalOverlay::_VISIBLE_COLOR);
        this->setColor(ModalOverlay::_INVISIBLE_COLOR);
        this->setVisible(true);
    }
    else {
        connect(this->opacityAnimation, &QPropertyAnimation::finished, this, &LinboDialog::hide);
        this->opacityAnimation->setEndValue(ModalOverlay::_INVISIBLE_COLOR);
    }

    this->opacityAnimation->setStartValue(this->getColor());
    this->opacityAnimation->start();
}

void ModalOverlay::mouseReleaseEvent (QMouseEvent* event) {
    Q_UNUSED(event)
    emit this->clicked();
}
