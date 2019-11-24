#include <gtest/gtest.h>
#include "exponential-fit.h"

TEST(ExpFitTest, PositiveTest)
{
  ExpFit expfit;
  expfit.SetTest(5, 1, 1, 0);
}

TEST(ExpFitTest, NegativeTest)
{
  ExpFit expfit;
  expfit.SetTest(5, -1, -1.0, 0);
}
