#pragma once
#include <QString>

class ItmoWrapper {
public:
    ItmoWrapper() = default;
    ~ItmoWrapper() = default;

    QString run(const QString& code);
};
