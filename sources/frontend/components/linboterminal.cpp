#include "linboterminal.h"

LinboTerminal::LinboTerminal(QWidget* parent) : QTextEdit(parent)
{
    this->_currentHistoryIndex = -1;
    this->_fixedPosition = 0;
    this->_commandBeforeHistorySwitch.clear();
    this->_doNotExitOnProcessExit = false;

    this->setStyleSheet(
        gTheme->insertValues(
            "QTextEdit {"
            "   border: 0 0 0 0;"
            "   background: %ElevatedBackgroundColor;"
            "   padding-left: 5px;"
            "   color: %TextColor;"
            "}"
        ));

    this->verticalScrollBar()->setStyleSheet(
        gTheme->insertValues(
            "QScrollBar:vertical {"
            "    background: %ElevatedBackgroundColor;"
            "    width:10px;    "
            "    margin: 0px 0px 0px 0px;"
            "}"
            "QScrollBar::handle:vertical {"
            "    background: %TextColor;"
            "    min-height: 10px;"
            "}"
            "QScrollBar::add-line:vertical {"
            "    height: 0px;"
            "}"
            "QScrollBar::sub-line:vertical {"
            "    height: 0 px;"
            "}"
        ));

    this->setCursorWidth(8);
    this->setFont(QFont("Ubuntu Mono"));

    connect(this, &QTextEdit::cursorPositionChanged, this, &LinboTerminal::_handleCursorPositionChanged);

    this->_process = new QProcess(this);
    this->_process->setProcessChannelMode(QProcess::MergedChannels);

    connect(this->_process, &QProcess::readyReadStandardOutput, this, &LinboTerminal::_readOutput);
    connect(this->_process, &QProcess::readyReadStandardError, this, &LinboTerminal::_readOutput);
    connect(this->_process, &QProcess::finished, this, &LinboTerminal::_handleProcessFinished);

    this->_process->setEnvironment({"PS1=$(whoami)@$(hostname):$(pwd)$ "});
    this->_process->start("sh", QStringList("-i"));
    this->moveCursor(QTextCursor::End);
}

void LinboTerminal::keyPressEvent(QKeyEvent *event)
{
    bool accept;

    int key = event->key();
    if (key == Qt::Key_Backspace || event->key() == Qt::Key_Left) {
        accept = textCursor().position() > _fixedPosition;
    }
    else if (key == Qt::Key_Return) {
        accept = false;
        this->_execute(this->_getCurrentCommand());
    }
    else if (key == Qt::Key_Up || key == Qt::Key_Down) {
        accept = false;

        int oldHistoryIndex = this->_currentHistoryIndex;

        if(key == Qt::Key_Up) {
            this->_currentHistoryIndex += 1;
        }
        else if(key == Qt::Key_Down) {
            this->_currentHistoryIndex -= 1;
        }

        if(this->_currentHistoryIndex >= 0 && this->_commandHistory.length() > this->_currentHistoryIndex) {
            if(oldHistoryIndex == -1)
                // store current command before bringing up the history
                this->_commandBeforeHistorySwitch = this->_getCurrentCommand();

            // show requested history command
            this->_setCurrentCommand(this->_commandHistory[this->_currentHistoryIndex]);
        }
        else if(this->_currentHistoryIndex == -1) {
            // show last typed command (is not in history yet, as it was not yet executed)
            this->_setCurrentCommand(this->_commandBeforeHistorySwitch);
            this->_commandBeforeHistorySwitch.clear();
        }
        else if(this->_commandHistory.length() <= this->_currentHistoryIndex) {
            // don't go above history length
            this->_currentHistoryIndex = this->_commandHistory.length() -1;
        }
        else {
            // don't do below -1
            this->_currentHistoryIndex = -1;
        }
    }
    else if( key == Qt::Key_Tab) {
        this->_process->write(QByteArray());
        accept = false;
    }
    else if( key == Qt::Key_C) {
        accept = false;
        if(event->modifiers().testFlag(Qt::ControlModifier) && event->modifiers().testFlag(Qt::ShiftModifier))
            // copy on ctrl+shift+c
            this->copy();
        else if(event->modifiers().testFlag(Qt::ControlModifier)) {
            // restart process on ctrl+c
            this->_setCurrentCommand("^C");
            this->_restartProcess();
        }
        else
            accept = true;
    }
    else if(key == Qt::Key_V) {
        accept = false;
        if(event->modifiers().testFlag(Qt::ShiftModifier))
            // paste on ctrl+shift+v
            this->paste();
        else
            accept = true;
    }
    else if(key == Qt::Key_Control || key == Qt::Key_Shift) {
        accept = true;
    }
    else {
        if(textCursor().position() < _fixedPosition)
            this->moveCursor(QTextCursor::End);
        accept = true;
    }

    if (accept) {
        QTextEdit::keyPressEvent(event);
    }
}

QString LinboTerminal::_getCurrentCommand() {
    int count = toPlainText().length() - _fixedPosition;
    return toPlainText().right(count);
}

void LinboTerminal::_setCurrentCommand(QString command) {
    while(this->textCursor().position() > this->_fixedPosition)
        this->moveCursor(QTextCursor::Left, QTextCursor::MoveAnchor);

    this->moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
    this->textCursor().removeSelectedText();

    this->insertPlainText(command);
}

void LinboTerminal::_readOutput() {
    QString output = this->_process->readAll();
    this->append(output);
    this->moveCursor(QTextCursor::End);
    _fixedPosition = textCursor().position();
}

void LinboTerminal::_handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)

    this->_process->start("sh", QStringList("-i"));

    if(!this->_doNotExitOnProcessExit) {
        this->clear();
        emit this->processExited();
    }

    this->_doNotExitOnProcessExit = false;
}

void LinboTerminal::_execute(QString command) {

    // handle history
    if (command != "")
        this->_commandHistory.insert(0, command);
    this->_currentHistoryIndex = -1;

    // execute command
    if(command == "clear") {
        this->clear();
        this->_fixedPosition = 0;
        command = "";
    }

    this->_process->write((command + "\n").toUtf8());
}

void LinboTerminal::_restartProcess() {
    this->_doNotExitOnProcessExit = true;
    this->_process->kill();
}

void LinboTerminal::_handleCursorPositionChanged()
{
    //qDebug() << "Cursor position changed position: " << textCursor().position() << " fixed position " << fixedPosition;
    if (textCursor().position() < _fixedPosition) {
        this->setCursorWidth(0);
    }
    else {
        this->setCursorWidth(8);
    }
}

void LinboTerminal::clearAndRestart() {
    this->clear();
    this->_fixedPosition = 0;
    this->_commandHistory.clear();
    this->_restartProcess();
}
