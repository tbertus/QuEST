#ifndef AIM_FFT_H
#define AIM_FFT_H

#include "expansion.h"
#include "fourier.h"
#include "grid.h"
#include "interactions/history_interaction.h"
#include "normalization.h"
#include "spacetime.h"

namespace AIM {
  class AimInteraction;
}

class AIM::AimInteraction final : public HistoryInteraction {
 public:
  AimInteraction(const int, const Grid, normalization::SpatialNorm);
  AimInteraction(
      const std::shared_ptr<const DotVector>,
      const std::shared_ptr<const Integrator::History<Eigen::Vector2cd>>,
      const int,
      const double,
      const double,
      const Grid,
      const Expansions::ExpansionTable &,
      Expansions::ExpansionFunction,
      normalization::SpatialNorm);

  const ResultArray &evaluate(const int) final;

 private:
  Grid grid;
  Expansions::ExpansionTable expansion_table;
  Expansions::ExpansionFunction expansion_function;
  normalization::SpatialNorm normalization;
  std::array<int, 4> toeplitz_dimensions, circulant_dimensions;

  // This corresponds to delta(t - R/c)/R and thus holds *scalar* quantities
  spacetime::vector<cmplx> fourier_table;

  // These correspond to J and E and thus hold *vector* quantities
  spacetime::vector3d<cmplx> source_table, source_table_fft;
  spacetime::vector3d<cmplx> obs_table, obs_table_fft;

  TransformPair spatial_vector_transforms;

  void fill_source_table(const int);
  void propagate(const int);
  void fill_results_table(const int);

  spacetime::vector<cmplx> circulant_fourier_table();
  void fill_gmatrix_table(spacetime::vector<cmplx> &) const;
  TransformPair spatial_fft_plans();
};

#endif
