//
//  Copyright © 2023-Present, Arkin Terli. All rights reserved.
//
//  NOTICE:  All information contained herein is, and remains the property of Arkin Terli.
//  The intellectual and technical concepts contained herein are proprietary to Arkin Terli
//  and may be covered by U.S. and Foreign Patents, patents in process, and are protected by
//  trade secret or copyright law. Dissemination of this information or reproduction of this
//  material is strictly forbidden unless prior written permission is obtained from Arkin Terli.

// Project includes
#include "SnakeGame.hpp"
// External includes
// System includes
#include <random>


void SnakeGame::Update()
{
    if (m_gameState != SnakeGameState::kSnakeGameStateRunning)
    {
        return;
    }

    m_steps++;    // Number of iterations until snake eats an apple.

    // If snake can't get the apple in 100 iterations than kill the game. Longer the same becomes
    // more chance to survive.
    if (m_steps > static_cast<size_t>(m_boardWidth * m_boardHeight))
    {
        m_gameState = SnakeGameState::kSnakeGameStateFailedLongLoop;
        return;
    }

    auto newHeadPos  = m_snake.front();

    switch (m_direction)
    {
        case SnakeDirection::kSnakeDirUp:       newHeadPos.y--;     break;
        case SnakeDirection::kSnakeDirDown:     newHeadPos.y++;     break;
        case SnakeDirection::kSnakeDirLeft:     newHeadPos.x--;     break;
        case SnakeDirection::kSnakeDirRight:    newHeadPos.x++;     break;
        default:                                                    break;
    }

    // Check if the snake is in the boundary conditions
    if (newHeadPos.x < 0 || newHeadPos.x >= m_boardWidth ||
        newHeadPos.y < 0 || newHeadPos.y >= m_boardHeight)
    {
        m_gameState = SnakeGameState::kSnakeGameStateFailedHitWall;
        return;
    }

    auto boardObj = m_board[newHeadPos.y][newHeadPos.x];

    // Check if the snake touches its own body.
    if (boardObj == BoardObjType::kBoardObjSnakeHead ||
        boardObj == BoardObjType::kBoardObjSnakeBody)
    {
        m_gameState = SnakeGameState::kSnakeGameStateFailedHitItself;
        return;
    }

    // Move the snake.
    m_snake.emplace_front(newHeadPos);

    // If the snake got an apple, place a new apple onto the game board.
    if (newHeadPos.x == m_applePos.x && newHeadPos.y == m_applePos.y)
    {
        m_score++;
        m_steps = 0;

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
    m_steps = 0;
    m_score = 0;
    m_snake.clear();
    m_gameState = SnakeGameState::kSnakeGameStateRunning;
    m_direction = SnakeDirection::kSnakeDirUp;

    Position  snakeHead;
    snakeHead.x = GetRandomNumber(2, m_boardWidth-2);
    snakeHead.y = GetRandomNumber(2, m_boardHeight-2);

    // Add snake head.
    m_snake.emplace_back(snakeHead);

    // Add snake body.
    snakeHead.y++;
    m_snake.emplace_back(snakeHead);

    ClearBoard();
    RenderSnake();
    PlaceApple();
    RenderApple();
}


std::vector<float> SnakeGame::GetParameters() const
{
    const Position snakeHeadPos = m_snake.front();

    auto IsPositionSafe = [&](const int x, const int y)
    {
        return x >= 0 && y >= 0 && x < m_boardWidth && y < m_boardHeight &&
               (m_board[y][x] == BoardObjType::kBoardObjEmpty || m_board[y][x] == BoardObjType::kBoardObjApple);
    };

    const int x = snakeHeadPos.x;
    const int y = snakeHeadPos.y;

    // Are surrounding blocks safe to move? (4 parameters)
    const float isN = IsPositionSafe(x, y-1) ? 1 : 0;
    const float isS = IsPositionSafe(x, y+1) ? 1 : 0;
    const float isW = IsPositionSafe(x-1, y) ? 1 : 0;
    const float isE = IsPositionSafe(x+1, y) ? 1 : 0;

    // Distance from snake's head to boarder of the game boards. (4 parameters)
    const float dN = y;
    const float dS = m_boardHeight - 1 - y;
    const float dW = x;
    const float dE = m_boardWidth - 1 - x;

    // Direction to apple from snake's head. (4 parameters)
    const float aN = m_applePos.y < y ? 1 : 0;
    const float aS = m_applePos.y > y ? 1 : 0;
    const float aW = m_applePos.x < x ? 1 : 0;
    const float aE = m_applePos.x > x ? 1 : 0;

    // Snake's current moving direction.  (4 parameters)
    float snakesDirUp    = 0;
    float snakesDirDown  = 0;
    float snakesDirLeft  = 0;
    float snakesDirRight = 0;

    switch (m_direction)
    {
        case SnakeDirection::kSnakeDirUp:    snakesDirUp    = 1;  break;
        case SnakeDirection::kSnakeDirDown:  snakesDirDown  = 1;  break;
        case SnakeDirection::kSnakeDirLeft:  snakesDirLeft  = 1;  break;
        case SnakeDirection::kSnakeDirRight: snakesDirRight = 1;  break;
        default: break;
    }

    const auto bW = static_cast<float>(m_boardWidth);
    const auto bH = static_cast<float>(m_boardHeight);

    // Add normalized parameters.
    std::vector<float> params =
    {
        isN, isS, isW, isE,                     // Surrounding blocks safety checks.
        dN / bH, dS / bH, dW / bW, dE / bW,     // Normalized snakes' distances to walls.
        aN, aS, aW, aE,                         // Apple's direction relative to snakes' head.
        snakesDirUp, snakesDirDown, snakesDirLeft, snakesDirRight, // Snakes direction (1 dir is active at a time)
    };

    if (params.size() != m_parameterSize)
    {
        throw std::runtime_error("Parameter size does not match!");
    }

    return params;
}


int SnakeGame::GetRandomNumber(int min, int max)
{
    return std::uniform_int_distribution<int>(min, max)(m_rndEng);
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

    const auto newSpotIndex = GetRandomNumber(0, static_cast<int>(emptySpots.size() - 1));
    m_applePos = emptySpots[newSpotIndex];

    return true;
}


float SnakeGame::GetDistance(const Position & pos, const int xDir, const int yDir, const bool useSnakeBody) const
{
    auto intersectionPos = pos;
    float distance = 0;    // Measured in blocks.

    // Find the intersection point on a boarder of the game board.
    while (intersectionPos.x + xDir >= 0 && intersectionPos.y + yDir >= 0 &&
           intersectionPos.x + xDir < m_boardWidth && intersectionPos.y + yDir < m_boardHeight &&
           (!useSnakeBody ||
            (m_board[intersectionPos.y + yDir][intersectionPos.x + xDir] != BoardObjType::kBoardObjSnakeHead &&
             m_board[intersectionPos.y + yDir][intersectionPos.x + xDir] != BoardObjType::kBoardObjSnakeBody)))
    {
        intersectionPos.x += xDir;
        intersectionPos.y += yDir;
        distance += 1;
    }

    return distance;
}


float SnakeGame::GetDistanceToApple() const
{
    const auto pos = m_snake.front();

    const float dx = m_applePos.x - pos.x;
    const float dy = m_applePos.y - pos.y;

    return std::sqrt(dx*dx + dy*dy);
}
