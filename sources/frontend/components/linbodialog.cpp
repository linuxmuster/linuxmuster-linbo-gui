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
    this->scale = 1;
    this->busy = false;
    this->modalOverlayWidget = new ModalOverlay(parent);
    this->modalOverlayWidget->setVisible(false);
    connect(this->modalOverlayWidget, SIGNAL(clicked()), this, SLOT(autoClose()));
    connect(this->modalOverlayWidget, SIGNAL(clicked()), this, SIGNAL(closedByUser()));

    this->opacityEffect = new QGraphicsOpacityEffect(this);
    this->opacityEffect->setOpacity(0);
    this->opacityEffect->setEnabled(false);
    this->setGraphicsEffect(this->opacityEffect);

    this->opacityEffectAnimation = new QPropertyAnimation(this->opacityEffect, "opacity");
    this->opacityEffectAnimation->setDuration(200);
    this->opacityEffectAnimation->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
    connect(this->opacityEffectAnimation, SIGNAL(finished()), this, SLOT(animationFinished()));
    connect(this->opacityEffectAnimation, SIGNAL(finished()), this, SIGNAL(closedByUser()));

    QPalette pal = palette();
    pal.setColor(QPalette::Background, gTheme->getColor(LinboGuiTheme::BackgroundColor));
    this->setAutoFillBackground(true);
    this->setPalette(pal);
    this->setStyleSheet("QLabel { color: " + gTheme->getColor(LinboGuiTheme::TextColor).name() + ";} ");

    this->raise();
    this->setVisible(false);

    //
    // - ToolBar -
    //

    this->titleLabel = new QLabel(this->objectName());
    this->titleLabel->setAlignment(Qt::AlignCenter);
    this->closeButton = new LinboToolButton(LinboGuiTheme::CancelIcon);
    connect(this->closeButton, SIGNAL(clicked()), this, SLOT(autoClose()));

    this->toolBarWidget = new QWidget(parent);
    this->toolBarWidget->setAutoFillBackground(true);
    this->toolBarWidget->setPalette(pal);
    this->toolBarWidget->setStyleSheet("QLabel { color: " + gTheme->getColor(LinboGuiTheme::TextColor).name() + ";} ");

    this->toolBarWidget->hide();

    this->toolBarLayout = new QHBoxLayout(this->toolBarWidget);
    this->toolBarLayout->setContentsMargins(0,0,0,0);
    this->toolBarLayout->addWidget(this->titleLabel);
    this->toolBarLayout->setAlignment(this->titleLabel, Qt::AlignCenter);
    this->toolBarLayout->addStretch();
    this->toolBarLayout->addWidget(this->closeButton);
    this->toolBarLayout->setAlignment(this->closeButton, Qt::AlignCenter);

    this->toolBarOpacityEffect = new QGraphicsOpacityEffect(this->toolBarWidget);
    this->toolBarOpacityEffect->setOpacity(0);
    this->toolBarOpacityEffect->setEnabled(false);
    this->toolBarWidget->setGraphicsEffect(this->toolBarOpacityEffect);

    this->toolBarOopacityEffectAnimation = new QPropertyAnimation(this->toolBarOpacityEffect, "opacity");
    this->toolBarOopacityEffectAnimation->setDuration(200);
    this->toolBarOopacityEffectAnimation->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));

    //
    // - Bottom tool bar -
    //

    this->bottomToolBarWidget = new QWidget(parent);
    this->bottomToolBarWidget->setAutoFillBackground(true);
    this->bottomToolBarWidget->setPalette(pal);
    this->bottomToolBarWidget->setStyleSheet("QLabel { color: " + gTheme->getColor(LinboGuiTheme::TextColor).name() + ";} ");

    this->bottomToolBarWidget->hide();

    this->bottomToolBarLayout = new QHBoxLayout(this->bottomToolBarWidget);
    this->bottomToolBarLayout->setContentsMargins(0,0,0,0);

    this->bottomToolBarOpacityEffect = new QGraphicsOpacityEffect(this->bottomToolBarWidget);
    this->bottomToolBarOpacityEffect->setOpacity(0);
    this->bottomToolBarOpacityEffect->setEnabled(false);
    this->bottomToolBarWidget->setGraphicsEffect(this->bottomToolBarOpacityEffect);

    this->bottomToolBarOopacityEffectAnimation = new QPropertyAnimation(this->bottomToolBarOpacityEffect, "opacity");
    this->bottomToolBarOopacityEffectAnimation->setDuration(200);
    this->bottomToolBarOopacityEffectAnimation->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
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
    this->toolButtons.append(toolButton);
    this->bottomToolBarLayout->addWidget(toolButton);
    this->repaint();
}

void LinboDialog::setScale(double scale) {
    if(scale == this->scale)
        return;

    this->scale = scale;

    if(this->originalGeometry.isEmpty())
        this->originalGeometry = this->geometry();

    QRect newGeometry;
    newGeometry.setX(this->originalGeometry.x() + this->originalGeometry.width() * (1 - scale) * 0.5);
    newGeometry.setY(this->originalGeometry.y() + this->originalGeometry.height() * (1 - scale) * 0.5);
    newGeometry.setWidth(this->originalGeometry.width() * scale);
    newGeometry.setHeight(this->originalGeometry.height() * scale);
    this->setGeometry(newGeometry);
}

