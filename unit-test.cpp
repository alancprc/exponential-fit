#include <gtest/gtest.h>
#include "exponential-fit.h"

TEST(ExpFitTest, PositiveTest)
{
  ExpFit expfit;
  expfit.SetTest(5, 1, 1, 0);
  EXPECT_NEAR(expfit.GetA(), 1, 1e-6);
  EXPECT_NEAR(expfit.GetB(), 1, 1e-6);
  EXPECT_NEAR(expfit.GetC(), 0, 1e-6);
}

TEST(ExpFitTest, NegativeABTest)
{
  ExpFit expfit;
  expfit.SetTest(5, -1, -1.0, 0);
  EXPECT_NEAR(expfit.GetA(), -1, 1e-6);
  EXPECT_NEAR(expfit.GetB(), -1, 1e-6);
  EXPECT_NEAR(expfit.GetC(), 0, 1e-6);
}

TEST(ExpFitTest, NegativeATest)
{
  ExpFit expfit;
  expfit.SetTest(5, -1, 1.0, 0);
  EXPECT_NEAR(expfit.GetA(), -1, 1e-6);
  EXPECT_NEAR(expfit.GetB(), 1, 1e-6);
  EXPECT_NEAR(expfit.GetC(), 0, 1e-6);
}

TEST(ExpFitTest, NegativeBTest)
{
  ExpFit expfit;
  expfit.SetTest(5, 1, -1.0, 0);
  EXPECT_NEAR(expfit.GetA(), 1, 1e-6);
  EXPECT_NEAR(expfit.GetB(), -1, 1e-6);
  EXPECT_NEAR(expfit.GetC(), 0, 1e-6);
}

TEST(ExpFitTest, PositiveMorePointsTest)
{
  ExpFit expfit;
  expfit.SetTest(50, 1, 1, 0);
  EXPECT_NEAR(expfit.GetA(), 1, 1e-6);
  EXPECT_NEAR(expfit.GetB(), 1, 1e-6);
  EXPECT_NEAR(expfit.GetC(), 0, 1e-6);
}

TEST(ExpFitTest, NegativeMorePointsTest)
{
  ExpFit expfit;
  expfit.SetTest(50, -1, -1.0, 0);
  EXPECT_NEAR(expfit.GetA(), -1, 0.404);
  EXPECT_NEAR(expfit.GetB(), -1, 0.034);
  EXPECT_NEAR(expfit.GetC(), 0, 1e-6);
}

TEST(ExpFitTest, InputTest)
{
  ExpFit expfit;
  expfit.SetY({1, exp(1), exp(2)});
  expfit.SetX();
  expfit.CalcFit();
  EXPECT_NEAR(expfit.GetA(), 1, 1e-6);
  EXPECT_NEAR(expfit.GetB(), 1, 1e-6);
  EXPECT_NEAR(expfit.GetC(), 0, 1e-6);
}
