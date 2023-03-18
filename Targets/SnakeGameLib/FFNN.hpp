//
//  Copyright © 2023-Present, Arkin Terli. All rights reserved.
//
//  NOTICE:  All information contained herein is, and remains the property of Arkin Terli.
//  The intellectual and technical concepts contained herein are proprietary to Arkin Terli
//  and may be covered by U.S. and Foreign Patents, patents in process, and are protected by
//  trade secret or copyright law. Dissemination of this information or reproduction of this
//  material is strictly forbidden unless prior written permission is obtained from Arkin Terli.

#pragma once

#include <Eigen/Dense>
#include <random>

// Simple multi-layer feed-forward neural network.
class FFNN
{
public:
    // Initializes layers.
    bool Init(const std::vector<int> & layers);

    // Makes prediction by using input data.
    Eigen::MatrixXd Forward(const Eigen::MatrixXd & input);

    // Returns all weights as a single vector.
    std::vector<double>  GetAllWeightsAsVector();

    // Sets all weights from a vector.
    bool SetAllWeightsFromVector(const std::vector<double> & weightsVector);

    // Save the network into a file.
    bool Save(const std::string & filename);

    // Loads a network from a file.
    bool Load(const std::string & filename);

    // Prints all interval variables and states.
    void PrintAll();

private:
    std::vector<Eigen::MatrixXd>  m_weights;
    std::vector<Eigen::MatrixXd>  m_biases;
};
