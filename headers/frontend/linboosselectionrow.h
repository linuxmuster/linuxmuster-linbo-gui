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

#ifndef LINBOOSSELECTIONROW_H
#define LINBOOSSELECTIONROW_H

#include <QObject>
#include <QWidget>
#include <QButtonGroup>
#include <QLabel>

#include "linbobackend.h"
#include "linboosselectbutton.h"

class LinboOsSelectionRow : public QWidget
{
    Q_OBJECT
public:
    explicit LinboOsSelectionRow(LinboBackend* backend, QWidget *parent = nullptr);

    LinboOs* getSelectedOs();
    void setShowOnlySelectedButton(bool value);

    void setMinimumSizeAnimated(QSize size);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    LinboBackend* backend;
    QList<LinboOsSelectButton*> osButtons;
    QButtonGroup* osButtonGroup;
    QLabel* noOsLabel;
    QFont noOsLabelFont;
    QLabel* environmentValuesLabel;
    QFont environmentValuesLabelFont;
    QPropertyAnimation* sizeAnimation;
    bool showOnlySelectedButton;
    bool inited;

    QSize* sizeOverride;

private slots:
    void resizeAndPositionAllButtons(int heightOverride = -1, int widthOverride = -1);
    void handleButtonToggled(bool checked);
    void handleLinboStateChanged(LinboBackend::LinboState newState);

signals:
    void imageCreationRequested();
    void imageUploadRequested();

};

#endif // LINBOOSSELECTIONROW_H
