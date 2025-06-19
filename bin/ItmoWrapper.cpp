#include "ItmoWrapper.h"
#include "lib/interpreter/interpreter.h"
#include <sstream>

QString ItmoWrapper::run(const QString& code) {
    std::string utf8 = code.toUtf8().constData();

    std::istringstream in(utf8);
    std::ostringstream out;

    auto result = interpret(in, out);

    return QString::fromUtf8(out.str().c_str());
}
