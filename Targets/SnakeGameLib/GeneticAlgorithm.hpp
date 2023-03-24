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
#include <ThreadPool.hpp>
// External includes
// System includes
#include <future>
#include <vector>


namespace ga
{

template<typename T>
class Individual
{
public:
    Individual() : m_fitness(0)
    {
    }

    explicit Individual(std::vector<T> value) : m_value{std::move(value)}, m_fitness(0)
    {
    }

    std::vector<T> GetValue() const
    {
        return m_value;
    }

    double GetFitness() const
    {
        return m_fitness;
    }

    void SetFitness(double fitness)
    {
        m_fitness = fitness;
    }

    auto operator[](const std::size_t i) const
    {
        return m_value[i];
    }

private:
    std::vector<T>  m_value;
    double          m_fitness;
};


template<typename T>
class Population
{
public:
    Population(const std::size_t maxPopulation, const std::size_t parentRatio,
               const std::size_t mutateProbability, const std::size_t transferRatio,
               const std::size_t crossover, const std::size_t geneticMaterialLength) :
        m_maxPopulation{maxPopulation},
        m_parentRatio{parentRatio},
        m_mutateProbability{mutateProbability},
        m_geneticMaterialLength{geneticMaterialLength}
    {
        m_transferCount = (transferRatio * maxPopulation) / 100;
        m_crossoverThreshold = (crossover * maxPopulation) / 100;
        m_newIndividualsPerGeneration = maxPopulation - m_transferCount;

        std::random_device  rndDev;
        m_rndEngine.seed(rndDev());
    }

    void CreateInitialGeneration()
    {
        // Create thread pool to calculate fitness functions.
        ThreadPool  tp(std::thread::hardware_concurrency());
        m_population.resize(m_maxPopulation);

        std::vector<std::future<void>>  results;
        for (std::size_t i=0; i<m_maxPopulation; ++i)
        {
            auto futureRet = tp.Enqueue([&](std::size_t i)
            {
                // Generate random generic material value.
                std::vector<T>  value(m_geneticMaterialLength, 0);
                std::generate_n(value.begin(), m_geneticMaterialLength, m_randomItemFunc);

                Individual<T> newChild{value};
                m_population[i] = newChild;
            }, i);

            results.emplace_back(std::move(futureRet));
        }

        // Wait until all threads are finished.
        for (std::size_t i=0; i<m_maxPopulation; ++i)
        {
            results[i].wait();
            results[i].get();     // We don't have a return value but this will rethrow an uncaught exception.
        }

        CalculatePopulationFitnessValues();

        SortIndividuals();
    }

    void CreateNextGeneration()
    {
        std::vector<Individual<T>>  nextGeneration;
        nextGeneration.resize(m_maxPopulation);

        // Create thread pool to calculate fitness functions.
        ThreadPool  tp(std::thread::hardware_concurrency());

        std::vector<std::future<void>>  results;
        for (std::size_t i=0; i<m_maxPopulation; ++i)
        {
            auto futureRet = tp.Enqueue([&](std::size_t i)
            {
                if (i < m_transferCount)
                {
                    nextGeneration[i] = m_population[i];
                }
                else
                {
                    Individual<T> & mother = m_population[GetRandomNumber(0, m_crossoverThreshold)];
                    Individual<T> & father = m_population[GetRandomNumber(0, m_crossoverThreshold)];
                    nextGeneration[i] = CreateChild(mother, father, m_parentRatio, m_mutateProbability);
                }
            }, i);

            results.emplace_back(std::move(futureRet));
        }

        // Wait until all threads are finished.
        for (std::size_t i=0; i<m_maxPopulation; ++i)
        {
            results[i].wait();
            results[i].get();     // We don't have a return value but this will rethrow an uncaught exception.
        }

        m_population = nextGeneration;

        CalculatePopulationFitnessValues();

        SortIndividuals();
    }

    void SetFitnessFunc(std::function<std::size_t(const std::vector<T> & value)>&& func)
    {
        m_fitnessFunc = std::move(func);
    }

