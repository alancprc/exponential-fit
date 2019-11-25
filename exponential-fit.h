#pragma  once
#include <vector>
#include <tuple>

class ExpFit
{
 public:
  using ResultType = std::tuple<double, double, double>;
  void SetTest(int n, double a, double b, double c);
  void SetY(std::vector<double> y);
  void SetX(std::vector<double> x);
  void SetX();
  /// calc the initial approximate of c
  void CalcFit();
  ResultType GetResult();
  double GetA();
  double GetB();
  double GetC();

 private:
  void GenTestData();
  void PrintTestData();

  double CalcApproxC();
  void CalcFitAB(double c_approx);
  double CalcFitError(double c_approx);

  bool AbortCalc(double c_approx);
  void PrintFit();

  int n;
  double a;
  double b;
  double c;
  double a_tmp;
  double b_tmp;
  double lna_calc;
  double b_calc;
  double c_calc;
  double a_approx;
  std::vector<double> y;
  std::vector<double> x;
  std::vector<double> dx;
  std::vector<double> dy;
  std::vector<double> cx;
  std::vector<double> dq;
  std::vector<double> y_fit;
};
