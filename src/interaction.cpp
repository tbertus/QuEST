#include "interaction.h"

Interaction::Interaction(const Eigen::Vector3d &dr) 
{
  double dist = dr.norm();
  double dimensionless_delay = dist/(config.c0*config.dt);

  split_delay.second = std::modf(dimensionless_delay, &split_delay.first);
  interp = UniformLagrangeSet(split_delay.second); 

  dyad = dr*dr.transpose()/std::pow(dist, 2);
}

std::pair<Eigen::Vector3d, Eigen::Vector3d>
  Interaction::operator()(const QuantumDot &d1, const QuantumDot &d2) const
{
  Eigen::Vector3d field1(Eigen::Vector3d::Zero()),
                  field2(Eigen::Vector3d::Zero());

  return std::pair<Eigen::Vector3d, Eigen::Vector3d>(field1, field2);
}
