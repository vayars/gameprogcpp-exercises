// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Board.h"
#include "Random.h"
#include <SDL/SDL_Log.h>

BoardState::BoardState()
{
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			mBoard[i][j] = Empty;
		}
	}
}

std::vector<BoardState*> BoardState::GetPossibleMoves(SquareState player) const
{
	std::vector<BoardState*> retVal;

	// For each column, find if a move is possible
	for (int col = 0; col < 7; col++)
	{
		for (int row = 5; row >= 0; row--)
		{
			if (mBoard[row][col] == BoardState::Empty)
			{
				retVal.emplace_back(new BoardState(*this));
				retVal.back()->mBoard[row][col] = player;
				break;
			}
		}
	}

	return retVal;
}

bool BoardState::IsTerminal() const
{
	// Is the board full?
	if (IsFull())
	{
		return true;
	}

	// Is there a four-in-a-row?
	int fourInRow = GetFourInARow();
	if (fourInRow != 0)
	{
		return true;
	}

	return false;
}

float BoardState::GetScore() const
{
	// If the board is full, the score is 0
	if (IsFull())
	{
		return 0.0f;
	}

	// Is there a four-in-a-row?
	int fourInRow = GetFourInARow();
	if (fourInRow != 0)
	{
		return static_cast<float>(fourInRow);
	}

	return CalculateHeuristic();
}

bool BoardState::IsFull() const
{
	bool isFull = true;
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			if (mBoard[i][j] == Empty)
			{
				isFull = false;
			}
		}
	}

	return isFull;
}

int BoardState::GetFourInARow() const
{
	// Returns -1 if yellow wins, 1 if red wins, 0 otherwise

	// Check if there's a row with four in a row
	for (int row = 0; row < 6; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			if (mBoard[row][col] == mBoard[row][col + 1] &&
				mBoard[row][col] == mBoard[row][col + 2] &&
				mBoard[row][col] == mBoard[row][col + 3])
			{
				if (mBoard[row][col] == BoardState::Yellow)
				{
					return -1;
				}
				else if (mBoard[row][col] == BoardState::Red)
				{
					return 1;
				}
			}
		}
	}

	// Check if there's a column with four in a row
	for (int col = 0; col < 7; col++)
	{
		for (int row = 0; row < 3; row++)
		{
			if (mBoard[row][col] == mBoard[row + 1][col] &&
				mBoard[row][col] == mBoard[row + 2][col] &&
				mBoard[row][col] == mBoard[row + 3][col])
			{
				if (mBoard[row][col] == BoardState::Yellow)
				{
					return -1;
				}
				else if (mBoard[row][col] == BoardState::Red)
				{
					return 1;
				}
			}
		}
	}

	// Check if there's a right diagonal four in a row
	for (int col = 0; col < 4; col++)
	{
		for (int row = 0; row < 3; row++)
		{
			if (mBoard[row][col] == mBoard[row + 1][col + 1] &&
				mBoard[row][col] == mBoard[row + 2][col + 2] &&
				mBoard[row][col] == mBoard[row + 3][col + 3])
			{
				if (mBoard[row][col] == BoardState::Yellow)
				{
					return -1;
				}
				else if (mBoard[row][col] == BoardState::Red)
				{
					return 1;
				}
			}
		}
	}

	// Check if there's a left diagonal for in a row
	for (int col = 0; col < 4; col++)
	{
		for (int row = 3; row < 6; row++)
		{
			if (mBoard[row][col] == mBoard[row - 1][col + 1] &&
				mBoard[row][col] == mBoard[row - 2][col + 2] &&
				mBoard[row][col] == mBoard[row - 3][col + 3])
			{
				if (mBoard[row][col] == BoardState::Yellow)
				{
					return -1;
				}
				else if (mBoard[row][col] == BoardState::Red)
				{
					return 1;
				}
			}
		}
	}
	return 0;
}