double LinboDialog::getScale() {
    return this->scale;
}

void LinboDialog::setTitle(QString title) {
    this->title = title;
    this->titleLabel->setText(title);
}

QString LinboDialog::getTitle() {
    return this->title;
}

void LinboDialog::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    int rowHeight = gTheme->getSize(LinboGuiTheme::RowHeight);
    int margins = gTheme->getSize(LinboGuiTheme::Margins);
    int toolBarHeight = rowHeight + margins ;

    this->toolBarWidget->setGeometry(this->geometry().x(), this->geometry().y() - toolBarHeight, this->geometry().width(), toolBarHeight);
    this->toolBarLayout->setContentsMargins(margins, margins * 0.5, margins * 0.5, 0);

    QFont titleFont = this->titleLabel->font();
    titleFont.setPixelSize(gTheme->getSize(LinboGuiTheme::RowFontSize) * 1.5);
    this->titleLabel->setFont(titleFont);
    this->titleLabel->setFixedHeight(rowHeight);
    this->titleLabel->setFixedWidth(this->width() - margins);

    this->closeButton->setFixedHeight(rowHeight);
    this->closeButton->setFixedWidth(rowHeight);

    this->bottomToolBarWidget->setGeometry(this->geometry().x(), this->geometry().y() + this->height(), this->geometry().width(), toolBarHeight);
    this->bottomToolBarLayout->setSpacing(margins);
    this->bottomToolBarLayout->setContentsMargins(margins, 0, margins, margins);

    for(LinboToolButton* toolButton : this->toolButtons)
        toolButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    if(!this->isFrameless()) {
        QMargins contentMargins = this->contentsMargins();
        contentMargins.setTop(0);
        this->setContentsMargins(contentMargins);
    }
}

void LinboDialog::setVisibleAnimated(bool visible) {
    if(this->isVisible() == visible || this->busy)
        return;

    this->busy = true;
    this->modalOverlayWidget->setVisibleAnimated(visible);

    if(visible) {
        this->setScale(1);
        this->show();
        emit this->opened();
        if(!this->isFrameless())
            this->toolBarWidget->show();

        if(this->toolButtons.length() > 0)
            this->bottomToolBarWidget->show();
    }

    this->opacityEffect->setEnabled(true);
    this->opacityEffectAnimation->setStartValue(visible ? 0:1);
    this->opacityEffectAnimation->setEndValue(visible ? 1:0);
    this->opacityEffectAnimation->start();

    if(!this->isFrameless()) {
        this->toolBarOpacityEffect->setEnabled(true);
        this->toolBarOopacityEffectAnimation->setStartValue(visible ? 0:1);
        this->toolBarOopacityEffectAnimation->setEndValue(visible ? 1:0);
        this->toolBarOopacityEffectAnimation->start();
    }

    if(this->toolButtons.length() > 0) {
        this->bottomToolBarOpacityEffect->setEnabled(true);
        this->bottomToolBarOopacityEffectAnimation->setStartValue(visible ? 0:1);
        this->bottomToolBarOopacityEffectAnimation->setEndValue(visible ? 1:0);
        this->bottomToolBarOopacityEffectAnimation->start();
    }
}

void LinboDialog::animationFinished() {
    if(this->opacityEffect->opacity() == 0) {
        this->hide();
        this->toolBarWidget->hide();
        this->bottomToolBarWidget->hide();
    }
    this->opacityEffect->setEnabled(false);
    this->toolBarOpacityEffect->setEnabled(false);
    this->bottomToolBarOpacityEffect->setEnabled(false);
    this->busy = false;
}

bool LinboDialog::isFrameless() {
    return (this->windowFlags() & Qt::FramelessWindowHint) == Qt::FramelessWindowHint;
}

void LinboDialog::centerInParent() {
    int width = this->width();
    int extraHeight = 0;

    if(!this->isFrameless())
        extraHeight = this->toolBarWidget->height();

    if(this->toolButtons.length() > 0)
        extraHeight += this->bottomToolBarWidget->height();

    this->move((this->parentWidget()->width() - width) / 2, (this->parentWidget()->height() - this->height() - extraHeight) / 2 + this->toolBarWidget->height());

    if(!this->isFrameless())
        this->toolBarWidget->move(this->x(), this->y() - extraHeight / 2);

    if(this->toolButtons.length() > 0)
        this->bottomToolBarWidget->move(this->x(), this->y() + this->height());
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
    this->color = color;
    QPalette pal = palette();
    pal.setColor(QPalette::Background, color);
    this->setPalette(pal);
}

void ModalOverlay::setVisibleAnimated(bool visible) {
    if(this->isVisible() == visible)
        return;

    if(visible) {
        disconnect(this->opacityAnimation, SIGNAL(finished()), this, SLOT(hide()));
        this->setColor("#00000000");
        this->setVisible(true);
    }
    else {
        connect(this->opacityAnimation, SIGNAL(finished()), this, SLOT(hide()));
    }

    this->opacityAnimation->setStartValue(this->getColor());
    this->opacityAnimation->setEndValue(visible ? "#66000000":"#00000000");
    this->opacityAnimation->start();
}

void ModalOverlay::mouseReleaseEvent (QMouseEvent* event) {
    Q_UNUSED(event)
    emit this->clicked();
}
