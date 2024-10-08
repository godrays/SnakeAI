//
//  Copyright © 2023-Present, Arkin Terli. All rights reserved.
//
//  NOTICE:  All information contained herein is, and remains the property of Arkin Terli.
//  The intellectual and technical concepts contained herein are proprietary to Arkin Terli
//  and may be covered by U.S. and Foreign Patents, patents in process, and are protected by
//  trade secret or copyright law. Dissemination of this information or reproduction of this
//  material is strictly forbidden unless prior written permission is obtained from Arkin Terli.


// Project includes
#include "FFNN.hpp"
// External includes
// System includes
#include <algorithm>
#include <iostream>
#include <fstream>


FFNN::FFNN(const std::vector<int> & layers, const std::vector<ActivationType> & activations)
{
    Init(layers, activations);
}


FFNN::~FFNN()
{
    DeleteActivations();
}


void FFNN::Init(const std::vector<int> & layers, const std::vector<ActivationType> & activations)
{
    if (layers.size() < 3 || activations.size() < 2 || layers.size() - 1 != activations.size())
    {
        throw std::runtime_error("Layer configuration is not correct");
    }

    DeleteActivations();

    std::random_device  rndDev;
    m_rndEngine.seed(rndDev());

    auto getRandomNumber = [&](double min, double max)
    {
        return std::uniform_real_distribution<double>(min, max)(m_rndEngine);
    };
    auto randGen = [&getRandomNumber](){ return getRandomNumber(-1, 1); };

    // Create weights and biases for all layers except input layer.
    for (size_t i=0; i<layers.size()-1; ++i)
    {
        // NullaryExpr() creates NxM matrix and uses randGen() to assign random values.
        m_weights.emplace_back(Eigen::MatrixXd::NullaryExpr(layers[i], layers[i+1], randGen));
        m_biases.emplace_back(Eigen::MatrixXd::NullaryExpr(1, layers[i+1], randGen));
        // Create activation object per hidden layer and the output later (the last layer).
        m_activations.emplace_back(ActivationFactory::Create(activations[i]));
    }
}


Eigen::MatrixXd FFNN::Forward(const Eigen::MatrixXd & input)
{
    Eigen::MatrixXd H = input;
    for (size_t i=0; i<m_weights.size(); ++i)
    {
        H = H * m_weights[i] + m_biases[i];
        // Apply activation.
        H = m_activations[i]->Calculate(H);
    }

    return H ;
}


std::vector<double> FFNN::SerializeWeights()
{
    return SerializeMatrices(m_weights);
}


bool FFNN::DeserializeWeights(const std::vector<double> & weightsVector)
{
    return DeserializeMatrices(weightsVector, m_weights);
}


std::vector<double> FFNN::SerializeBiases()
{
    return SerializeMatrices(m_biases);
}


bool FFNN::DeserializeBiases(const std::vector<double> & biasesVector)
{
    return DeserializeMatrices(biasesVector, m_biases);
}


std::vector<double> FFNN::SerializeAllParameters()
{
    auto vec1 = SerializeMatrices(m_weights);
    auto vec2 = SerializeMatrices(m_biases);

    auto vec1Size = vec1.size();
    vec1.resize(vec1.size() + vec2.size());
    std::copy(vec2.begin(), vec2.end(), vec1.begin() + vec1Size);

    return vec1;
}


