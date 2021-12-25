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
    LinboBackend* _backend;
    QList<LinboOsSelectButton*> _osButtons;
    QButtonGroup* _osButtonGroup;
    QLabel* _noOsLabel;
    QFont _noOsLabelFont;
    QLabel* _environmentValuesLabel;
    QFont _environmentValuesLabelFont;
    QPropertyAnimation* _sizeAnimation;
    bool _showOnlySelectedButton;
    bool _inited;

    QSize* _sizeOverride;

private slots:
    void _resizeAndPositionAllButtons(int heightOverride = -1, int widthOverride = -1);
    void _handleButtonToggled(bool checked);
    void _handleLinboStateChanged(LinboBackend::LinboState newState);

signals:
    void imageCreationRequested();
    void imageUploadRequested();

};

#endif // LINBOOSSELECTIONROW_H