float BoardState::CalculateHeuristic() const
{
	// Check the maximum number of disks the AI and the player has in a row
    float maxRed = 0.0f;
    float maxYellow = 0.0f;
    
    // Check if there's a row with four in a row
    for (int row = 0; row < 6; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (mBoard[row][col] == BoardState::Yellow)
            {
                //SDL_Log("Checking Yellow");
                // If we spot 4 in a row for the first time,
                if (mBoard[row][col] == mBoard[row][col + 1] &&
                    mBoard[row][col] == mBoard[row][col + 2] &&
                    mBoard[row][col] == mBoard[row][col + 3] &&
                    maxYellow < 4.0f)
                {
                    maxYellow = 4.0f;
                }
                // Else if we spot 3 in a row for the first time,
                else if (mBoard[row][col] == mBoard[row][col + 1] &&
                         mBoard[row][col] == mBoard[row][col + 2] &&
                         maxYellow < 3.0f)
                {
                    maxYellow = 3.0f;
                }
                // Else if we spot 2 in a row for the first time,
                else if (mBoard[row][col] == mBoard[row][col + 1] &&
                    maxYellow < 2.0f)
                {
                    maxYellow = 2.0f;
                }
                else if (maxYellow < 1) {
                    maxYellow = 1.0f;
                }
            }
            
            if (mBoard[row][col] == BoardState::Red)
            {
                //SDL_Log("Checking Red");
                // If we spot 4 in a row for the first time,
                if (mBoard[row][col] == mBoard[row][col + 1] &&
                    mBoard[row][col] == mBoard[row][col + 2] &&
                    mBoard[row][col] == mBoard[row][col + 3] &&
                    maxRed < 4.0f)
                {
                    maxRed = 4.0f;
                }
                // Else if we spot 3 in a row for the first time,
                else if (mBoard[row][col] == mBoard[row][col + 1] &&
                         mBoard[row][col] == mBoard[row][col + 2] &&
                         maxRed < 3.0f)
                {
                    maxRed = 3.0f;
                }
                // Else if we spot 2 in a row for the first time,
                else if (mBoard[row][col] == mBoard[row][col + 1] &&
                    maxRed < 2.0f)
                {
                    maxRed = 2.0f;
                }
                else if (maxRed < 1.0f) {
                    maxRed = 1.0f;
                }
            }
        }
    }
        
    // Check if there's a column with four in a row
    for (int col = 0; col < 7; col++)
    {
        for (int row = 0; row < 3; row++)
        {
            if (mBoard[row][col] == BoardState::Yellow)
            {
                // 4 in a row
                if (mBoard[row][col] == mBoard[row + 1][col] &&
                         mBoard[row][col] == mBoard[row + 2][col] &&
                         mBoard[row][col] == mBoard[row + 3][col] &&
                         maxYellow < 4.0f)
                {
                    maxYellow = 4.0f;
                }
                // 3 in a row
                else if (mBoard[row][col] == mBoard[row + 1][col] &&
                         mBoard[row][col] == mBoard[row + 2][col] &&
                         maxYellow < 3.0f)
                {
                    maxYellow = 3.0f;
                }
                // 2 in a row
                else if (mBoard[row][col] == mBoard[row + 1][col] &&
                         maxYellow < 2.0f)
                {
                    maxYellow = 2.0f;
                }
                // Don't need to check for one since the rows would've caught it
            }
            else if (mBoard[row][col] == BoardState::Red)
            {
                // 4 in a row
                if (mBoard[row][col] == mBoard[row + 1][col] &&
                         mBoard[row][col] == mBoard[row + 2][col] &&
                         mBoard[row][col] == mBoard[row + 3][col] &&
                         maxRed < 4.0f)
                {
                    maxRed = 4.0f;
                }
                // 3 in a row
                else if (mBoard[row][col] == mBoard[row + 1][col] &&
                         mBoard[row][col] == mBoard[row + 2][col] &&
                         maxRed < 3.0f)
                {
                    maxRed = 3.0f;
                }
                // 2 in a row
                else if (mBoard[row][col] == mBoard[row + 1][col] &&
                         maxRed < 2.0f)
                {
                    maxRed = 2.0f;
                }
                // Don't need to check for one since the rows would've caught it
            }
        }
    }

    // Check if there's a right diagonal four in a row
    for (int col = 0; col < 4; col++)
    {
        for (int row = 0; row < 3; row++)
        {
            if (mBoard[row][col] == BoardState::Yellow)
            {
                // 4 in a row
                if (mBoard[row][col] == mBoard[row + 1][col + 1] &&
                    mBoard[row][col] == mBoard[row + 2][col + 2] &&
                    mBoard[row][col] == mBoard[row + 3][col + 3] &&
                    maxYellow < 4.0f)
                {
                    maxYellow = 4.0f;
                }
                // 3 in a row
                else if (mBoard[row][col] == mBoard[row + 1][col + 1] &&
                         mBoard[row][col] == mBoard[row + 2][col + 2] &&
                         maxYellow < 3.0f)
                {
                    maxYellow = 3.0f;
                }
                // 2 in a row
                else if (mBoard[row][col] == mBoard[row + 1][col + 1] &&
                         maxYellow < 2.0f)
                {
                    maxYellow = 2.0f;
                }
            }
            else if (mBoard[row][col] == BoardState::Red)
            {
                // 4 in a row
                if (mBoard[row][col] == mBoard[row + 1][col + 1] &&
                    mBoard[row][col] == mBoard[row + 2][col + 2] &&
                    mBoard[row][col] == mBoard[row + 3][col + 3] &&
                    maxRed < 4.0f)
                {
                    maxRed = 4.0f;
                }
                // 3 in a row
                else if (mBoard[row][col] == mBoard[row + 1][col + 1] &&
                         mBoard[row][col] == mBoard[row + 2][col + 2] &&
                         maxRed < 3.0f)
                {
                    maxRed = 3.0f;
                }
                // 2 in a row
                else if (mBoard[row][col] == mBoard[row + 1][col + 1] &&
                         maxRed < 2.0f)
                {
                    maxRed = 2.0f;
                }
            }
        }
    }

    // Check if there's a left diagonal for in a row
    for (int col = 0; col < 4; col++)
    {
        for (int row = 3; row < 6; row++)
        {
            if (mBoard[row][col] == BoardState::Yellow)
            {
                // 4 in a row
                if (mBoard[row][col] == mBoard[row - 1][col + 1] &&
                    mBoard[row][col] == mBoard[row - 2][col + 2] &&
                    mBoard[row][col] == mBoard[row - 3][col + 3] &&
                    maxYellow < 4.0f)
                {
                    maxYellow = 4.0f;
                }
                // 3 in a row
                else if (mBoard[row][col] == mBoard[row - 1][col + 1] &&
                         mBoard[row][col] == mBoard[row - 2][col + 2] &&
                         maxYellow < 3.0f)
                {
                    maxYellow = 3.0f;
                }
                // 2 in a row
                else if (mBoard[row][col] == mBoard[row - 1][col + 1] &&
                         maxYellow < 2.0f)
                {
                    maxYellow = 2.0f;
                }
            }
            else if (mBoard[row][col] == BoardState::Red)
            {
                // 4 in a row
                if (mBoard[row][col] == mBoard[row - 1][col + 1] &&
                    mBoard[row][col] == mBoard[row - 2][col + 2] &&
                    mBoard[row][col] == mBoard[row - 3][col + 3] &&
                    maxRed < 4.0f)
                {
                    maxRed = 4.0f;
                }
                // 3 in a row
                else if (mBoard[row][col] == mBoard[row - 1][col + 1] &&
                         mBoard[row][col] == mBoard[row - 2][col + 2] &&
                         maxRed < 3.0f)
                {
                    maxRed = 3.0f;
                }
                // 2 in a row
                else if (mBoard[row][col] == mBoard[row - 1][col + 1] &&
                         maxRed < 2.0f)
                {
                    maxRed = 2.0f;
                }
            }
        }
    }
    
    // Calculate heuristic
    float score;
    if (maxRed == 4.0f)
    {
        score = 1;
    }
    else
    {
        score = (maxRed - maxYellow) / 3;
    }
    SDL_Log("Heuristic: %f", score);
    return score;
}

