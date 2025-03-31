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
#include <stdexcept>
#include <vector>


enum class SnakeDirection : int32_t
{
    kUp     = 0,
    kDown   = 1,
    kLeft   = 2,
    kRight  = 3,
};

enum class BoardObjType : int32_t
{
    kEmpty      = 0,
    kSnakeHead  = 1,
    kSnakeBody  = 2,
    kApple      = 3,
};

enum class SnakeGameState : int32_t
{
    kInvalid          = 0,
    kRunning          = 1,
    kFailedHitWall    = 2,
    kFailedHitItself  = 3,
    kFailedLongLoop   = 4,
    kWon              = 5,
};

struct Position
{
    Position() : x{0}, y{0} { }
    Position(const int xP, const int yP) : x{xP}, y{yP} { }

    int x;
    int y;
};


class SnakeGame
{
public:
    // Constructor
    explicit SnakeGame(const int boardWidth, const int boardHeight, const int seed) :
            m_boardWidth{boardWidth},
            m_boardHeight{boardHeight},
            m_direction{SnakeDirection::kUp},
            m_gameState{SnakeGameState::kInvalid},
            m_score{0},
            m_steps{0},
            m_rndEng(seed)
    {
        // Initialize board 2D game board.
        m_board.resize(m_boardHeight, std::vector<BoardObjType>(m_boardWidth, BoardObjType::kEmpty));

        Reset();
    }

    // Returns 2D Game board.
    BoardObjType GetBoardObject(const int x, const int y) const
    {
        if (x < 0 || y < 0 || x >= m_boardWidth || y >= m_boardHeight)
        {
            throw std::runtime_error("Out-of-bounds access in GetBoardObject()");
        }
        return m_board[y][x];
    }

    // Set direction of snake
    void SetDirection(const SnakeDirection & newDir)
    {
        // Ignore direction change if the change is opposite side of the direction.
        if (m_direction == SnakeDirection::kUp    && newDir == SnakeDirection::kDown)  return;
        if (m_direction == SnakeDirection::kDown  && newDir == SnakeDirection::kUp)    return;
        if (m_direction == SnakeDirection::kLeft  && newDir == SnakeDirection::kRight) return;
        if (m_direction == SnakeDirection::kRight && newDir == SnakeDirection::kLeft)  return;

        m_direction = newDir;
    }

    // Returns direction of snake
    SnakeDirection GetDirection() const
    {
        return m_direction;
    }

    // Returns game score.
    int GetScore() const
    {
        return m_score;
    }

    SnakeGameState GetGameState() const
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
    std::vector<float> GetParameters() const;

    // Returns distance from snake heads to apple.
    float GetDistanceToApple() const;

    // Return number of steps  snake took without eating an apple.
    std::size_t GetSteps() const
    {
        return m_steps;
    }

private:
    // Return a random number between min and max.
    int GetRandomNumber(int min, int max);

    // Clears the board.
    void ClearBoard();

    // Render snake onto the 2D game board.
    void RenderSnake();

    // Render apple onto the 2D game board.
    void RenderApple();

    // Returns true if a spot found and for an Apple on the board.
    bool PlaceApple();

    // Returns distance in block for cross directions.
    float GetDistance(const Position & pos, int xDir, int yDir, bool useSnakeBody) const;

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
    static constexpr std::size_t  m_parameterSize{16};
};
