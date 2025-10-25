#pragma once
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class ItmoHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    ItmoHighlighter(QTextDocument* parent);

protected:
    void highlightBlock(const QString& text) override;

private:
    struct Rule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<Rule> keywordRules;
    QVector<Rule> stringRules;
    QVector<Rule> commentRules;
};