    void SetRandomItemFunc(std::function<T()> && func)
    {
        m_randomItemFunc = std::move(func);
    }

    // Returns the best individual of the current population.
    const Individual<T> & GetBestIndividual() const
    {
        return m_population.front();
    }

private:
    Individual<T> CreateChild(const Individual<T> & mother, const Individual<T> & father, const std::size_t parentRatio,
                              const std::size_t mutateProbability)
    {
        std::vector<T>  childValue(m_geneticMaterialLength, 0);

        for (size_t i=0; i < m_geneticMaterialLength; ++i)
        {
            if (GetRandomNumber(0, 100) < mutateProbability)
            {
                childValue[i] = m_randomItemFunc();
            }
            else if (GetRandomNumber(0, 100) < parentRatio)
            {
                childValue[i] = mother[i];
            }
            else
            {
                childValue[i] = father[i];
            }
        }

        return Individual<T>{childValue};
    }

    // Calculates population fitness values in parallel.
    void CalculatePopulationFitnessValues()
    {
        // Create thread pool to calculate fitness functions.
        ThreadPool  tp(std::thread::hardware_concurrency());

        // Calculate fitness values in parallel.
        std::vector<std::future<void>>  results;
        for (std::size_t i=0; i<m_maxPopulation; ++i)
        {
            auto futureRet = tp.Enqueue([&](std::size_t i)
            {
                auto & individual = m_population[i];
                individual.SetFitness(m_fitnessFunc(individual.GetValue()));
            }, i);

            results.emplace_back(std::move(futureRet));
        }

        // Wait until all threads are finished.
        for (std::size_t i=0; i<m_maxPopulation; ++i)
        {
            results[i].wait();
            results[i].get();     // We don't have a return value but this will rethrow an uncaught exception.
        }
    }

    std::size_t GetRandomNumber(std::size_t min, std::size_t max)
    {
        return std::uniform_int_distribution<std::size_t>(min, max)(m_rndEngine);
    }

    // Sorts all individuals in current population based on their fitness values. The highest value is the best.
    void SortIndividuals()
    {
        std::sort(m_population.begin(), m_population.end(), [](const Individual<T> & left, const Individual<T> & right)
        {
            return left.GetFitness() > right.GetFitness();
        });
    }

private:
    std::vector<Individual<T>>  m_population;
    std::size_t  m_maxPopulation;
    std::size_t  m_parentRatio;
    std::size_t  m_mutateProbability;
    std::size_t  m_transferCount;
    std::size_t  m_crossoverThreshold;
    std::size_t  m_newIndividualsPerGeneration;
    std::size_t  m_geneticMaterialLength;

    std::function<double(const std::vector<T> & value)>   m_fitnessFunc;
    std::function<T()>   m_randomItemFunc;
    std::mt19937         m_rndEngine;
};


template<typename T>
class GeneticAlgorithm
{
public:
    GeneticAlgorithm(const std::size_t maxPopulation, const std::size_t parentRatio,
                     const std::size_t mutateProbability, const std::size_t transferRatio,
                     const std::size_t crossover, const std::size_t geneticMaterialLength) :
            m_population{maxPopulation, parentRatio, mutateProbability, transferRatio, crossover, geneticMaterialLength},
            m_generation{0}
    {
    }

    void SetFitnessFunc(std::function<double(const std::vector<T> & value)>&& func)
    {
        m_population.SetFitnessFunc(std::move(func));
    }

    void SetRandomItemFunc(std::function<T()>&& func)
    {
        m_population.SetRandomItemFunc(std::move(func));
    }

    void CreateInitialPopulation()
    {
        m_generation = 1;
        m_population.CreateInitialGeneration();
    }

    void CreateNextPopulation()
    {
        m_generation++;
        m_population.CreateNextGeneration();
    }

    const Individual<T> & GetBestIndividual() const
    {
        return m_population.GetBestIndividual();
    }

    std::size_t GetGeneration() const
    {
        return m_generation;
    }

private:
    Population<T>   m_population;
    std::size_t     m_generation;
};

} // namespace ga
