#pragma once
#include <QPlainTextEdit>

class ConsoleWidget : public QPlainTextEdit {
    Q_OBJECT
public:
    ConsoleWidget(QWidget* parent = nullptr) : QPlainTextEdit(parent) {
        setReadOnly(true);
        QFont consFont("Courier New", 16);
        setFont(consFont);
    }
    void append(const QString& text) {
        moveCursor(QTextCursor::End);
        insertPlainText(text + "\n");
    }
    void clear() { QPlainTextEdit::clear(); }
};