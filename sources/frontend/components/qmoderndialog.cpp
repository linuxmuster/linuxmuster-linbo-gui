#include "qmoderndialog.h"

QModernDialog::QModernDialog(QWidget* parent) : QWidget(parent)
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

    QPalette pal2 = palette();
    pal2.setColor(QPalette::Background, Qt::white);
    this->setAutoFillBackground(true);
    this->setPalette(pal2);

    this->raise();
    this->setVisible(false);

    //
    // - ToolBar -
    //

    this->titleLabel = new QLabel(this->objectName());
    this->titleLabel->setAlignment(Qt::AlignCenter);
    this->closeButton = new QModernPushButton(":svgIcons/cancel.svg");
    connect(this->closeButton, SIGNAL(clicked()), this, SLOT(autoClose()));

    this->toolBarWidget = new QWidget(parent);
    this->toolBarWidget->setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, "white");
    this->toolBarWidget->setPalette(pal);
    this->toolBarWidget->hide();

    QVBoxLayout* toolBarVLayout = new QVBoxLayout(this->toolBarWidget);
    toolBarVLayout->setContentsMargins(0,0,0,0);

    this->toolBarLayout = new QHBoxLayout();
    toolBarVLayout->addLayout(this->toolBarLayout);
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
}

void QModernDialog::open() {
    this->setVisibleAnimated(true);
}

void QModernDialog::close() {
    this->setVisibleAnimated(false);
}

void QModernDialog::autoClose() {
    this->close();
}

void QModernDialog::setScale(double scale) {
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

double QModernDialog::getScale() {
    return this->scale;
}

void QModernDialog::setTitle(QString title) {
    this->title = title;
    this->titleLabel->setText(title);
}

QString QModernDialog::getTitle() {
    return this->title;
}

void QModernDialog::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    int toolBarHeight = this->parentWidget()->height() * 0.07;

    this->toolBarWidget->setGeometry(this->geometry().x(), this->geometry().y() - toolBarHeight, this->geometry().width(), toolBarHeight);
    this->toolBarLayout->setContentsMargins(toolBarHeight * 0.2, 0,toolBarHeight * 0.1, 0);

    QFont titleFont = this->titleLabel->font();
    titleFont.setPixelSize(toolBarHeight <= 0 ? 1:toolBarHeight * 0.5);
    this->titleLabel->setFont(titleFont);
    this->titleLabel->setFixedHeight(toolBarHeight * 0.9);
    this->titleLabel->setFixedWidth(this->width() - toolBarHeight * 0.2);

    this->closeButton->setFixedHeight(toolBarHeight * 0.9 * 0.9);
    this->closeButton->setFixedWidth(toolBarHeight * 0.9 * 0.9);

    if(!this->isFrameless()) {
        QMargins contentMargins = this->contentsMargins();
        contentMargins.setTop(0);
        this->setContentsMargins(contentMargins);
    }
}

void QModernDialog::setVisibleAnimated(bool visible) {
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
}

void QModernDialog::animationFinished() {
    if(this->opacityEffect->opacity() == 0) {
        this->hide();
        this->toolBarWidget->hide();
    }
    this->opacityEffect->setEnabled(false);
    this->toolBarOpacityEffect->setEnabled(false);
    this->busy = false;
}

bool QModernDialog::isFrameless() {
    return (this->windowFlags() & Qt::FramelessWindowHint) == Qt::FramelessWindowHint;
}

void QModernDialog::centerInParent() {
    int width = this->width();
    int extraHeight = 0;

    if(!this->isFrameless())
        extraHeight = this->toolBarWidget->height();

    this->move((this->parentWidget()->width() - width) / 2, (this->parentWidget()->height() - this->height() - extraHeight) / 2 + extraHeight);

    if(!this->isFrameless())
        this->toolBarWidget->move(this->x(), this->y() - extraHeight);
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
