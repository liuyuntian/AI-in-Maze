//Yuntian Liu
//c00151315
#ifndef MAZE_H
#define MAZE_H

#define UP 0
#define RIGHT 1
#define BOTTOM 2
#define LEFT 3
#define DIRECTIONS 4

#include <iostream>
#include <vector>
#include "stack.h"

class CCell
{
public:
    CCell()
    {
        m_visitFlags = 0;
        m_arriveFlags = 0;
        m_wall[UP] = m_wall[RIGHT] = m_wall[BOTTOM] = m_wall[LEFT] = true;
        m_x = -1;
        m_y = -1;
    }
    void DestroyWall(int dir)
    {
        m_wall[dir] = false;
    }
    bool IsVisited()
    {
        return m_visitFlags == 1 ? true: false;
    }
    bool IsArrived()
    {
        return m_arriveFlags == 1 ? true : false;
    }
    void MarkVisited()
    {
        m_visitFlags = 1;
    }
    void MarkArrived()
    {
        m_arriveFlags = 1;
    }
    void ResetArriveFlag()
    {
        m_arriveFlags = 0;
    }
    void SetPosition(int x, int y)
    {
        m_x = x;
        m_y = y;
    }
    void GetPosition(int &x, int &y)
    {
        x = m_x;
        y = m_y;
    }
    bool IsHaveWall(int dir)
    {
        if (dir < DIRECTIONS)
            return m_wall[dir];
        else
            return true;
    }
    void Draw(float width, float height);
protected:
    int m_visitFlags; //use when create maze
    int m_arriveFlags; //use when search
    bool m_wall[DIRECTIONS];
    int m_x;
    int m_y;
};

class CMaze
{
public:
    CMaze(int width, int height);
    ~CMaze();
    void Resize(int width, int height);
    CCell* GetCell(int x, int y);
    void Draw(int width, int height);
    void MovePlayer(int dir);
    void ResetPlayer() { m_playerPosX = m_playerPosY = 0;}
    bool IsPlayerOut();

    /*
    ** this function tell the computer find the way
    */
    void FindWayAuto();

    /*
    ** In auto mode, player will move one step after call this function until to the
    ** maze exit
    */
    void NextStepAuto();

    bool IsAutoMode()
    {
        return m_bAutoMode;
    }
protected:
    void CreateDFS(); //use depth first search to create maze
    void FindWayDFS(); //use depth first search to find a way to the exit
    CCell* GetMovableNeighborRandom(int myX, int myY);
    CCell* GetUnvisitedNeighborRandom(int myX, int myY);
    void ConnectCells(CCell *c1, CCell *c2);
protected:
    int m_width;
    int m_height;
    CCell *m_cellArray;
    int m_playerPosX;
    int m_playerPosY;
    bool m_bAutoMode;
    int m_steps;
    int m_curStep;
    unsigned long *m_pSteps;
};

#endif
