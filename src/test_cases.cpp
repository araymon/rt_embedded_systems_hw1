#include <iostream>
#include <gtest/gtest.h>
#include "hw.h"

TEST(AccessTest1,ReaderPriority)
{
    bool reader_priority = true;    
    run_rw_problem(3,3,reader_priority);
    bool valid = test_access_vector();
  
    ASSERT_TRUE(true == valid);
}
TEST(AccessTest2,WriterPriority)
{
    bool reader_priority = false;    
    run_rw_problem(3,3,reader_priority);
    bool valid = test_access_vector();

    ASSERT_TRUE(true == valid);
}

TEST(PriorityTest1,WriterPriority)
{
    bool reader_priority = false;
    run_rw_problem(2,2,reader_priority);
    std::string str = test_order_vector();

    ASSERT_TRUE(("1100" == str) | ("0110" == str));
}

TEST(PriorityTest2,ReadPriority)
{
    bool reader_priority = true;
    run_rw_problem(2,2,reader_priority);
    std::string str = test_order_vector();

    ASSERT_TRUE(("1001" == str) | ("0011" == str));
}

TEST(PriorityTest3,ReadPriority)
{
    bool reader_priority = true;
    run_rw_problem(2,1,reader_priority);
    std::string str = test_order_vector();

    ASSERT_TRUE(("001" == str) | ("100" == str));
}

TEST(PriorityTest4,WritePriority)
{
    bool reader_priority = false;
    run_rw_problem(3,3,reader_priority);
    std::string str = test_order_vector();

    ASSERT_TRUE(("111000" == str) | ("011100" == str));
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
