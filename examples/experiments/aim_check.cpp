#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>

#include "interactions/AIM/aim_interaction.h"
#include "interactions/direct_interaction.h"
#include "math_utils.h"

using dbl = std::numeric_limits<double>;

class Gaussian {
 public:
  Gaussian(double mu, double sigma) : mu(mu), sigma(sigma){};
  double operator()(double t) const { return gaussian((t - mu) / sigma); }
 private:
  double mu, sigma;
};

int main()
{
  const int interpolation_order = 4, expansion_order = 1, num_steps = 1024;
  const double c = 1, dt = 1, total_time = num_steps * dt, omega = 0;
  const Gaussian source(total_time / 2.0, total_time / 12.0);

  const Eigen::Array3d spacing(1, 1, 1);

  auto dots = std::make_shared<DotVector>();
  dots->push_back(QuantumDot({0.5, 0.5, 0.5}, omega, {0, 0}, {0, 0, 1}));
  dots->push_back(QuantumDot({0.5, 0.5, 10.5}, omega, {0, 0}, {0, 0, 1}));

  const int num_dots = dots->size();
  auto history = std::make_shared<Integrator::History<Eigen::Vector2cd>>(
      num_dots, 10, num_steps);
  for(int d = 0; d < num_dots; ++d) {
    for(int t = -10; t < num_steps; ++t) {
      history->array_[d][t][0](RHO_01) = source(t * dt);
    }
  }

  //Propagation::RotatingEFIE efie(c, 1, omega);
  //int nt = AIM::Grid(spacing, expansion_order, *dots).max_transit_steps(c, dt) +
           //interpolation_order;

  //AIM::Interaction aim(dots, history, efie, spacing, interpolation_order,
                       //expansion_order, 1, c, dt,
                       //AIM::Expansions::RotatingEFIE(nt, c, dt, omega),
                       //AIM::normalization::Helmholtz(omega / c));
  //DirectInteraction direct(dots, history, efie, interpolation_order, c, dt);

  Propagation::Identity<cmplx> identity;
  int nt = AIM::Grid(spacing, expansion_order, *dots).max_transit_steps(c, dt) +
           interpolation_order;

  AIM::Interaction aim(dots, history, identity, spacing, interpolation_order,
                       expansion_order, 1, c, dt,
                       AIM::Expansions::Retardation(nt),
                       AIM::normalization::unit);
  DirectInteraction direct(dots, history, identity, interpolation_order, c, dt);

  std::ofstream comparison("comparison.dat");
  comparison.precision(17);
  for(int t = 0; t < num_steps; ++t) {
    std::cout << t << " ";
    comparison << t * dt << " " << direct.evaluate(t).transpose() << " "
               << aim.evaluate(t).transpose() << std::endl;
  }

  return 0;
}
