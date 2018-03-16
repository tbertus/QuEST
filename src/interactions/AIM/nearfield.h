#ifndef NEARFIELD_H
#define NEARFIELD_H

#include "aim_base.h"

namespace AIM {
  class Nearfield;
}

class AIM::Nearfield final : public AimBase {
 public:
  Nearfield(const std::shared_ptr<const DotVector>,
            const std::shared_ptr<const Integrator::History<Eigen::Vector2cd>>,
            const int,
            const int,
            const double,
            const double,
            std::shared_ptr<const Grid>,
            std::shared_ptr<const Expansions::ExpansionTable>,
            Expansions::ExpansionFunction,
            Normalization::SpatialNorm);
  ~Nearfield() = default;

  const ResultArray &evaluate(const int) final;
  std::vector<std::pair<int, int>> make_pair_list(const int) const;

 private:
  std::vector<std::pair<int, int>> interaction_pairs_;
  std::array<int, 2> shape_;
  boost::multi_array<cmplx, 2> coefficients_;

  boost::multi_array<cmplx, 2> build_coefficient_table(
      const std::array<int, 2> &, const int) const;
};

#endif
