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

#include "linboterminaldialog.h"

LinboTerminalDialog::LinboTerminalDialog(QWidget* parent) : LinboDialog(parent)
{
    this->currentHistoryIndex = -1;
    this->commandBeforeHistorySwitch.clear();

    //% "Terminal"
    this->setTitle(qtTrId("dialog_terminal_title"));

    this->textBrowser = new LinboTextBrowser();
    this->textBrowser->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
    this->textBrowser->setStyleSheet("QTextBrowser {"
                                     "border: 0 0 0 0;"
                                     "background: black;"
                                     "padding-left: 5px;"
                                     "color: white;"
                                     "}"
                                     "QScrollBar:vertical {"
                                     "    background: black;"
                                     "    width:10px;    "
                                     "    margin: 0px 0px 0px 0px;"
                                     "}"
                                     "QScrollBar::handle:vertical {"
                                     "    background: lightgrey;"
                                     "    min-height: 0px;"
                                     "}"
                                     "QScrollBar::add-line:vertical {"
                                     "    background: grey;"
                                     "    height: 0px;"
                                     "    subcontrol-position: bottom;"
                                     "    subcontrol-origin: margin;"
                                     "}"
                                     "QScrollBar::sub-line:vertical {"
                                     "    background: grey;"
                                     "    height: 0 px;"
                                     "    subcontrol-position: top;"
                                     "    subcontrol-origin: margin;"
                                     "}");

    this->lineEdit = new LinboLineEdit();
    this->lineEdit->installEventFilter(this);
    connect(this->lineEdit, SIGNAL(returnPressed()), this, SLOT(execute()));

    this->mainLayout = new QVBoxLayout(this);
    this->mainLayout->addWidget(this->textBrowser);
    this->mainLayout->addWidget(this->lineEdit);

    this->process = new QProcess(this);
    this->process->setProcessChannelMode(QProcess::MergedChannels);

    connect(this->process, SIGNAL(readyReadStandardOutput()),
             this, SLOT(readOutput()) );
    connect(this->process, SIGNAL(readyReadStandardError()),
             this, SLOT(readOutput()) );
    connect(this->process, SIGNAL(finished(int, QProcess::ExitStatus)),
             this, SLOT(handleProcessFinished(int, QProcess::ExitStatus)));
}

void LinboTerminalDialog::setVisibleAnimated(bool visible) {
    //this->textBrowser->setVisible(visible);
    LinboDialog::setVisibleAnimated(visible);
    if(visible)
        this->process->start("sh", QStringList("-i"));
    else
        this->process->kill();
}


void LinboTerminalDialog::readOutput() {
    QString output = this->process->readAll();
    this->textBrowser->append(output);
}

void LinboTerminalDialog::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)

    this->textBrowser->clear();
    this->close();
}

void LinboTerminalDialog::execute() {
    QString command = this->lineEdit->text();

    // handle history
    this->commandHistory.insert(0, command);
    this->currentHistoryIndex = -1;

    // handle ui
    this->textBrowser->insertPlainText(command);
    this->process->write((command + "\n").toUtf8());
    this->lineEdit->clear();

    // scroll to bottom
    QScrollBar* sb = this->textBrowser->verticalScrollBar();
    sb->setValue(sb->maximum());
}

bool LinboTerminalDialog::eventFilter(QObject* obj, QEvent* event) {
    if(obj == this->lineEdit) {
        if(event != nullptr && event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

            if(keyEvent->key() != Qt::Key_Up && keyEvent->key() != Qt::Key_Down)
                return LinboDialog::eventFilter(obj, event);

            int oldHistoryIndex = this->currentHistoryIndex;

            if(keyEvent->key() == Qt::Key_Up) {
                this->currentHistoryIndex += 1;
            }
            else if(keyEvent->key() == Qt::Key_Down) {
                this->currentHistoryIndex -= 1;
            }

            if(this->currentHistoryIndex >= 0 && this->commandHistory.length() > this->currentHistoryIndex) {
                if(oldHistoryIndex == -1)
                    // store current command before bringing up the history
                    this->commandBeforeHistorySwitch = this->lineEdit->text();

                // show requested history command
                this->lineEdit->setText(this->commandHistory[this->currentHistoryIndex]);
            }
            else if(this->currentHistoryIndex == -1) {
                // show last typed command (is not in history yet, as it was not yet executed)
                this->lineEdit->setText(this->commandBeforeHistorySwitch);
                this->commandBeforeHistorySwitch.clear();
            }
            else if(this->commandHistory.length() <= this->currentHistoryIndex) {
                // don't go above history length
                this->currentHistoryIndex = this->commandHistory.length() -1;
            }
            else {
                // don't do below -1
                this->currentHistoryIndex = -1;
            }
        }
    }
    return LinboDialog::eventFilter(obj, event);
}


void LinboTerminalDialog::resizeEvent(QResizeEvent *event) {
    LinboDialog::resizeEvent(event);

    this->lineEdit->setFixedHeight(this->parentWidget()->height() * 0.06);
    QFont font = this->lineEdit->font();
    font.setPixelSize(this->lineEdit->height() * 0.5);
    this->lineEdit->setFont(font);
}
