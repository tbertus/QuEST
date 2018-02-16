#include <boost/test/unit_test.hpp>

#include "interactions/AIM/expansion.h"
#include "interactions/AIM/grid.h"

BOOST_AUTO_TEST_SUITE(EXPANSIONS)

struct PARAMETERS {
  int expansion_order;
  Eigen::Array3d grid_spacing;
  PARAMETERS() : expansion_order(1), grid_spacing(0.5, 1.0, 2.0){};
};

BOOST_FIXTURE_TEST_CASE(ONE_POINT_ON_GRID, PARAMETERS)
{
  std::shared_ptr<DotVector> dots = std::make_shared<DotVector>(
      DotVector{QuantumDot(Eigen::Array3d::Zero())});
  AIM::Grid grid(grid_spacing, expansion_order, *dots);

  auto expansions =
      AIM::Expansions::LeastSquaresExpansionSolver(grid).table(*dots);

  for(int i = 1; i < 8; ++i) {
    BOOST_CHECK_EQUAL(expansions[0][i].weight, 0);
  }
}

BOOST_FIXTURE_TEST_CASE(ONE_POINT_EXPANSION_OFF_GRID, PARAMETERS)
{
  std::shared_ptr<DotVector> dots =
      std::make_shared<DotVector>(DotVector{QuantumDot(grid_spacing / 2.0)});
  AIM::Grid grid(grid_spacing, expansion_order, *dots);
  auto expansions =
      AIM::Expansions::LeastSquaresExpansionSolver(grid).table(*dots);

  for(int i = 0; i < 8; ++i) {
    BOOST_CHECK_EQUAL(expansions[0][i].weight, 1.0 / 8);
  }
}

BOOST_FIXTURE_TEST_CASE(TWO_POINTS_ON_GRID, PARAMETERS)
{
  std::shared_ptr<DotVector> dots = std::make_shared<DotVector>(
      DotVector{QuantumDot(grid_spacing * Eigen::Array3d::Zero()),
                QuantumDot(grid_spacing * Eigen::Array3d(10, 1, 1))});
  AIM::Grid grid(grid_spacing, expansion_order, *dots);
  auto expansions =
      AIM::Expansions::LeastSquaresExpansionSolver(grid).table(*dots);

  enum PointIndex { POINT_0, POINT_1 };
  BOOST_CHECK_EQUAL(expansions[POINT_0][0].weight, 1);
  BOOST_CHECK_EQUAL(expansions[POINT_1][0].weight, 1);

  for(int i = 1; i < 8; ++i) {
    BOOST_CHECK_EQUAL(expansions[POINT_0][i].weight, 0);
    BOOST_CHECK_EQUAL(expansions[POINT_1][i].weight, 0);
  }
}

BOOST_FIXTURE_TEST_CASE(TWO_POINTS_OFF_GRID, PARAMETERS)
{
  std::shared_ptr<DotVector> dots = std::make_shared<DotVector>(
      DotVector{QuantumDot(grid_spacing / 2.0),
                QuantumDot(grid_spacing * Eigen::Array3d(10.5, 0.5, 0.5))});
  AIM::Grid grid(grid_spacing, expansion_order, *dots);
  auto expansions =
      AIM::Expansions::LeastSquaresExpansionSolver(grid).table(*dots);

  enum PointIndex { POINT_0, POINT_1 };
  for(int i = 0; i < 8; ++i) {
    BOOST_CHECK_CLOSE(expansions[POINT_0][i].weight, 1.0 / 8, 1e-12);
    BOOST_CHECK_CLOSE(expansions[POINT_1][i].weight, 1.0 / 8, 1e-12);
  }
}

BOOST_AUTO_TEST_CASE(CHEBYSHEV)
{
  AIM::Grid grid(Eigen::Array3d(1, 1, 1), 1, Eigen::Array3i(2, 2, 2),
                 Eigen::Vector3i::Zero());

  AIM::Expansions::LeastSquaresExpansionSolver LSE(grid);
  auto cheb_pts = Math::chebyshev_points(2);
  auto cheb_table = LSE.chebyshev_table(cheb_pts);

  BOOST_CHECK_EQUAL(cheb_table[0][0][0][0], 1);
  BOOST_CHECK_EQUAL(cheb_table[0][2][2][2], 1);

  for(int w = 0; w < 8; ++w) {
    BOOST_CHECK_CLOSE(cheb_table[w][1][1][1], 1.0 / 8.0, 1e-12);
  }
}

BOOST_AUTO_TEST_SUITE_END()  // EXPANSIONS
