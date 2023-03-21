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
// System includes
#include <list>
#include <random>
#include <vector>


enum class SnakeDirection : int32_t
{
    kSnakeDirUp     = 0,
    kSnakeDirDown   = 1,
    kSnakeDirLeft   = 2,
    kSnakeDirRight  = 3,
};

enum class BoardObjType : int32_t
{
    kBoardObjEmpty      = 0,
    kBoardObjSnakeHead  = 1,
    kBoardObjSnakeBody  = 2,
    kBoardObjApple      = 3,
};

enum class SnakeGameState : int32_t
{
    kSnakeGameStateInvalid    = 0,
    kSnakeGameStateRunning    = 1,
    kSnakeGameStateFailed     = 2,
    kSnakeGameStateWon        = 3,
};

struct Position
{
    Position() : x{0}, y{0} { }
    Position(int xP, int yP) : x{xP}, y{yP} { }

    int x;
    int y;
};


class SnakeGame
{
public:
    // Constructor
    explicit SnakeGame(int boardWidth, int boardHeight, int seed) :
            m_boardWidth{boardWidth},
            m_boardHeight{boardHeight},
            m_direction{SnakeDirection::kSnakeDirUp},
            m_gameState{SnakeGameState::kSnakeGameStateInvalid},
            m_score{0},
            m_steps{0},
            m_rndEng(seed)
    {
        // Initialize board 2D game board.
        m_board.resize(m_boardHeight, std::vector<BoardObjType>(m_boardWidth, BoardObjType::kBoardObjEmpty));

        Reset();
    }

    // Returns 2D Game board.
    BoardObjType GetBoardObject(int x, int y)
    {
        return m_board[y][x];
    }

    // Set direction of snake
    void SetDirection(const SnakeDirection & newDir)
    {
        // Ignore direction change if the change is opposite side of the direction.
        if (m_direction == SnakeDirection::kSnakeDirUp    && newDir == SnakeDirection::kSnakeDirDown)  return;
        if (m_direction == SnakeDirection::kSnakeDirDown  && newDir == SnakeDirection::kSnakeDirUp)    return;
        if (m_direction == SnakeDirection::kSnakeDirLeft  && newDir == SnakeDirection::kSnakeDirRight) return;
        if (m_direction == SnakeDirection::kSnakeDirRight && newDir == SnakeDirection::kSnakeDirLeft)  return;

        m_direction = newDir;
    }

    // Returns direction of snake
    SnakeDirection GetDirection()
    {
        return m_direction;
    }

    // Returns game score.
    int GetScore() const
    {
        return m_score;
    }

    SnakeGameState GetGameState()
    {
        return m_gameState;
    }

    // Move snake and check environment.
    void Update();

    // Resets game into initial state.
    void Reset();

    // Returns parameter size that can be used in AI model training.
    static std::size_t GetParameterSize()
    {
        return m_parameterSize;
    }

    // Returns parameters that can be used in AI model training.
    std::vector<double> GetParameters();

    // Returns distance from snake heads to apple.
    double GetDistanceToApple();

    // Return number of steps  snake took without eating an apple.
    std::size_t GetSteps() const
    {
        return m_steps;
    }

private:
    // Return a random number between 0 and 1.
    int64_t GetRandomNumber(int64_t min, int64_t max);

    // Clears the board.
    void ClearBoard();

    // Render snake onto the 2D game board.
    void RenderSnake();

    // Render apple onto the 2D game board.
    void RenderApple();

    // Returns true if a spot found and for an Apple on the board.
    bool PlaceApple();

    // Returns distance in block for cross directions.
    double GetDistance(const Position & pos, int xDir, int yDir, bool useSnakeBody);

private:
    int  m_boardWidth;
    int  m_boardHeight;

    std::vector<std::vector<BoardObjType>>   m_board;
    std::list<Position>  m_snake;
    SnakeDirection  m_direction;
    SnakeGameState  m_gameState;
    Position  m_applePos;
    int m_score;
    std::size_t  m_steps;
    std::mt19937_64   m_rndEng;
    static const std::size_t  m_parameterSize{26};
};
