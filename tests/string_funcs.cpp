#include "lib/interpreter/interpreter.h"
#include <gtest/gtest.h>

TEST(StringFuncsTestSuite, LenTest) {
    std::string code = R"(
        a = "1234567890"
        print(len(a))
    )";

    std::string expected = "10";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(StringFuncsTestSuite, LowerTest) {
    std::string code = R"(
        a = "RONALDO"
        print(lower(a))
    )";

    std::string expected = "ronaldo";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(StringFuncsTestSuite, UpperTest) {
    std::string code = R"(
        a = "messi"
        print(upper(a))
    )";

    std::string expected = "MESSI";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(StringFuncsTestSuite, SplitTest) {
    std::string code = R"(
        a = "sa.fs..dg.d..f"
        b = split(a, "..")
        for i in b
            print(i)
            print(" ")
        end for
    )";

    std::string expected = "sa.fs dg.d f ";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(StringFuncsTestSuite, JoinTest) {
    std::string code = R"(
        a = [1 , 2, 3, true, false, "string", 4]
        b = join(a, "d")
        print(b)
    )";

    std::string expected = "1d2d3d1d0dstringd4";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(StringFuncsTestSuite, ReplaceTest) {
    std::string code = R"(
        a = "ITMOITMO"
        b = replace(a, "IT", "LO")
        print(b)
    )";

    std::string expected = "LOMOLOMO";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(StringFuncsTestSuite, SliceTest) {
    std::string code = R"(
        a = "string"
        println(a[1 : 5])
        println(a[2 : ])
        println(a[ : 4])
        println(a[ : ])
    )";

    std::string expected = "trin\nring\nstri\nstring\n";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

