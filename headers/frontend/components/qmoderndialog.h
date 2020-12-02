#ifndef QMODERNDIALOG_H
#define QMODERNDIALOG_H

#include <QDialog>
#include <QObject>
#include <QWidget>
#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QtDebug>

class ModalOverlay;

class QModernDialog : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double scale READ getScale WRITE setScale)
public:
    QModernDialog(QWidget* parent);

protected:
    virtual void setVisibleAnimated(bool visible);

private:
    double scale;
    bool busy;

    ModalOverlay* modalOverlayWidget;

    QGraphicsOpacityEffect* opacityEffect;
    QPropertyAnimation* opacityEffectAnimation;
    QPropertyAnimation* scaleAnimation;

    QRect originalGeometry;

public slots:
    void open();
    void close();
    void autoClose();

    void setScale(double scale);
    double getScale();

private slots:
    void animationFinished();
};

class ModalOverlay : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QColor color READ getColor WRITE setColor)

public:
    friend class QModernDialog;

protected:
    void mouseReleaseEvent (QMouseEvent* event) override;

private:
    ModalOverlay(QWidget* parent);

    QColor getColor();
    void setColor(QColor color);

    void setVisibleAnimated(bool visible);

    QPropertyAnimation* opacityAnimation;
    QColor color;

signals:
    void clicked();
};

#endif // QMODERNDIALOG_H
