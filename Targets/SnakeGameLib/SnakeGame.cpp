//
//  Copyright © 2023-Present, Arkin Terli. All rights reserved.
//
//  NOTICE:  All information contained herein is, and remains the property of Arkin Terli.
//  The intellectual and technical concepts contained herein are proprietary to Arkin Terli
//  and may be covered by U.S. and Foreign Patents, patents in process, and are protected by
//  trade secret or copyright law. Dissemination of this information or reproduction of this
//  material is strictly forbidden unless prior written permission is obtained from Arkin Terli.

#include "SnakeGame.hpp"

#include <random>


void SnakeGame::Update()
{
    if (m_gameState != SnakeGameState::kSnakeGameStateRunning)
    {
        return;
    }

    auto newHeadPos  = m_snake.front();

    switch (m_direction)
    {
        case SnakeDirection::kSnakeDirUp:
            newHeadPos.y--;
            break;
        case SnakeDirection::kSnakeDirDown:
            newHeadPos.y++;
            break;
        case SnakeDirection::kSnakeDirLeft:
            newHeadPos.x--;
            break;
        case SnakeDirection::kSnakeDirRight:
            newHeadPos.x++;
            break;
        default:
            break;
    }

    auto boardObj = m_board[newHeadPos.y][newHeadPos.x];

    // Check if the snake is in the boundary conditions
    if (newHeadPos.x < 0 || newHeadPos.x >= m_boardWidth ||
        newHeadPos.y < 0 || newHeadPos.y >= m_boardHeight ||
        boardObj == BoardObjType::kBoardObjSnakeHead ||
        boardObj == BoardObjType::kBoardObjSnakeBody)
    {
        m_gameState = SnakeGameState::kSnakeGameStateFailed;
        return;
    }

    // Move the snake.
    m_snake.emplace_front(newHeadPos);

    // If the snake got an apple, place a new apple onto the game board.
    if (newHeadPos.x == m_applePos.x && newHeadPos.y == m_applePos.y)
    {
        m_score++;
        if (!PlaceApple())
        {
            m_gameState = SnakeGameState::kSnakeGameStateWon;
            return;
        }
    }
    else
    {
        // Do not remove the tail since the snake got an apple.
        m_snake.pop_back();
    }

    ClearBoard();
    RenderSnake();
    RenderApple();
}


void SnakeGame::Reset()
{
    m_score = 0;
    m_direction = SnakeDirection::kSnakeDirUp;

    Position  snakeHead;
    snakeHead.x = static_cast<int>(GetRandomNumber(float(m_boardWidth)/2, 3*float(m_boardWidth)/4-1));
    snakeHead.y = static_cast<int>(GetRandomNumber(float(m_boardWidth)/2, 3*float(m_boardHeight)/4-1));

    // Add snake head.
    m_snake.emplace_back(snakeHead);

    snakeHead.y++;

    // Add snake body.
    m_snake.emplace_back(snakeHead);

    ClearBoard();
    RenderSnake();
    PlaceApple();
    RenderApple();

    m_gameState = SnakeGameState::kSnakeGameStateRunning;
}


double SnakeGame::GetRandomNumber(double start, double end)
{
    // Create a random device
    std::random_device  rd;

    // Initialize a Mersenne Twister pseudo-random number generator with the random device's seed
    std::mt19937  gen(rd());

    // Define a uniform_real_distribution for the range [0, 1)
    std::uniform_real_distribution<>  dis(start, end);

    // Generate and return a random number in the specified range
    return dis(gen);
}


void SnakeGame::ClearBoard()
{
    // Reset board.
    for (int y = 0; y < m_boardHeight; ++y)
    {
        for (int x = 0; x < m_boardWidth; ++x)
        {
            m_board[y][x] = BoardObjType::kBoardObjEmpty;
        }
    }
}


void SnakeGame::RenderSnake()
{
    bool headRendered = false;

    // Render Snake
    for (const auto & bodyPos : m_snake)
    {
        if (!headRendered)
        {
            m_board[bodyPos.y][bodyPos.x] = BoardObjType::kBoardObjSnakeHead;
            headRendered = true;
        }
        else
        {
            m_board[bodyPos.y][bodyPos.x] = BoardObjType::kBoardObjSnakeBody;
        }
    }
}


void SnakeGame::RenderApple()
{
    // Render Apple
    m_board[m_applePos.y][m_applePos.x] = BoardObjType::kBoardObjApple;
}


bool SnakeGame::PlaceApple()
{
    std::vector<Position>  emptySpots;  // Holds empty spots on the game board.

    for (int y=0; y<m_boardHeight; ++y)
    {
        for (int x=0; x<m_boardWidth; ++x)
        {
            if (m_board[y][x] == BoardObjType::kBoardObjEmpty)
            {
                emptySpots.emplace_back(x,y);
            }
        }
    }

    // If there is no spot left then return false
    if (emptySpots.empty())
    {
        return false;
    }

    auto newSpotIndex = static_cast<int>(GetRandomNumber(0, double(emptySpots.size())-1));
    m_applePos = emptySpots[newSpotIndex];

    return true;
}
