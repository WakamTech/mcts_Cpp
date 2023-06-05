#ifndef __MODEL_HPP__
#define __MODEL_HPP__

#include <torch/torch.h>
#include <iostream>

// Define a simple feedforward neural network
struct NetImpl : torch::nn::Module
{
    NetImpl()
    {
        fc1 = register_module("fc1", torch::nn::Linear(9, 128));
        fc2 = register_module("fc2", torch::nn::Linear(128, 128));
        fc3 = register_module("fc3", torch::nn::Linear(128, 9));
    }

    torch::Tensor forward(torch::Tensor x)
    {
        x = torch::relu(fc1->forward(x));
        x = torch::relu(fc2->forward(x));
        x = torch::sigmoid(fc3->forward(x));
        return x;
    }

    torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr};
};
TORCH_MODULE(Net);

#endif // __MODEL_HPP__