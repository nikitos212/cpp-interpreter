#include "ItmoHighlighter.h"
#include <QColor>

ItmoHighlighter::ItmoHighlighter(QTextDocument* parent)
  : QSyntaxHighlighter(parent)
{
    QTextCharFormat kwFmt;
    QTextCharFormat kwFmt2;
    kwFmt.setForeground(Qt::blue);
    kwFmt2.setForeground(QColorConstants::Svg::orange);
    for (auto kw : {"if","then","else","end","function","for","in","while", "return", "nil", "break", "continue"}) {
        keywordRules.append({
            QRegularExpression("\\b" + QString(kw) + "\\b"),
            kwFmt
        });
    }

    for (auto kw : {"range","print","push","pop","remove","sort","insert","MAX", "MIN", "ceil", "abs", "floor", "round", "sqrt", "rnd", "parse_num", "to_string", "lower", "upper", "split", "join", "replace", "println", "read", "stacktrace"}) {
        keywordRules.append({
            QRegularExpression("\\b" + QString(kw) + "\\b"),
            kwFmt2
        });
    }

    QTextCharFormat strFmt;
    strFmt.setForeground(Qt::darkGreen);
    stringRules.append({
        QRegularExpression("\".*?\""),
        strFmt
    });

    QTextCharFormat comFmt;
    comFmt.setForeground(Qt::darkGray);
    commentRules.append({
        QRegularExpression("//[^\\n]*"),
        comFmt
    });
}

void ItmoHighlighter::highlightBlock(const QString& text) {
    struct Range { int start, length; };
    QVector<Range> skip;

    auto applyAndRemember = [&](const QVector<Rule>& rules){
        for (auto& r : rules) {
            auto it = r.pattern.globalMatch(text);
            while (it.hasNext()) {
                auto match = it.next();
                int idx = match.capturedStart();
                int len = match.capturedLength();
                setFormat(idx, len, r.format);
                skip.push_back({idx, len});
            }
        }
    };

    applyAndRemember(stringRules);
    applyAndRemember(commentRules);

    for (auto& r : keywordRules) {
        auto it = r.pattern.globalMatch(text);
        while (it.hasNext()) {
            auto match = it.next();
            int idx = match.capturedStart();
            int len = match.capturedLength();

            bool inSkip = false;
            for (auto& rs : skip) {
                if (idx >= rs.start && idx < rs.start + rs.length) {
                    inSkip = true;
                    break;
                }
            }
            if (!inSkip) {
                setFormat(idx, len, r.format);
            }
        }
    }
}
