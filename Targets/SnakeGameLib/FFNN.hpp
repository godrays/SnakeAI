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


// Activation Type
enum class ActivationType : int64_t
{
    kActivationTypeInvalid   = 0,
    kActivationTypeSigmoid   = 1,
    kActivationTypeTanh      = 2,
    kActivationTypeReLU      = 3,
    kActivationTypeLeakyReLU = 4,
    kActivationTypeSoftmax   = 5,
};


// Activation Base Class
class ActivationBase
{
public:
    // Destructor
    virtual ~ActivationBase() = default;

    virtual Eigen::MatrixXd Calculate(Eigen::MatrixXd & mat) = 0;

    // Returns activation type
    ActivationType GetType() { return m_type; }

protected:
    ActivationType  m_type{ActivationType::kActivationTypeInvalid};
};


// Simple multi-layer fully-connected feed-forward neural network.
class FFNN
{
public:
    // Constructor
    FFNN() = default;

    // Constructor
    explicit FFNN(const std::vector<int> & layers, const std::vector<ActivationType> & activations);

    // Destructor
    virtual ~FFNN();

    // Initializes layers.
    void Init(const std::vector<int> & layers, const std::vector<ActivationType> & activations);

    // Makes prediction by using input data.
    Eigen::MatrixXd Forward(const Eigen::MatrixXd & input);

    // Returns all weights as a single vector.
    std::vector<double> SerializeWeights();

    // Sets all weights from a vector.
    bool DeserializeWeights(const std::vector<double> & weightsVector);

    // Returns all biases as a single vector.
    std::vector<double> SerializeBiases();

    // Sets all biases from a vector.
    bool DeserializeBiases(const std::vector<double> & biasesVector);

    // Returns all parameters, weights + biases, as a single vector.
    std::vector<double> SerializeAllParameters();

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

    // Deletes all activations.
    void DeleteActivations();

private:
    std::vector<Eigen::MatrixXd>  m_weights;
    std::vector<Eigen::MatrixXd>  m_biases;
    std::vector<ActivationBase*>  m_activations;
    std::mt19937                  m_rndEngine;
};


// Activation methods.

class Sigmoid : public ActivationBase
{
public:
    Sigmoid() { m_type = ActivationType::kActivationTypeSigmoid; }
    Eigen::MatrixXd Calculate(Eigen::MatrixXd & mat) final;
};


class Tanh : public ActivationBase
{
public:
    Tanh() { m_type = ActivationType::kActivationTypeTanh; }
    Eigen::MatrixXd Calculate(Eigen::MatrixXd & mat) final;
};


class ReLU : public ActivationBase
{
public:
    ReLU() { m_type = ActivationType::kActivationTypeReLU; }
    Eigen::MatrixXd Calculate(Eigen::MatrixXd & mat) final;
};


class LeakyReLU : public ActivationBase
{
public:
    LeakyReLU() { m_type = ActivationType::kActivationTypeLeakyReLU; }
    Eigen::MatrixXd Calculate(Eigen::MatrixXd & mat) final;
};


class Softmax : public ActivationBase
{
public:
    Softmax() { m_type = ActivationType::kActivationTypeSoftmax; }
    Eigen::MatrixXd Calculate(Eigen::MatrixXd & mat) final;
};


// Activation Factory

class ActivationFactory
{
public:
    // Creates and returns a new activation object.
    static ActivationBase* Create(ActivationType type);

private:
    // Constructor to prevent creating a dynamic object.
    ActivationFactory() = default;
};
