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
  EXPECT_NEAR(expfit.GetA(), 1, 4e-6);
  EXPECT_NEAR(expfit.GetB(), 1, 1e-6);
  EXPECT_NEAR(expfit.GetC(), 0, 4e-5);
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

TEST(ExpFitTest, NoiseTest)
{
  ExpFit expfit;
  expfit.SetY({1, expf(-1) + 0.004, expf(-2) - 0.003, expf(-3)});
  expfit.SetX();
  expfit.CalcFit();
  EXPECT_NEAR(expfit.GetA(), 1, 0.08);
  EXPECT_NEAR(expfit.GetB(), -1, 0.03);
  EXPECT_NEAR(expfit.GetC(), 0, 0.004);
}

TEST(ExpFitTest, SimCapIdealTest)
{
  ExpFit expfit;
  expfit.SetTest(220, 0.5, -1.0 / 220, 0.0);
  EXPECT_NEAR(expfit.GetA(), 0.5, 1e-6);
  EXPECT_NEAR(expfit.GetB(), -1.0 / 220, 1e-6);
  EXPECT_NEAR(expfit.GetC(), 0, 1e-6);
}

TEST(ExpFitTest, NonConsequentXTest)
{
  ExpFit expfit;
  expfit.SetY({0.2388, 1.2529, 5.8686, 6.0383, 6.1825});
  expfit.SetX({-5, -4, 6, 7, 8});
  expfit.CalcFit();
  EXPECT_NEAR(expfit.GetA(), -3, 1e-3);
  EXPECT_NEAR(expfit.GetB(), -0.16, 3e-3);
  EXPECT_NEAR(expfit.GetC(), 7, 2e-4);
}

TEST(ExpFitTest, NonIdealDataTest)
{
  ExpFit expfit;
  expfit.SetY(
      {10000, 8000, 6000, 5000, 4000, 3000, 2000, 1000, 750, 500, 250, 100});
  expfit.SetX({36, 39, 45, 48, 53, 61, 73, 97, 108, 121, 140, 152});
  expfit.CalcFit();
  EXPECT_NEAR(expfit.GetA(), 31000, 1e4);
  EXPECT_NEAR(expfit.GetB(), -1.0 / 28, 0.02);
  EXPECT_NEAR(expfit.GetC(), 100, 1e2);
}

TEST(ExpFitTest, TimeMeasureTest)
{
  for (int i = 0; i != 1000; ++i) {
    ExpFit expfit;
    expfit.SetTest(220, 0.5, -1.0 / 220, 0.0);
    EXPECT_NEAR(expfit.GetA(), 0.5, 1e-6);
    EXPECT_NEAR(expfit.GetB(), -1.0 / 220, 1e-6);
    EXPECT_NEAR(expfit.GetC(), 0, 1e-6);
  }
}
