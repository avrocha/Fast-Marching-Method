#include <cstdint>

#include <fmm/solver.hpp> 

#include "H5Cpp.h"

namespace io{
    
    fmm::function<fmm::data_t> read(std::string const& filename, std::string const& dataset_name);    
}