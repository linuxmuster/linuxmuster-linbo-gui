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

#ifndef LINBOOSSELECTBUTTON_H
#define LINBOOSSELECTBUTTON_H

#include <QObject>
#include <QWidget>
#include <QButtonGroup>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QEventLoop>
#include <QLabel>

#include "linbopushbutton.h"
#include "linboos.h"
#include "linbobackend.h"

class LinboOsSelectButton : public QWidget
{
    Q_OBJECT
public:
    friend class LinboOsSelectionRow;

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    LinboOsSelectButton(QString icon, LinboOs* os, QButtonGroup* buttonGroup, QWidget* parent = nullptr);

    LinboOs* getOs();
    void setVisibleAnimated(bool visible);
    void setVisible(bool visible) override;

    // if set to true, the "old" Layout will be used (one big and four small buttons)
    void setShowActionButtons(bool showActionButtons);

    bool inited;
    bool showActionButtons;
    bool shouldBeVisible;
    bool showDefaultAction;

    LinboOs* os;
    LinboPushButton* button;
    QList<LinboPushButton*> startActionButtons;
    QList<LinboPushButton*> rootActionButtons;
    LinboPushButtonOverlay* defaultStartActionOverlay;
    LinboPushButtonOverlay* defaultRootActionOverlay;
    QButtonGroup* buttonGroup;
    QLabel* osNameLabel;

private slots:
    void handleBackendStateChange(LinboBackend::LinboState state);
    void updateActionButtonVisibility(bool doNotAnimate = false);
    void handlePrimaryButtonClicked();
    QString getTooltipContentForAction(LinboOs::LinboOsStartAction action);

signals:
    void imageCreationRequested();
    void imageUploadRequested();

};

#endif // LINBOOSSELECTBUTTON_H