bool FFNN::DeserializeAllParameters(const std::vector<double> & vector)
{
    // Get the size of weights vector. The rest of them will be biases.
    std::size_t totalMatricesSize = 0;
    for (const auto & matrix : m_weights)
        totalMatricesSize += matrix.size();

    // Split the vector into two parts: weights and biases.
    std::vector<double>  weightsVec(vector.begin(), vector.begin() + totalMatricesSize);
    std::vector<double>  biasesVec(vector.begin() + totalMatricesSize, vector.end());

    return DeserializeWeights(weightsVec) && DeserializeBiases(biasesVec);
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

    // Write layers activation types.
    for (const auto activation : m_activations)
    {
        WriteInt64(static_cast<int64_t>(activation->GetType()));
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

    // Read activation types.
    std::vector<ActivationType> layerActivations;
    for (int64_t i=0; i<numHiddenLayers; ++i)
    {
        int64_t activationType;
        ReadInt64(activationType);
        layerActivations.emplace_back(static_cast<ActivationType>(activationType));
    }

    Init(layers, layerActivations);

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
    // Print all weights matrices.
    for (const auto & weight : m_weights)
    {
        std::cout << "Weight(" << weight.rows() << "," << weight.cols() << "):" << std::endl << weight << std::endl;
    }

    // Print all bias matrices.
    for (auto & bias : m_biases)
    {
        std::cout << "Bias(" << bias.rows() << "," << bias.cols() << "):" << std::endl << bias << std::endl;
    }
}


std::vector<double> FFNN::SerializeMatrices(const std::vector<Eigen::MatrixXd> & matrices)
{
    std::vector<double>  resultVec;

    for (const auto & matrix : matrices)
    {
        auto currWeightsSize = static_cast<ssize_t>(resultVec.size());
        resultVec.resize(resultVec.size() + matrix.size());
        std::copy(matrix.data(), matrix.data() + matrix.size(), resultVec.begin() + currWeightsSize);
    }

    return resultVec;
}


bool FFNN::DeserializeMatrices(const std::vector<double> & vector, std::vector<Eigen::MatrixXd> & matrices)
{
    size_t totalSize = 0;
    for (const auto & matrix : matrices)
    {
        totalSize += matrix.size();
    }

    if (vector.size() != totalSize)
    {
        return false;
    }

    size_t copiedSize = 0;
    for (auto & matrix : matrices)
    {
        std::copy(vector.data() + copiedSize, vector.data() + copiedSize + matrix.size(), matrix.data());
        copiedSize += matrix.size();
    }

    return true;
}


void FFNN::DeleteActivations()
{
    for (auto activation : m_activations)
    {
        delete activation;
    }
    m_activations.clear();
}


Eigen::MatrixXd Sigmoid::Calculate(Eigen::MatrixXd & mat)
{
    return mat.unaryExpr([&](double x) { return 1.0 / (1.0 + std::exp(-x)); });
}


Eigen::MatrixXd Tanh::Calculate(Eigen::MatrixXd & mat)
{
    return mat.unaryExpr([&](double x) { return (std::exp(x) - std::exp(-x)) / (std::exp(x) + std::exp(-x)); });
}


Eigen::MatrixXd ReLU::Calculate(Eigen::MatrixXd & mat)
{
    return mat.unaryExpr([&](double x) { return std::max<double>(x, 0); });
}


Eigen::MatrixXd LeakyReLU::Calculate(Eigen::MatrixXd & mat)
{
    return mat.unaryExpr([&](double x) { return x > 0 ? x : x * 0.001; });
}


Eigen::MatrixXd Softmax::Calculate(Eigen::MatrixXd & mat)
{
    double sumExp = 0;
    mat = mat.unaryExpr([&](double x) { sumExp += std::exp(x); return x; });
    return mat.unaryExpr([&](double x) { return std::exp(x) / sumExp; });
}


// ActivationFactory Implementations

ActivationBase* ActivationFactory::Create(ActivationType type)
{
    ActivationBase* activation = nullptr;

    switch (type)
    {
        case ActivationType::kActivationTypeSigmoid:    activation = new Sigmoid();      break;
        case ActivationType::kActivationTypeTanh:       activation = new Tanh();         break;
        case ActivationType::kActivationTypeReLU:       activation = new ReLU();         break;
        case ActivationType::kActivationTypeLeakyReLU:  activation = new LeakyReLU();    break;
        case ActivationType::kActivationTypeSoftmax:    activation = new Softmax();      break;
        case ActivationType::kActivationTypeInvalid:
        default:
            throw std::runtime_error("Unknown activation type encountered when creating an activation object.");
            break;
    }

    return activation;
}
