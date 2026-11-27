/**
 * @file main.cpp
 * @brief Entry point for the GoogleTest unit tests.
 */
#include <gtest/gtest.h>

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
