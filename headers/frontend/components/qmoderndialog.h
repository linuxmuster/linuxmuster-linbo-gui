#ifndef QMODERNDIALOG_H
#define QMODERNDIALOG_H

#include <QDialog>
#include <QObject>
#include <QWidget>
#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QtDebug>
#include <QHBoxLayout>

#include "qmodernpushbutton.h"

class ModalOverlay;

class QModernDialog : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double scale READ getScale WRITE setScale)
    Q_PROPERTY(QString title READ getTitle WRITE setTitle)

public:
    QModernDialog(QWidget* parent);

    virtual void setTitle(QString title);
    virtual QString getTitle();

    virtual void centerInParent();

protected:
    void resizeEvent(QResizeEvent *event) override;
    virtual void setVisibleAnimated(bool visible);
    virtual bool isFrameless();

private:
    double scale;
    bool busy;
    QString title;

    ModalOverlay* modalOverlayWidget;

    QGraphicsOpacityEffect* opacityEffect;
    QPropertyAnimation* opacityEffectAnimation;

    QRect originalGeometry;

    QWidget* toolBarWidget;
    QHBoxLayout* toolBarLayout;
    QLabel* titleLabel;
    QModernPushButton* closeButton;
    QGraphicsOpacityEffect* toolBarOpacityEffect;
    QPropertyAnimation* toolBarOopacityEffectAnimation;

public slots:
    void open();
    void close();
    void autoClose();

    void setScale(double scale);
    double getScale();

private slots:
    void animationFinished();

signals:
    void opened();
    void closedByUser();
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
