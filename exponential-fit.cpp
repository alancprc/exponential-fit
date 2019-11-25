#include "exponential-fit.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>

using namespace std;

void ExpFit::SetTest(int n, double a, double b, double c)
{
  this->n = n;
  this->a = a;
  this->b = b;
  this->c = c;
  GenTestData();
  PrintTestData();
  CalcFit();
}

void ExpFit::GenTestData()
{
  x.resize(n);
  iota(x.begin(), x.end(), 0);

  y.resize(n);
  for (auto vi = y.begin(); vi != y.end(); ++vi) {
    auto i = std::distance(y.begin(), vi);
    *vi = a * expf(b * i) + c;
  }
}

void ExpFit::PrintTestData()
{
  cout << "test y:\n\t";
  for (const auto& e : y) {
    cout << left << setw(10) << e;
  }
  // clang-format off
  cout << "\ntest parameters:"
       << "\n\tn: " << n
       << "\n\ta: " << a
       << "\n\tb: " << b
       << "\n\tc: " << c
       << "\n";
  // clang-format on
}

void ExpFit::CalcFit()
{
  double c_approx = CalcApproxC();
  double deltaC = a_approx > 0
                      ? *(std::min_element(y.begin(), y.end())) - c_approx
                      : *(std::max_element(y.begin(), y.end())) - c_approx;
  double min_error = CalcFitError(c_approx);

  while (std::abs(deltaC) > 10e-8) {
    double c_tmp = c_approx + deltaC;
    if (AbortCalc(c_tmp)) {
      deltaC /= 2;
      continue;
    }
    double error = CalcFitError(c_approx + deltaC);

    cout << "\n========== c_tmp = " << c_tmp
         << "\n        c_approx = " << c_approx
         << "\n          deltaC = " << deltaC
         << "\n       min_error = " << min_error
         << "\n           error = " << error << endl;

    PrintFit();

    if (error < min_error) {
      min_error = error;
      c_approx += deltaC;
    } else {
      deltaC /= -2;
    }
  }
  c_calc = c_approx;
  CalcFitAB(c_calc);
  PrintFit();
}

ExpFit::ResultType ExpFit::GetResult()
{
  return make_tuple(copysign(expf(lna_calc), a_approx), b_calc, c_calc);
}

double ExpFit::GetA() { return copysignf(expf(lna_calc), a_approx); }

double ExpFit::GetB() { return b_calc; }

double ExpFit::GetC() { return c_calc; }

double ExpFit::CalcApproxC()
{
  dx.resize(n - 1, 1);
  dy.resize(n - 1);
  cx.resize(n - 1);
  dq.resize(n - 1);

  for (int i = 0; i != n - 1; ++i) {
    dy[i] = y[i + 1] - y[i];
    cx[i] = (x[i + 1] + x[i]) / 2;
    dq[i] = dy[i] / dx[i];
  }

  vector<double> b_c(n - 2);  // b calculated
  for (int i = 0; i != n - 2; ++i) {
    b_c[i] = log(dq[i + 1] / dq[i]) / (cx[i + 1] - cx[i]);
  }
  double b_approx = std::accumulate(b_c.begin(), b_c.end(), 0.0) / b_c.size();
  cout << "b_approx: " << b_approx << endl;

  vector<double> a_c(n - 1);  // a calculated
  for (int i = 0; i != n - 1; ++i) {
    a_c[i] = dy[i] / (expf(b_approx * x[i + 1]) - expf(b_approx * x[i]));
  }
  a_approx = std::accumulate(a_c.begin(), a_c.end(), 0.0) / a_c.size();
  cout << "a_approx: " << a_approx << endl;

  vector<double> c_c(n);  // c calculated
  for (int i = 0; i != n; ++i) {
    c_c[i] = y[i] - a_approx * expf(b_approx * x[i]);
  }
  // in case that c_c veries greatly, e.g. [1e-10, 1e-20],
  // the median value is better than mean value.
  auto nth = c_c.begin() + c_c.size() / 2;
  std::nth_element(c_c.begin(), nth, c_c.end());
  double c_approx = *nth;
  cout << "c_approx: " << c_approx << endl;
  return c_approx;
}

