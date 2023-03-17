//
//  Copyright © 2023-Present, Arkin Terli. All rights reserved.
//
//  NOTICE:  All information contained herein is, and remains the property of Arkin Terli.
//  The intellectual and technical concepts contained herein are proprietary to Arkin Terli
//  and may be covered by U.S. and Foreign Patents, patents in process, and are protected by
//  trade secret or copyright law. Dissemination of this information or reproduction of this
//  material is strictly forbidden unless prior written permission is obtained from Arkin Terli.

#pragma once

#include <vector>
#include <list>


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
    explicit SnakeGame(int boardWidth, int boardHeight) :
        m_boardWidth{boardWidth},
        m_boardHeight{boardHeight},
        m_direction{SnakeDirection::kSnakeDirUp},
        m_gameState{SnakeGameState::kSnakeGameStateInvalid},
        m_score{0}
    {
        // Initialize board 2D game board.
        m_board.resize(boardHeight, std::vector<BoardObjType>(boardWidth, BoardObjType::kBoardObjEmpty));

        Reset();
    }

    // Returns 2D Game board.
    std::vector<std::vector<BoardObjType>> GetBoard()
    {
        return m_board;
    }

    // Set direction of snake
    void SetDirection(const SnakeDirection & dir)
    {
        m_direction = dir;
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

private:
    // Return a random number between 0 and 1.
    static double GetRandomNumber(double start, double end);

    // Clears the board.
    void ClearBoard();

    // Render snake onto the 2D game board.
    void RenderSnake();

    // Render apple onto the 2D game board.
    void RenderApple();

    // Returns true if a spot found and for an Apple on the board.
    bool PlaceApple();

private:
    int  m_boardWidth;
    int  m_boardHeight;

    std::vector<std::vector<BoardObjType>>   m_board;
    std::list<Position>  m_snake;
    SnakeDirection  m_direction;
    SnakeGameState  m_gameState;

    Position  m_applePos;

    int m_score;
};
