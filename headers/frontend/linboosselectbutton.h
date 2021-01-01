/****************************************************************************
 ** Modern Linbo GUI
 ** Copyright (C) 2020  Dorian Zedler <dorian@itsblue.de>
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

#include "qmodernpushbutton.h"
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

    QModernPushButton* button;
    QList<QModernPushButton*> startActionButtons;
    QList<QModernPushButton*> rootActionButtons;
    QModernPushButtonOverlay* defaultStartActionOverlay;
    QModernPushButtonOverlay* defaultRootActionOverlay;
    QButtonGroup* buttonGroup;
    LinboOs* os;

private slots:
    void handleBackendStateChange(LinboBackend::LinboState state);
    void updateActionButtonVisibility();
    void handlePrimaryButtonClicked();

signals:
    void imageCreationRequested();
    void imageUploadRequested();

};

#endif // LINBOOSSELECTBUTTON_H