void ExpFit::CalcFitAB(double c_approx)
{
  // Calculate the values of ln(yi)
  vector<double> lny(n);
  for (int i = 0; i < n; i++) lny[i] = log(abs(y[i] - c_approx));

  // variables for sums/sigma of xi,yi,xi^2,xiyi etc
  double xsum = 0, x2sum = 0, ysum = 0, xysum = 0;
  for (int i = 0; i < n; i++) {
    xsum += x[i];            // calculate sigma(xi)
    ysum += lny[i];          // calculate sigma(yi)
    x2sum += pow(x[i], 2);   // calculate sigma(x^2i)
    xysum += x[i] * lny[i];  // calculate sigma(xi*yi)
  }

  // calculate slope(or the the power of exp)
  b_calc = (n * xysum - xsum * ysum) / (n * x2sum - xsum * xsum);
  // calculate intercept
  lna_calc = (x2sum * ysum - xsum * xysum) / (x2sum * n - xsum * xsum);

  // to calculate y(fitted) at given x points
  y_fit.resize(n);
  for (int i = 0; i < n; i++)
    y_fit[i] =
        copysignf(expf(lna_calc), a_approx) * expf(b_calc * x[i]) + c_approx;
}

double ExpFit::CalcFitError(double c_approx)
{
  vector<double> ee(n - 1);
  for (int i = 0; i != n - 1; ++i) {
    ee[i] = log((y[i + 1] - c_approx) / (y[i] - c_approx));
  }

  vector<double> growthFactor(n - 1);
  cout << "growthFactor:\n\t";
  for (size_t i = 0; i != growthFactor.size(); ++i) {
    growthFactor[i] = expf(ee[i] / dx[i]);
    cout << growthFactor[i] << " ";
  }

  double ee_sum = std::accumulate(ee.begin(), ee.end(), 0.0);
  double avgGrowthFactor = expf(ee_sum / (x.back() - x.front()));
  cout << "\navgGrowthFactor: \n\t" << avgGrowthFactor << endl;

  double deviation = 0;
  for (size_t i = 0; i != growthFactor.size(); ++i) {
    deviation += std::abs(avgGrowthFactor - growthFactor[i]);
  }
  cout << "\ndeviation: " << deviation << endl;
  return deviation;
}

bool ExpFit::AbortCalc(double c_approx)
{
  auto static min = min_element(y.begin(), y.end());
  auto static max = max_element(y.begin(), y.end());
  bool abort = (c_approx >= *min) and (c_approx <= *max);
  if (abort) {
    cout << "========== abort,  c_approx : " << c_approx << "\t min: " << *min
         << "\t max: " << *max << endl;
  }
  return abort;
}

void ExpFit::PrintFit()
{
  const size_t max_print = 10;
  // original
  cout << "original:";
  for (size_t i = 0; i < min(max_print, y.size()); ++i)
    cout << "\n\t" << y.at(i);
  // clang-format off
  cout << "\nparameters:"
       << "\n\ta: " << a
       << "\n\tb: " << b
       << "\n\tc: " << c
       << "\n\ty = " << a << " * e^" << b
       << "x + " << c << endl;
  // clang-format on
  // fitted
  cout << "\nfitted:";
  for (size_t i = 0; i < min(max_print, y_fit.size()); ++i)
    cout << "\n\t" << y_fit.at(i);
  // clang-format off
  cout << "\nThe exponential fit is:"
       << "\n\ta: " << copysignf(expf(lna_calc), a_approx)
       << "\n\tb: " << b_calc
       << "\n\tc: " << c_calc
       << "\n\ty = " << copysignf(expf(lna_calc), a_approx) << " * e^" << b_calc
       << "x + " << c_calc << endl;
  // clang-format on
}
