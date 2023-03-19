//
//  Copyright © 2023-Present, Arkin Terli. All rights reserved.
//
//  NOTICE:  All information contained herein is, and remains the property of Arkin Terli.
//  The intellectual and technical concepts contained herein are proprietary to Arkin Terli
//  and may be covered by U.S. and Foreign Patents, patents in process, and are protected by
//  trade secret or copyright law. Dissemination of this information or reproduction of this
//  material is strictly forbidden unless prior written permission is obtained from Arkin Terli.

#include "FFNN.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>


bool FFNN::Init(const std::vector<int> & layers)
{
    if (layers.size() < 3)
    {
        // Needs at least two layers.
        return false;
    }

    // Create a random number generator.
    std::random_device  randDevice;
    std::mt19937        randEngine(randDevice());  // Random Device used to change seed each time.
    std::normal_distribution<double>  dist(0.0, 1.0);
    auto randGen = [&randEngine, &dist](){ return dist(randEngine); };

    for (size_t i=0; i<layers.size()-1; ++i)
    {
        // NullaryExpr() creates NxM matrix and uses randGen() to assign random values.
        m_weights.emplace_back(Eigen::MatrixXd::NullaryExpr(layers[i], layers[i+1], randGen));
        m_biases.emplace_back(Eigen::MatrixXd::NullaryExpr(1, layers[i+1], randGen));
    }

    return true;
}


Eigen::MatrixXd FFNN::Forward(const Eigen::MatrixXd & input)
{
    Eigen::MatrixXd H = input;
    for (size_t i=0; i<m_weights.size(); ++i)
    {
        H = H * m_weights[i] + m_biases[i];
        // Apply activation function
        H = H.unaryExpr([](double x) { return std::tanh(x); });
    }

    return H;
}


std::vector<double> FFNN::GetAllWeightsAsVector()
{
    std::vector<double>  weightsVec;

    for (const auto & weight : m_weights)
    {
        auto currWeightsSize = static_cast<ssize_t>(weightsVec.size());
        weightsVec.resize(weightsVec.size() + weight.size());
        std::copy(weight.data(), weight.data() + weight.size(), weightsVec.begin() + currWeightsSize);
    }

    return weightsVec;
}


bool FFNN::SetAllWeightsFromVector(const std::vector<double> & weightsVector)
{
    size_t totalSize = 0;
    for (const auto & weight : m_weights)
    {
        totalSize += weight.size();
    }

    if (weightsVector.size() != totalSize)
    {
        return false;
    }

    size_t copiedSize = 0;
    for (auto & weight : m_weights)
    {
        std::copy(weightsVector.data() + copiedSize, weightsVector.data() + copiedSize + weight.size(), weight.data());
        copiedSize += weight.size();
    }

    return true;
}


bool FFNN::Save(const std::string & filename)
{
    std::ofstream  file(filename, std::ios::binary);

    if (!file.is_open())
    {
        return false;
    }

    // Writes an int64 value to the file.
    auto WriteInt64  = [&](int64_t val) { file.write(reinterpret_cast<const char*>(&val), sizeof(int64_t)); };

    // Writes a MatrixXd to the file.
    auto WriteMatrix = [&](const Eigen::MatrixXd & mat)
    {
        int64_t rows = mat.rows();
        int64_t cols = mat.cols();
        file.write(reinterpret_cast<const char*>(&rows), sizeof(rows));
        file.write(reinterpret_cast<const char*>(&cols), sizeof(cols));
        file.write(reinterpret_cast<const char*>(mat.data()), rows * cols * sizeof(double));
    };

    // Write input layer size.
    WriteInt64(m_weights[0].rows());

    // Write number of hidden layers + output layer
    WriteInt64(m_weights.size());

    // Write all hidden layer sizes + output layer
    for (const auto & weight : m_weights)
    {
        WriteInt64(weight.cols());
    }

    // Write all weights matrices.
    for (const auto & weight : m_weights)
    {
        WriteMatrix(weight);
    }

    // Write all bias matrices.
    for (const auto & bias : m_biases)
    {
        WriteMatrix(bias);
    }

    return true;
}


bool FFNN::Load(const std::string & filename)
{
    std::ifstream  file(filename, std::ios::binary);

    if (!file.is_open())
    {
        return false;
    }

    // Reads an int64 value from the file.
    auto ReadInt64 = [&](int64_t & val) { file.read(reinterpret_cast<char*>(&val), sizeof(val)); };

    // Reads a MatrixXd from the file.
    auto ReadMatrix = [&](Eigen::MatrixXd & mat)
    {
        int64_t rows;
        int64_t cols;
        ReadInt64(rows);
        ReadInt64(cols);
        mat.resize(rows, cols);
        file.read(reinterpret_cast<char*>(mat.data()), rows * cols * sizeof(double));
    };

    std::vector<int>  layers;

    // Read input layer size.
    int64_t inputSize;
    ReadInt64(inputSize);
    layers.emplace_back(inputSize);

    // Read number of hidden layers + output layer
    int64_t numHiddenLayers;
    ReadInt64(numHiddenLayers);

    // Read all hidden layer sizes.
    for (int i=0; i<numHiddenLayers; ++i)
    {
        int64_t hiddenLayerSize = 0;
        ReadInt64(hiddenLayerSize);
        layers.emplace_back(hiddenLayerSize);
    }

    Init(layers);

    // Read all weights matrices.
    for (auto & weight : m_weights)
    {
        ReadMatrix(weight);
    }

    // Read all bias matrices.
    for (auto & bias : m_biases)
    {
        ReadMatrix(bias);
    }

    return true;
}


void FFNN::PrintAll()
{
    // Read all weights matrices.
    for (const auto & weight : m_weights)
    {
        std::cout << "Weight(" << weight.rows() << "," << weight.cols() << "):" << std::endl << weight << std::endl;
    }

    // Read all bias matrices.
    for (auto & bias : m_biases)
    {
        std::cout << "Bias(" << bias.rows() << "," << bias.cols() << "):" << std::endl << bias << std::endl;
    }
}
