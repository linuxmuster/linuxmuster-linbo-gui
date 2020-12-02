#include "qmoderndialog.h"

QModernDialog::QModernDialog(QWidget* parent) : QWidget(parent)
{
    this->scale = 1;
    this->busy = false;
    this->modalOverlayWidget = new ModalOverlay(parent);
    this->modalOverlayWidget->setVisible(false);
    connect(this->modalOverlayWidget, SIGNAL(clicked()), this, SLOT(autoClose()));

    this->opacityEffect = new QGraphicsOpacityEffect(this);
    this->opacityEffect->setOpacity(0);
    this->opacityEffect->setEnabled(false);
    this->setGraphicsEffect(this->opacityEffect);

    this->opacityEffectAnimation = new QPropertyAnimation(this->opacityEffect, "opacity");
    this->opacityEffectAnimation->setDuration(200);
    this->opacityEffectAnimation->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
    connect(this->opacityEffectAnimation, SIGNAL(finished()), this, SLOT(animationFinished()));

    this->scaleAnimation = new QPropertyAnimation(this, "scale");
    this->scaleAnimation->setDuration(200);
    this->scaleAnimation->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));

    QPalette pal2 = palette();
    pal2.setColor(QPalette::Background, Qt::white);
    this->setAutoFillBackground(true);
    this->setPalette(pal2);

    this->raise();
    this->setVisible(false);
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

void QModernDialog::setVisibleAnimated(bool visible) {
    if(this->isVisible() == visible || this->busy)
        return;

    this->busy = true;
    this->modalOverlayWidget->setVisibleAnimated(visible);

    if(visible) {
        this->setScale(1);
        this->setVisible(true);
    }

    this->opacityEffect->setEnabled(true);
    this->opacityEffectAnimation->setStartValue(visible ? 0:1);
    this->opacityEffectAnimation->setEndValue(visible ? 1:0);
    this->opacityEffectAnimation->start();

    //this->scaleAnimation->setStartValue(visible ? 0.95:1);
    //this->scaleAnimation->setEndValue(visible ? 1:0.95);
    //this->scaleAnimation->start();
}

void QModernDialog::animationFinished() {
    if(this->opacityEffect->opacity() == 0) {
        this->hide();
    }
    this->opacityEffect->setEnabled(false);
    this->busy = false;
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
