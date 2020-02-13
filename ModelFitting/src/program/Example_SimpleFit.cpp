/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/**
 * @file Example_SimpleFit.cpp
 * @date April 12, 2019
 * @author Alejandro Alvarez
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <AlexandriaKernel/memory_tools.h>
#include <ModelFitting/Parameters/NeutralConverter.h>
#include <ModelFitting/Engine/LeastSquareEngineManager.h>
#include <ModelFitting/Engine/EngineParameterManager.h>
#include <ModelFitting/Parameters/NormalizedConverter.h>
#include "utils.h"

using namespace ModelFitting;
using Euclid::make_unique;

class ExpResidualProvider: public ResidualBlockProvider {
private:
  std::shared_ptr<BasicParameter> m_A, m_lambda, m_b;
  std::vector<double> m_y, m_t;

public:
  ExpResidualProvider(std::shared_ptr<BasicParameter> a, std::shared_ptr<BasicParameter> lambda,
      std::shared_ptr<BasicParameter> b, const std::vector<double>& y, const std::vector<double>& t)
      : m_A{a}, m_lambda{lambda}, m_b{b}, m_y{y}, m_t{t} {
  }

  virtual ~ExpResidualProvider() = default;

  std::size_t numberOfResiduals() const override {
    return m_y.size();
  }

  void populateResidualBlock(IterType iter) override {
    std::cout << std::fixed << std::setprecision(4) << "A=" << m_A->getValue() << "\tlambda=" << m_lambda->getValue() << "\tb=" << m_b->getValue();
    double e = 0;
    for (size_t i = 0; i < m_y.size(); ++i) {
      double Yi = m_b->getValue() + m_A->getValue() * exp(-m_lambda->getValue() * m_t[i]);
      *iter = Yi - m_y[i];
      e += *iter * *iter;
      ++iter;
    }
    std::cout << std::scientific << "\tsum(e^2)=" << e << std::endl;
  }
};

/*
 * Adapted from
 * https://www.gnu.org/software/gsl/doc/html/nls.html#exponential-fitting-example
 * "Simple" exponential function, so it is easier to understand and visualize what's going on
 */
int main(int argc, char **argv) {
  std::string engine_impl("levmar");
  if (argc > 1) {
    engine_impl = argv[1];
  }

  // Generate data points, using the exponential function
  int n = 100;
  double tmax = 40.;
  std::vector<double> y(n), t(n);

  for (int i = 0; i < n; ++i) {
    double ti = i * tmax / (n - 1.0);
    double yi = 1. + 5. * exp(-0.1 * ti);
    t[i] = ti;
    y[i] = yi;
  }

  // Initialize and run engine
  auto A = std::make_shared<EngineParameter>(2., make_unique<NeutralConverter>());
  auto lambda = std::make_shared<EngineParameter>(1., make_unique<NeutralConverter>());
  auto b = std::make_shared<EngineParameter>(0., make_unique<NeutralConverter>());

  EngineParameterManager manager {};
  manager.registerParameter(A);
  manager.registerParameter(lambda);
  manager.registerParameter(b);

  ResidualEstimator res_estimator {};
  res_estimator.registerBlockProvider(make_unique<ExpResidualProvider>(A, lambda, b, y, t));

  std::cout << "Using engine " << engine_impl << std::endl;
  auto engine = LeastSquareEngineManager::create(engine_impl);
  auto t1 = std::chrono::steady_clock::now();
  auto solution = engine->solveProblem(manager, res_estimator);
  auto t2 = std::chrono::steady_clock::now();

  std::cout << std::defaultfloat << std::endl;
  std::cout << "Time of fitting: " << std::chrono::duration<double, std::milli>(t2 - t1).count() << " ms" << std::endl;
  std::cout << "A (5.)      : " << A->getValue() << std::endl;
  std::cout << "lambda (0.1): " << lambda->getValue() << std::endl;
  std::cout << "b (1.)      : " << b->getValue() << std::endl;
  printLevmarInfo(boost::any_cast<std::array<double, 10>>(solution.underlying_framework_info));
}
