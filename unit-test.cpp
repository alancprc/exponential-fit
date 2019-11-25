#include <gtest/gtest.h>
#include "exponential-fit.h"

TEST(ExpFitTest, PositiveTest)
{
  ExpFit expfit;
  expfit.SetTest(5, 1, 1, 0);
  auto result = expfit.GetResult();
  EXPECT_NEAR(std::get<0>(result), 1, 1e-6);
  EXPECT_NEAR(std::get<1>(result), 1, 1e-6);
  EXPECT_NEAR(std::get<2>(result), 0, 1e-6);
}

TEST(ExpFitTest, NegativeABTest)
{
  ExpFit expfit;
  expfit.SetTest(5, -1, -1.0, 0);
  auto result = expfit.GetResult();
  EXPECT_NEAR(std::get<0>(result), -1, 1e-6);
  EXPECT_NEAR(std::get<1>(result), -1, 1e-6);
  EXPECT_NEAR(std::get<2>(result), 0, 1e-6);
}

TEST(ExpFitTest, NegativeATest)
{
  ExpFit expfit;
  expfit.SetTest(5, -1, 1.0, 0);
  auto result = expfit.GetResult();
  EXPECT_NEAR(std::get<0>(result), -1, 1e-6);
  EXPECT_NEAR(std::get<1>(result), 1, 1e-6);
  EXPECT_NEAR(std::get<2>(result), 0, 1e-6);
}

TEST(ExpFitTest, NegativeBTest)
{
  ExpFit expfit;
  expfit.SetTest(5, 1, -1.0, 0);
  auto result = expfit.GetResult();
  EXPECT_NEAR(std::get<0>(result), 1, 1e-6);
  EXPECT_NEAR(std::get<1>(result), -1, 1e-6);
  EXPECT_NEAR(std::get<2>(result), 0, 1e-6);
}

TEST(ExpFitTest, PositiveMorePointsTest)
{
  ExpFit expfit;
  expfit.SetTest(50, 1, 1, 0);
  auto result = expfit.GetResult();
  EXPECT_NEAR(std::get<0>(result), 1, 1e-6);
  EXPECT_NEAR(std::get<1>(result), 1, 1e-6);
  EXPECT_NEAR(std::get<2>(result), 0, 1e-6);
}

TEST(ExpFitTest, NegativeMorePointsTest)
{
  ExpFit expfit;
  expfit.SetTest(50, -1, -1.0, 0);
  auto result = expfit.GetResult();
  EXPECT_NEAR(std::get<0>(result), -1, 0.404);
  EXPECT_NEAR(std::get<1>(result), -1, 0.034);
  EXPECT_NEAR(std::get<2>(result), 0, 1e-6);
}
