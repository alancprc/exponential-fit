#pragma once
#include <tuple>
#include <vector>

class ExpFit
{
 public:
  using ResultType = std::tuple<double, double, double>;
  void SetTest(int n, double a, double b, double c);
  void SetY(std::vector<double> y);
  void SetX(std::vector<double> x);
  void SetX();
  void CalcFit();
  ResultType GetResult();
  double GetA();
  double GetB();
  double GetC();
  void Debug();

 private:
  void GenTestData(double n, double a, double b, double c);
  void PrintTestData(double n, double a, double b, double c);

  double CalcApproxC();
  double CalcInitDeltaC();
  void CalcFitAB(double c_approx);
  double CalcFitError(double c_approx);

  bool AbortCalc(double c_approx);
  void PrintFitParameter();
  void PrintFitData();
  void PrintFitError(double c_tmp, double deltaC, double error,
                     double min_error, int step);
  void PrintInputData();

  int n;
  double a_tmp;
  double b_tmp;
  double lna_calc;
  double b_calc;
  double c_calc;
  double a_approx;
  double c_approx;
  bool debug = false;
  std::vector<double> y;
  std::vector<double> x;
  std::vector<double> dx;
  std::vector<double> dy;
  std::vector<double> cx;
  std::vector<double> dq;
  std::vector<double> y_fit;
};
