#include "lib/interpreter/interpreter.h"
#include <gtest/gtest.h>

TEST(ListFuncsTestSuite, PushPopRemoveInsertTest) {
    std::string code = R"(
        a = [11, true, 10.5, false]

        push(a, 3)
        for i in a
            print(i)
            print(" ")
        end for
        print("||")

        pop(a)
        for i in a
            print(i)
            print(" ")
        end for
        print("||")

        remove(a, 1)
        for i in a
            print(i)
            print(" ")
        end for
        print("||")
        
        insert(a, 1 , "string")
        for i in a
            print(i)
            print(" ")
        end for
    )";

    std::string expected = "11 true 10.5 false 3 ||11 true 10.5 false ||11 10.5 false ||11 string 10.5 false ";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ListFuncsTestSuite, SortTest) {
    std::string code = R"(
        a = [11, 1.7, 10.5, 4]

        sort(a)
        for i in a
            print(i)
            print(" ")
        end for
    )";

    std::string expected = "1.7 4 10.5 11 ";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}