#include <silo.h>
#include <Eigen/Dense>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using std::cout;
using std::endl;

constexpr int nmax = 10000;

typedef Eigen::Matrix<double, Eigen::Dynamic, 3> PosArray;  // Column major!

class SiloFile {
 public:
  SiloFile(const std::string &fname)
  {
    db_file_ptr =
        DBCreate(fname.data(), DB_CLOBBER, DB_LOCAL, "bloch", DB_HDF5);
  }

  ~SiloFile() { DBClose(db_file_ptr); }
  void write_point_mesh(const PosArray &coords, const std::string &mesh_id)
  {
    double *data_ptr = new double[coords.size()];
    Eigen::Map<PosArray>(data_ptr, coords.rows(), coords.cols()) = coords;

    double *coord_ptrs[] = {&data_ptr[0], &data_ptr[coords.rows()],
                            &data_ptr[2 * coords.rows()]};

    DBPutPointmesh(db_file_ptr, mesh_id.data(), 3, coord_ptrs, coords.rows(),
                   DB_DOUBLE, NULL);
  }

  void write_point_data(std::vector<double> &data, const std::string &name,
                        const std::string &mesh_name)
  {
    DBPutPointvar1(db_file_ptr, name.data(), mesh_name.data(), data.data(),
                   data.size(), DB_DOUBLE, NULL);
  }

 private:
  DBfile *db_file_ptr;
};

PosArray read_coords(const std::string &);

int main()
{
  auto xyz(read_coords("/home/connor/Scratch/high_density/high_density.cfg"));
  std::ifstream datafile("/home/connor/Scratch/high_density/output.dat");

  std::string line;
  for(int t = 0; t < 8000; ++t) {
    std::getline(datafile, line);  // Must advance input!
    if(t % 10 != 0) continue;

    std::istringstream ss(line);
    ss.ignore(256, ' ');  // Skip time token

    std::vector<double> pop(nmax), pol_re(nmax), pol_im(nmax);
    for(int i = 0; i < nmax; ++i) {
      double dummy;
      ss >> pop[i] >> dummy >> pol_re[i] >> pol_im[i];
    }

    std::stringstream namestring;
    namestring << "vis_" << std::setw(4) << std::setfill('0') << t << ".silo";

    SiloFile silo(namestring.str());
    silo.write_point_mesh(xyz, "coordinates");
    silo.write_point_data(pop, "population", "coordinates");
    silo.write_point_data(pol_re, "re_polarization", "coordinates");
    silo.write_point_data(pol_im, "im_polarization", "coordinates");
  }

  return 0;
}

PosArray read_coords(const std::string &fname)
{
  std::ifstream ifs(fname);
  if(!ifs) {
    throw std::runtime_error("Could not open " + fname);
  }

  PosArray result(nmax, 3);
  std::string line;

  for(int i = 0; i < nmax; ++i) {
    std::getline(ifs, line);
    std::istringstream ss(line);
    ss >> result(i, 0) >> result(i, 1) >> result(i, 2);
  }

  return result;
}
