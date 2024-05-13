# SnakeAI (C++)

SnakeAI is a machine learning project that employs genetic algorithms to train an AI model for the classic Snake game, offering an unconventional approach to the problem, designed by `Arkin Terli`.

### Overview

At the core of SnakeAI lies a fully connected neural network whose parameters are evolved through a genetic algorithm. This approach presents an effective solution for training small-scale neural networks with fewer than 2,000 parameters, demonstrating the versatility and potential of evolutionary algorithms in the realm of machine learning.

The AI agent undergoes an evolutionary process, where its ability to navigate the snake and maximize the game score is continuously optimized. This is achieved by evaluating the fitness of each generation's individuals and selectively applying mutation and crossover operators to the neural network's weights and biases.

The examples below are from the four pre-trained models that solves the problem effectively.

<a href="https://gifyu.com/image/Sa1tL"><img src="https://s12.gifyu.com/images/Sa1tL.gif" alt="Untitled" border="0" /></a>

### Genetic Algorithm

The following genetic algorithm parameters worked well for the Snake game problem, and they are set by default.

    population size    = 50
    parent ratio       = 50%
    mutate probability = 1%
    transfer ratio     = 15%
    crossover          = 50%

A genetic algorithm requires a fitness score to evaluate how well an individual in a population is performing in a generation to make evolution decisions. The following fitness function is used to score individuals in this application:

```C++
    fitness score = highestScore * 500 + avgScore * 50 - avgDeaths * 15 - avgSteps * 10 - avgLongLoopFails * 100
```

Average values from many random starting games is used to calculate the fitness score. A single game result alone is not sufficient to properly evaluate how well a model is performing, which is why the average across 2000 games is used instead. 

The highest score refers to the maximum score achieved by a particular model across those 2000 games.

Finding a good fitness function formula is an art.

### Neural Network Model

A three-layer fully-connected neural network with the hidden layer sizes (16, 16, 8) is utilized to train the Snake AI agent.

The model employs 16 input features:
- Surrounding block safety checks to determine if movement is safe (4 parameters)
- Normalized snake distances to walls (4 parameters)  
- Apple's direction relative to the snake's head (4 parameters)
- Snake's direction, with only one active at a time (4 parameters)

and 4 model outputs representing the snake's direction:
- Up
- Down
- Left
- Right

The highest value is chosen as the direction.

---

# How to Train and Play SnakeAI

Follow the following steps to train and play model.

### Step 1: Train

```bash
cd product-rel
./SnakeAIApp ga train --modelfile=snakeai.mdl --maxGen=500
```

The example command above sets the maximum generation count to 500, which is fairly enough. A well-trained model usually reaches ~52,000 fitness score for the default game board dimensions. When you set the board dimensions higher, you can expect to reach a higher fitness score during the training.

Run `SnakeAIApp ga train` to see all command line options.

### Step 2: Play

```bash
./SnakeAIApp ga play --modelfile=snakeai.mdl
```

---

# Project Build Instructions

Follow these steps to build the project and make it deployment-ready.

Currently, it supports only macOS and has been tested on Sonoma.

### Step 1: Build Externals

This step will build external libraries.

```bash
cd Externals
./build_all.sh
```

### Step 2: Build Targets

This step will build all binaries and deploy into a specific folder.

```bash
./build.sh release product-rel
```

After a successful build, all target binaries will be deployed into the product-rel folder.

Note: Run the build.sh file without parameters to see all options.

---

# License

Copyright © 2023 - Present, Arkin Terli. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

3. Neither the name of Arkin Terli nor the names of its contributors may be
   used to endorse or promote products derived from this software without
   specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