bool TryPlayerMove(BoardState* state, int column)
{
	// Find the first row in that column that's available
	// (if any)
	for (int row = 5; row >= 0; row--)
	{
		if (state->mBoard[row][column] == BoardState::Empty)
		{
			state->mBoard[row][column] = BoardState::Yellow;
			return true;
		}
	}

	return false;
}

void CPUMove(BoardState* state, AIComponent aic)
{
    *state = *aic.MinimaxDecide(state, 2);
    
    /*// For now, this just randomly picks one of the possible moves
	std::vector<BoardState*> moves = state->GetPossibleMoves(BoardState::Red);

	int index = Random::GetIntRange(0, moves.size() - 1);

	*state = *moves[index];

	// Clear up memory from possible moves
	for (auto state : moves)
	{
		delete state;
	}*/
}

AIComponent::AIComponent()
{
}

float AIComponent::MaxPlayerLimit(const BoardState* state, int depth)
{
    // If we've reached max dpeth or the board is terminal:
    if (depth == 0 || state->IsTerminal())
    {
        return state->GetScore();
    }
    // Else, find the subtree with the max value
    float maxValue = -std::numeric_limits<float>::infinity();
    for (const BoardState* child : state->GetPossibleMoves(state->Red))
    {
        maxValue = std::max(maxValue, MinPlayerLimit(child, depth-1));
    }
    return maxValue;
}

float AIComponent::MinPlayerLimit(const BoardState* state, int depth)
{
    // If we've reached the max depth or the board is terminal:
    if (depth == 0 || state->IsTerminal())
    {
        return state->GetScore();
    }
    // Else, find the subtree with the min value
    float minValue = std::numeric_limits<float>::infinity();
    for (const BoardState* child : state->GetPossibleMoves(state->Yellow))
    {
        minValue = std::min(minValue, MaxPlayerLimit(child, depth-1));
    }
    return minValue;
}

const BoardState* AIComponent::MinimaxDecide(const BoardState* root, int depth)
{
    // Find the subtree with the maximum value and save the choice
    const BoardState* choice = nullptr;
    float maxValue = -std::numeric_limits<float>::infinity();
    for (const BoardState* child : root->GetPossibleMoves(root->Red))
    {
        float v = MinPlayerLimit(child, depth);
        if (v > maxValue)
        {
            maxValue = v;
            choice = child;
        }
    }
    
    return choice;
}

