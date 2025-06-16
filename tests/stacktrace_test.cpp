#include "lib/interpreter/interpreter.h"
#include <gtest/gtest.h>

TEST(StacktraceTestSuite, StacktraceTest) {
    std::string code = R"(
        foo = function() 
            println(stacktrace())
            return bar() 
        end function

        bar = function() 
            println(stacktrace())
        end function

        println(stacktrace())
        foo()
        println(stacktrace())
    )";

    std::string expected = "[]\n[foo]\n[foo, bar]\n[]\n";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}