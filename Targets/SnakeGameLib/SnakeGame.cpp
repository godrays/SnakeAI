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
    if (m_steps > std::size_t(m_boardWidth * m_boardHeight))
    {
        m_gameState = SnakeGameState::kSnakeGameStateFailed;
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

    // Check if the snake is in the boundary conditions
    if (newHeadPos.x < 0 || newHeadPos.x >= m_boardWidth ||
        newHeadPos.y < 0 || newHeadPos.y >= m_boardHeight)
    {
        m_gameState = SnakeGameState::kSnakeGameStateFailed;
        return;
    }

    auto boardObj = m_board[newHeadPos.y][newHeadPos.x];

    // Check if the snake touches its own body.
    if (boardObj == BoardObjType::kBoardObjSnakeHead ||
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
    snakeHead.x = static_cast<int>(GetRandomNumber(2, m_boardWidth-2));
    snakeHead.y = static_cast<int>(GetRandomNumber(2, m_boardHeight-2));

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


std::vector<double> SnakeGame::GetParameters()
{
    std::vector<double>  params;

    Position snakeHeadPos = m_snake.front();

    // Distance from snake's head to boarder of the game boards. (8 parameters)
    double N = snakeHeadPos.y;
    double S = m_boardHeight - 1 - snakeHeadPos.y;
    double W = snakeHeadPos.x;
    double E = m_boardWidth - 1 - snakeHeadPos.x;
    double NW = GetDistance(snakeHeadPos, -1, -1, false);
    double NE = GetDistance(snakeHeadPos,  1, -1, false);
    double SW = GetDistance(snakeHeadPos, -1,  1, false);
    double SE = GetDistance(snakeHeadPos,  1,  1, false);

    // Distance from snake's head to either snake's body or game board borders. (8 parameters)
    double sN = GetDistance(snakeHeadPos,  0, -1, true);
    double sS = GetDistance(snakeHeadPos,  0,  1, true);
    double sW = GetDistance(snakeHeadPos, -1,  0, true);
    double sE = GetDistance(snakeHeadPos,  1,  0, true);
    double sNW = GetDistance(snakeHeadPos, -1, -1, true);
    double sNE = GetDistance(snakeHeadPos,  1, -1, true);
    double sSW = GetDistance(snakeHeadPos, -1,  1, true);
    double sSE = GetDistance(snakeHeadPos,  1,  1, true);

    // Direction to apple from snake's head. (2 parameters)
    double aN = m_applePos.y < snakeHeadPos.y ? 1 : 0;
    double aS = m_applePos.y > snakeHeadPos.y ? 1 : 0;
    double aW = m_applePos.x < snakeHeadPos.x ? 1 : 0;
    double aE = m_applePos.x > snakeHeadPos.x ? 1 : 0;

    // Snake's current moving direction.  (4 parameters)
    double snakesDirUp    = 0;
    double snakesDirDown  = 0;
    double snakesDirLeft  = 0;
    double snakesDirRight = 0;

    switch (m_direction)
    {
        case SnakeDirection::kSnakeDirUp:    snakesDirUp    = 1;  break;
        case SnakeDirection::kSnakeDirDown:  snakesDirDown  = 1;  break;
        case SnakeDirection::kSnakeDirLeft:  snakesDirLeft  = 1;  break;
        case SnakeDirection::kSnakeDirRight: snakesDirRight = 1;  break;
        default: break;
    }

    // Distance to apple. (1 parameter)
    double appleDist = GetDistanceToApple();

    // Size of snake.  (1 parameter)
    double snakeSize = m_snake.size();

    params = {
        N, S, W, E, NW, NE, SW, SE,
        sN, sS, sW, sE, sNW, sNE, sSW, sSE,
        aN, aS, aW, aE,
        snakesDirUp, snakesDirDown, snakesDirLeft, snakesDirRight,
        appleDist,
        snakeSize,
    };

    if (params.size() != m_parameterSize)
    {
        throw std::runtime_error("Parameter size does not match!");
    }

    return params;
}


int64_t SnakeGame::GetRandomNumber(int64_t min, int64_t max)
{
    // EXTREMELY IMPORTANT: Each game must have it's own random number sequence.
    //std::default_random_engine rndEng(m_seed);
    std::uniform_int_distribution<int>  dist(min, max);
    return dist(m_rndEng);
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

    auto newSpotIndex = GetRandomNumber(0, emptySpots.size()-1);
    m_applePos = emptySpots[newSpotIndex];

    return true;
}


double SnakeGame::GetDistance(const Position & pos, int xDir, int yDir, bool useSnakeBody)
{
    auto intersectionPos = pos;
    double distance = 0;    // Measured in blocks.

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


double SnakeGame::GetDistanceToApple()
{
    Position pos = m_snake.front();

    double dx = m_applePos.x - pos.x;
    double dy = m_applePos.y - pos.y;

    return std::sqrt(dx*dx + dy*dy);
}
