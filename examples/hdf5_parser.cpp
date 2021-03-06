#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "fmm/defs.hpp"
#include "hdf5_parser.hpp"

namespace io
{
    H5::PredType get_ds_type()
    {
        if(sizeof(fmm::data_t) == sizeof(float))
        {
            return H5::PredType::NATIVE_FLOAT;
        }
        else if(sizeof(fmm::data_t) == sizeof(double))
        {
            return H5::PredType::NATIVE_DOUBLE;
        }
    }

    fmm::function<fmm::data_t> read(std::string const& file_name,
                                    std::string const& dataset_name)
    {
        H5::H5File file(file_name, H5F_ACC_RDONLY);
        H5::DataSet dataset = file.openDataSet(dataset_name);

        H5::DataSpace dataspace = dataset.getSpace();
        int n_dimensions = dataspace.getSimpleExtentNdims();

        std::vector<hsize_t> dimensions(n_dimensions);
        dataspace.getSimpleExtentDims(dimensions.data());

        int n_points = dataspace.getSimpleExtentNpoints();

        std::vector<fmm::data_t> ds_values(n_points);
        dataset.read(ds_values.data(), get_ds_type());

        fmm::function<fmm::data_t> f(ds_values);

        return f;
    }

    void write(std::string const& file_name,
               std::string const& dataset_name,
               fmm::function<fmm::data_t> f)
    {
        // Truncates file, if it already exists, erasing all data previously
        // stored in the file.
        H5::H5File file(file_name, H5F_ACC_TRUNC);

        std::vector<hsize_t> dims = { f.dim_size[0], f.dim_size[1] };
        H5::DataSpace dataspace(DIM, dims.data());

        H5::DataSet dataset =
            file.createDataSet(dataset_name, get_ds_type(), dataspace);

        auto data_ref = f.get_data();

        dataset.write(data_ref, get_ds_type(), dataspace);
    }
}    // namespace io