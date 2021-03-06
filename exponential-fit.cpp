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
  if (not debug) return;

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

void ExpFit::SetY(const vector<double>& y)
{
  this->y = y;
  n = y.size();
}

void ExpFit::SetY(const double* begin, const double* end)
{
  y.assign(begin, end);
  n = y.size();
}

void ExpFit::SetX(const vector<double>& x) { this->x = x; }

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
  CalcApproxC();
  double deltaC = CalcInitDeltaC();

  double min_error = CalcFitError(c_approx);
  int step = 0;
  PrintFitError(c_approx, deltaC, min_error, min_error, step);

  c_calc = c_approx;
  for (; std::abs(deltaC) > 10e-8; ++step) {
    double c_tmp = c_calc + deltaC;
    if (AbortCalc(c_tmp)) {
      deltaC /= 2;
      continue;
    }
    double error = CalcFitError(c_tmp);

    if (error < min_error) {
      PrintFitError(c_tmp, deltaC, error, min_error, step);
      min_error = error;
      c_calc = c_tmp;
    } else {
      deltaC /= -2;
    }
  }
  CalcFitAB(c_calc);
  PrintInputData();
  PrintFitData();
  PrintFitParameter();
}

ExpFit::ResultType ExpFit::GetResult()
{
  return make_tuple(a_calc, b_calc, c_calc);
}

double ExpFit::GetA() { return a_calc; }

double ExpFit::GetB() { return b_calc; }

double ExpFit::GetC() { return c_calc; }

void ExpFit::DebugOn() { debug = true; }

void ExpFit::DebugOff() { debug = false; }

double ExpFit::CalcApproxC()
{
  vector<double> dx(n - 1, 1);
  vector<double> dy(n - 1);
  vector<double> cx(n - 1);
  vector<double> dq(n - 1);

  for (int i = 0; i != n - 1; ++i) {
    dx[i] = x[i + 1] - x[i];
    dy[i] = y[i + 1] - y[i];
    cx[i] = (x[i + 1] + x[i]) / 2;
    dq[i] = dy[i] / dx[i];
  }

  vector<double> b_c(n - 2);  // b calculated
  for (int i = 0; i != n - 2; ++i) {
    b_c[i] = log(dq[i + 1] / dq[i]) / (cx[i + 1] - cx[i]);
  }
  double b_approx = std::accumulate(b_c.begin(), b_c.end(), 0.0) / b_c.size();

  vector<double> a_c(n - 1);  // a calculated
  for (int i = 0; i != n - 1; ++i) {
    a_c[i] = dy[i] / (expf(b_approx * x[i + 1]) - expf(b_approx * x[i]));
  }
  a_approx = std::accumulate(a_c.begin(), a_c.end(), 0.0) / a_c.size();

  vector<double> c_c(n);  // c calculated
  for (int i = 0; i != n; ++i) {
    c_c[i] = y[i] - a_approx * expf(b_approx * x[i]);
  }
  // in case that c_c veries greatly, e.g. [1e-10, 1e-20],
  // the median value is better than mean value.
  auto nth = c_c.begin() + c_c.size() / 2;
  std::nth_element(c_c.begin(), nth, c_c.end());
  c_approx = *nth;

  if (debug) {
    cout << "a_approx: " << a_approx << endl;
    cout << "b_approx: " << b_approx << endl;
    cout << "c_approx: " << c_approx << endl;
  }
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
  double lna_calc = (x2sum * ysum - xsum * xysum) / (x2sum * n - xsum * xsum);
  a_calc = copysignf(expf(lna_calc), a_approx);

  // to calculate y(fitted) at given x points
  y_fit.resize(n);
  for (int i = 0; i < n; i++)
    y_fit[i] =
        a_calc * expf(b_calc * x[i]) + c_approx;
}

double ExpFit::CalcFitError(double c_approx)
{
  CalcFitAB(c_approx);
  double error = 0;
  for(size_t i = 0; i != y.size(); ++i) {
    error += std::abs(y.at(i) - y_fit.at(i));
  }
  if (debug) cout << "error : " << error << endl;

  return error;
}

bool ExpFit::AbortCalc(double c_approx)
{
  auto static min = min_element(y.begin(), y.end());
  auto static max = max_element(y.begin(), y.end());
  bool abort = (c_approx >= *min) and (c_approx <= *max);
  if (debug and abort) {
    cout << "\n========== c : " << c_approx << "\t min: " << *min
         << "\t max: " << *max << "\t abort ==========" << endl;
  }
  return abort;
}

void ExpFit::PrintInputData()
{
  if (not debug) return;

  cout << "\ninput data:";
  for (size_t i = 0; i != y.size(); ++i)
    cout << "\n\t" << x.at(i) << "\t" << y.at(i);
  cout << endl;
}

void ExpFit::PrintFitParameter()
{
  if (not debug) return;

  // clang-format off
  cout << "\nThe exponential fit is:"
       << "\n\ty = " << a_calc << " * e^" << b_calc
       << "x + " << c_calc << endl << endl;
  // clang-format on
}

void ExpFit::PrintFitData()
{
  if (not debug) return;

  cout << "\nfitted data:";
  for (size_t i = 0; i != y_fit.size(); ++i)
    cout << "\n\t" << x.at(i) << "\t" << y.at(i) << "\t" << y_fit.at(i);
  cout << endl;
}

void ExpFit::PrintFitError(double c_tmp, double deltaC, double error,
                           double min_error, int step)
{
  if (not debug) return;

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
