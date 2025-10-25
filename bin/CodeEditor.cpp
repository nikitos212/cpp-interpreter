#include "CodeEditor.h"
#include "ItmoHighlighter.h"

CodeEditor::CodeEditor(QWidget* parent)
    : QPlainTextEdit(parent)
    , highlighter(new ItmoHighlighter(document()))
{
    setLineWrapMode(QPlainTextEdit::NoWrap);

    QFont codeFont("Courier New", 16);
    setFont(codeFont);
}