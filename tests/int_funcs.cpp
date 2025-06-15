#include "lib/interpreter/interpreter.h"
#include <gtest/gtest.h>

TEST(IntFuncsTestSuite, AbsTest) {
    std::string code = R"(
        a = -12.3
        print(abs(a))
    )";

    std::string expected = "12.3";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(IntFuncsTestSuite, CeilTest) {
    std::string code = R"(
        a = 5.4
        print(ceil(a))
    )";

    std::string expected = "6";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(IntFuncsTestSuite, FloorTest) {
    std::string code = R"(
        a = 5.6
        print(floor(a))
    )";

    std::string expected = "5";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(IntFuncsTestSuite, RoundtTest) {
    std::string code = R"(
        a = 5.2
        b = 5.7
        print(round(a))
        print(round(b))
    )";

    std::string expected = "56";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(IntFuncsTestSuite, SqrtTest) {
    std::string code = R"(
        a = 9
        print(sqrt(a))
    )";

    std::string expected = "3";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(IntFuncsTestSuite, ParseNumTest) {
    std::string code = R"(
        a = "123"
        print(parse_num(a) + 7)
    )";

    std::string expected = "130";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(IntFuncsTestSuite, ToStringTest) {
    std::string code = R"(
        a = 52
        print(to_string(a) + "SPB")
    )";

    std::string expected = "52SPB";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}