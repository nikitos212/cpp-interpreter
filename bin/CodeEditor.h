#pragma once
#include <QPlainTextEdit>
class ItmoHighlighter;
class CodeEditor : public QPlainTextEdit {
    Q_OBJECT
public:
    CodeEditor(QWidget* parent = nullptr);
private:
    ItmoHighlighter* highlighter;
};