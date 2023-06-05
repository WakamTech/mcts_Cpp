
#ifndef __CUSTOM_DATASET___
#define __CUSTOM_DATASET___

#include <iostream>
#include <filesystem>
#include <torch/torch.h>
#include <vector>
#include <string>


using namespace torch;

class CustomDataset : public data::datasets::Dataset<CustomDataset>
{

public:
    CustomDataset(const std::string& data_path, const std::string& target_path) {
    std::ifstream data_file(data_path);
    std::ifstream target_file(target_path);
    std::string line;

    while (std::getline(data_file, line)) {
        std::stringstream ss(line);
        std::vector<float> data_row;
        float value;
        while (ss >> value) {
            data_row.push_back(value);
            if (ss.peek() == ',') {
                ss.ignore();
            }
        }
        data_.push_back(torch::tensor(data_row));
    }

    while (std::getline(target_file, line)) {
        std::stringstream ss(line);
        std::vector<float> target_row;
        float target_value;
        while (ss >> target_value) {
            target_row.push_back(target_value);
            if (ss.peek() == ',') {
                ss.ignore();
            }
        }
        targets_.push_back(torch::tensor(target_row));
    }
}

torch::data::Example<> get(size_t index) {
    return {data_.at(index), targets_.at(index)};
}

torch::optional<size_t> size() const {
    return data_.size();
}

    private :
    std::vector<torch::Tensor> data_;
    std::vector<torch::Tensor> targets_;
};

#endif