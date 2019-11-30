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
  GenTestData(n, a, b, c);
  PrintTestData(n, a, b, c);
  CalcFit();
}

void ExpFit::GenTestData(double n, double a, double b, double c)
{
  x.resize(n);
  iota(x.begin(), x.end(), 0);

  y.resize(n);
  for (auto vi = y.begin(); vi != y.end(); ++vi) {
    auto i = std::distance(y.begin(), vi);
    *vi = a * expf(b * i) + c;
  }
}

void ExpFit::PrintTestData(double n, double a, double b, double c)
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

void ExpFit::SetY(vector<double> y)
{
  this->y = y;
  n = y.size();
}

void ExpFit::SetX(vector<double> x) { this->x = x; }

void ExpFit::SetX()
{
  x.resize(y.size());
  iota(x.begin(), x.end(), 0);
}

double ExpFit::CalcInitDeltaC()
{
  return a_approx > 0 ? *(std::min_element(y.begin(), y.end())) - c_approx
                      : *(std::max_element(y.begin(), y.end())) - c_approx;
}

void ExpFit::CalcFit()
{
  c_approx = CalcApproxC();
  double deltaC = CalcInitDeltaC();

  double min_error = CalcFitError(c_approx);
  int step = 0;
  PrintFitError(c_approx, deltaC, min_error, min_error, step);

  for (; std::abs(deltaC) > 10e-8; ++step) {
    double c_tmp = c_approx + deltaC;
    if (AbortCalc(c_tmp)) {
      deltaC /= 2;
      continue;
    }
    double error = CalcFitError(c_tmp);

    if (error < min_error) {
      PrintFitError(c_tmp, deltaC, error, min_error, step);
      min_error = error;
      c_approx = c_tmp;
    } else {
      deltaC /= -2;
    }
  }
  c_calc = c_approx;
  CalcFitAB(c_calc);
  PrintInputData();
  PrintFitData();
  PrintFitParameter();
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
  for (size_t i = 0; i != growthFactor.size(); ++i) {
    growthFactor[i] = expf(ee[i] / dx[i]);
  }

  double ee_sum = std::accumulate(ee.begin(), ee.end(), 0.0);
  double avgGrowthFactor = expf(ee_sum / (x.back() - x.front()));

  double deviation = 0;
  for (size_t i = 0; i != growthFactor.size(); ++i) {
    deviation += std::abs(avgGrowthFactor - growthFactor[i]);
  }
  return deviation;
}

bool ExpFit::AbortCalc(double c_approx)
{
  auto static min = min_element(y.begin(), y.end());
  auto static max = max_element(y.begin(), y.end());
  bool abort = (c_approx >= *min) and (c_approx <= *max);
  if (abort) {
    cout << "\n========== c : " << c_approx << "\t min: " << *min
         << "\t max: " << *max << "\t abort ==========" << endl;
  }
  return abort;
}

void ExpFit::PrintInputData()
{
  const size_t max_print = 10;
  cout << "\ninput data:";
  for (size_t i = 0; i < min(max_print, y.size()); ++i)
    cout << "\n\t" << y.at(i);
  cout << endl;
}

void ExpFit::PrintFitParameter()
{
  // clang-format off
  cout << "\nThe exponential fit is:"
       << "\n\ty = " << copysignf(expf(lna_calc), a_approx) << " * e^" << b_calc
       << "x + " << c_calc << endl << endl;
  // clang-format on
}

void ExpFit::PrintFitData()
{
  const size_t max_print = 10;
  cout << "\nfitted data:";
  for (size_t i = 0; i < min(max_print, y_fit.size()); ++i)
    cout << "\n\t" << y_fit.at(i);
  cout << endl;
}

void ExpFit::PrintFitError(double c_tmp, double deltaC, double error,
                           double min_error, int step)
{
  // clang-format off
  cout << "\n=== c_tmp = " << c_tmp
       << "\t    error = " << error
       << "\t   deltaC = " << deltaC
       << "\n   c_prev = " << c_approx
       << "\tmin_error = " << min_error
       << "\t     step = " << step
       << endl;
  // clang-format on
}
