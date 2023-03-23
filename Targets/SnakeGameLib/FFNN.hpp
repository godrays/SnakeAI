//
//  Copyright © 2023-Present, Arkin Terli. All rights reserved.
//
//  NOTICE:  All information contained herein is, and remains the property of Arkin Terli.
//  The intellectual and technical concepts contained herein are proprietary to Arkin Terli
//  and may be covered by U.S. and Foreign Patents, patents in process, and are protected by
//  trade secret or copyright law. Dissemination of this information or reproduction of this
//  material is strictly forbidden unless prior written permission is obtained from Arkin Terli.

#pragma once

// Project includes
// External includes
#include <Eigen/Dense>
// System includes
#include <random>


// Simple multi-layer feed-forward neural network.
class FFNN
{
public:
    // Constructors
    FFNN() = default;

    explicit FFNN(const std::vector<int> & layers);

    // Initializes layers.
    bool Init(const std::vector<int> & layers);

    // Makes prediction by using input data.
    Eigen::MatrixXd Forward(const Eigen::MatrixXd & input);

    // Returns all weights as a single vector.
    std::vector<double>  SerializeWeights();

    // Sets all weights from a vector.
    bool DeserializeWeights(const std::vector<double> & weightsVector);

    // Returns all biases as a single vector.
    std::vector<double>  SerializeBiases();

    // Sets all biases from a vector.
    bool DeserializeBiases(const std::vector<double> & biasesVector);

    // Returns all parameters, weights + biases, as a single vector.
    std::vector<double>  SerializeAllParameters();

    // Sets all parameters, weights + biases, from a vector.
    bool DeserializeAllParameters(const std::vector<double> & vector);

    // Save the network into a file.
    bool Save(const std::string & filename);

    // Loads a network from a file.
    bool Load(const std::string & filename);

    // Prints all interval variables and states.
    void PrintAll();

private:
    // Serialize all matrices into a single vector.
    std::vector<double> SerializeMatrices(const std::vector<Eigen::MatrixXd> & matrices);

    // Deserialize a vector into source matrices.
    bool DeserializeMatrices(const std::vector<double> & vector, std::vector<Eigen::MatrixXd> & matrices);

private:
    std::vector<Eigen::MatrixXd>  m_weights;
    std::vector<Eigen::MatrixXd>  m_biases;
};
