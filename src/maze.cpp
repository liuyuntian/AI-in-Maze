//Yuntian Liu
//c00151315
#include "maze.h"
#include "stack.h"
#include <algorithm>
#include <assert.h>
#include <glut.h>

CMaze::CMaze(int width, int height)
{
    m_width = width; 
    m_height = height;

    m_cellArray = new CCell[m_height * m_height];

    for (int h = 0; h < m_height; h++)
    {
        for (int w = 0; w < m_width; w++)
        {
            CCell *pCell = GetCell(w, h);
            pCell->SetPosition(w, h);
        }
    }

    CreateDFS();

    m_playerPosX = m_playerPosY = 0;

    m_bAutoMode = false;
    m_steps = 0;
    m_pSteps = NULL;
}

CMaze::~CMaze()
{
    delete[] m_cellArray;

    if (m_pSteps)
    {
        delete[] m_pSteps;
        m_pSteps = NULL;
    }
}

void CMaze::Resize(int width, int height)
{
    delete[] m_cellArray;

    m_width = width;
    m_height = height;

    m_cellArray = new CCell[m_height * m_height];

    for (int h = 0; h < m_height; h++)
    {
        for (int w = 0; w < m_width; w++)
        {
            CCell *pCell = GetCell(w, h);
            pCell->SetPosition(w, h);
        }
    }

    CreateDFS();

    m_playerPosX = m_playerPosY = 0;
    m_bAutoMode = false;
}

CCell* CMaze::GetCell(int x, int y)
{
    if (x >= 0 && y >= 0 && x < m_width && y < m_height)
    {
        int idx = y * m_width + x;

        return &m_cellArray[idx];
    }
    else
    {
        return NULL;
    }
}

CCell* CMaze::GetUnvisitedNeighborRandom(int myX, int myY)
{
    int dir;
    CCell *pCell = NULL;
    int flag = 0;

    while (pCell == NULL)
    {
        dir = rand() % DIRECTIONS;

        if (dir == UP)
        {
            pCell = GetCell(myX, myY + 1);
        }
        else if (dir == RIGHT)
        {
            pCell = GetCell(myX + 1, myY);
        }
        else if (dir == BOTTOM)
        {
            pCell = GetCell(myX, myY - 1);
        }
        else if (dir == LEFT)
        {
            pCell = GetCell(myX - 1, myY);
        }
        else
        {
            assert(0);
        }

        if (pCell && pCell->IsVisited())
            pCell = NULL;

        flag |= 1 << dir;

        if (flag == 0xF && pCell == NULL) //all neighbor was visited, return NULL
            return NULL;
    }

    return pCell;
}

CCell* CMaze::GetMovableNeighborRandom(int myX, int myY)
{
    int dir;
    CCell *pCell = GetCell(myX, myY);
    CCell *pNei = NULL;
    int flag = 0;

    while (pNei == NULL)
    {
        dir = rand() % DIRECTIONS;

        flag |= 1 << dir;

        if (pCell->IsHaveWall(dir))
            continue;

        if (dir == UP)
        {
            pNei = GetCell(myX, myY + 1);
        }
        else if (dir == RIGHT)
        {
            pNei = GetCell(myX + 1, myY);
        }
        else if (dir == BOTTOM)
        {
            pNei = GetCell(myX, myY - 1);
        }
        else if (dir == LEFT)
        {
            pNei = GetCell(myX - 1, myY);
        }
        else
        {
            assert(0);
        }

        if (pNei && pNei->IsArrived())
            pNei = NULL;

        if (flag == 0xF && pNei == NULL)
            return NULL;
    }

    return pNei;
}

void CMaze::ConnectCells(CCell *c1, CCell *c2)
{
    int x1, y1;
    int x2, y2;

    c1->GetPosition(x1, y1);
    c2->GetPosition(x2, y2);

    int dirX = x2 - x1;
    int dirY = y2 - y1;

    if (dirX != 0 && dirY == 0)
    {
        if (dirX == 1)
        {
            //c2 at the right of c1
            c1->DestroyWall(RIGHT);
            c2->DestroyWall(LEFT);
        }
        else if (dirX == -1)
        {
            //c2 at the left of c1
            c1->DestroyWall(LEFT);
            c2->DestroyWall(RIGHT);
        }
        else 
        {
            assert(0);
        }
    }
    else if (dirX == 0 && dirY != 0)
    {
        if (dirY == 1)
        {
            //c2 at the top of c1
            c1->DestroyWall(UP);
            c2->DestroyWall(BOTTOM);
        }
        else if (dirY == -1)
        {
            //c2 at the bottom of c1
            c1->DestroyWall(BOTTOM);
            c2->DestroyWall(UP);
        }
        else
        {
            assert(0);
        }
    }
    else
    {
        assert(0);
    }
}

void CMaze::CreateDFS()
{
    CCell *pCurrent = NULL;
    CCell *pStart;
    CStack stack;
    CCell *pNeighbor = NULL;
    int x, y;

    //always use (0, 0) as maze entry
    pStart = GetCell(0, 0);
    pStart->MarkVisited();
    pCurrent = pStart;
    pCurrent->DestroyWall(LEFT);
    pNeighbor = GetUnvisitedNeighborRandom(0, 0);

    do
    {
        if (pNeighbor)
        {
            ConnectCells(pCurrent, pNeighbor);
            pNeighbor->MarkVisited();

            stack.Push(pCurrent);

            pCurrent = pNeighbor;
            pCurrent->GetPosition(x, y);
            pNeighbor = GetUnvisitedNeighborRandom(x,y);
        }
        else if (!stack.IsEmpty())
        {
            pCurrent = (CCell *)stack.Pop();
            pCurrent->GetPosition(x, y);
            pNeighbor = GetUnvisitedNeighborRandom(x,y);
        }
    }while(pCurrent != pStart);

    //this it end
    GetCell(m_width - 1, m_height - 1)->DestroyWall(UP);
}

void CMaze::FindWayDFS()
{
    CCell *pCurrent = NULL;
    CCell *pEnd = GetCell(m_width - 1, m_height - 1);
    CCell *pNeighbor = NULL;
    CStack stack;
    int x, y;

    //always use (0, 0) as maze entry
    pCurrent = GetCell(m_playerPosX, m_playerPosY);
    pCurrent->MarkArrived();
    pNeighbor = GetMovableNeighborRandom(m_playerPosX, m_playerPosY);

    do
    {
        if (pNeighbor)
        {
            pNeighbor->MarkArrived();

            stack.Push(pCurrent);

            pCurrent = pNeighbor;
            pCurrent->GetPosition(x, y);
            pNeighbor = GetMovableNeighborRandom(x,y);
        }
        else if (!stack.IsEmpty())
        {
            pCurrent = (CCell *)stack.Pop();
            pCurrent->GetPosition(x, y);
            pNeighbor = GetMovableNeighborRandom(x,y);
        }
    }while(pCurrent != pEnd);

    stack.Push(pCurrent);

    m_steps = stack.GetSize();
    m_pSteps = new unsigned long[m_steps];
    for (int i = m_steps - 1; i >= 0; i--)
    {
        pCurrent = (CCell *)stack.Pop();
        pCurrent->GetPosition(x, y);
        m_pSteps[i] = ((y & 0xFFFF) << 16 ) | (x & 0xFFFF);
    }
}

void CMaze::Draw(int width, int height)
{
    float cellWidth = (float)width / m_width;
    float cellHeight = (float)height / m_height;
    float radiu = min(cellWidth, cellHeight) / 2.0f - 0.5f;

    for (int h = 0; h < m_height; h++)
    {
        for (int w = 0; w <m_width; w++)
        {
            glColor3f(1.0f, 1.0f, 1.0f);
            glPushMatrix();
                glTranslatef(w * cellWidth, h * cellHeight, 0.0);
                GetCell(w, h)->Draw(cellWidth, cellHeight);
            glPopMatrix();
        }
    }

    //draw player
    glColor3f(0.0f, 1.0f, 0.0f);
    glPushMatrix();
        glTranslatef(m_playerPosX * cellWidth + cellWidth / 2.0f, m_playerPosY * cellHeight + cellHeight / 2.0f, 0.0f);
        glutSolidSphere(radiu, 10, 10);
    glPopMatrix();
}

void CCell::Draw(float width, float height)
{  
    glBegin(GL_LINES);
    if (m_wall[UP])
    {
        glVertex2f(0.0f, height);
        glVertex2f(width, height);
    }

    if (m_wall[RIGHT])
    {
        glVertex2f(width, height);
        glVertex2f(width, 0.0f);
    }

    if (m_wall[BOTTOM])
    {
        glVertex2f(0.0f, 0.0f);
        glVertex2f(width, 0.0f);
    }

    if (m_wall[LEFT])
    {
        glVertex2f(0.0f, 0.0f);
        glVertex2f(0.0f, height);
    }
    glEnd();
}

void CMaze::MovePlayer(int dir)
{
    if (m_bAutoMode)
        return;

    CCell *pCell = GetCell(m_playerPosX, m_playerPosY);

    //is the mode direction have wall, can not move
    if (pCell->IsHaveWall(dir))
        return;

    if (dir == UP)
    {
        if (m_playerPosX == m_width - 1 && 
            m_playerPosY == m_height - 1)// end position, can not move up, or will leave maze
            return;
        else
            m_playerPosY += 1;
    }
    else if (dir == RIGHT)
    {
        m_playerPosX += 1;
    }
    else if (dir == BOTTOM)
    {
        m_playerPosY -= 1;
    }
    else if (dir == LEFT)
    {
        if (m_playerPosX == 0 && m_playerPosY == 0)// start position, can not move left, or will leave maze
            return;
        else
            m_playerPosX -= 1;
    }
}

bool CMaze::IsPlayerOut()
{
    if (m_playerPosX == m_width - 1 && 
        m_playerPosY == m_height - 1)// end position, can not move up, or will leave maze
        return true;
    else
        return false;
}

void CMaze::FindWayAuto()
{
    m_bAutoMode = true;
    m_steps = 0;

    if (m_pSteps)
    {
        delete[] m_pSteps;
        m_pSteps = NULL;
    }

    for (int h = 0; h < m_height; h++)
    {
        for (int w = 0; w <m_width; w++)
        {
            GetCell(w, h)->ResetArriveFlag();
        }
    }

    FindWayDFS();
    m_curStep = 0;
}

void CMaze::NextStepAuto()
{
    int x, y;
    unsigned long data = m_pSteps[m_curStep];

    x = data & 0xFFFF;
    y = (data >> 16 ) & 0xFFFF;

    m_playerPosX = x;
    m_playerPosY = y;

    if (m_playerPosX >= m_width || m_playerPosY >= m_height)
        assert(0);

    m_curStep ++;
    if (m_curStep >= m_steps)
        m_curStep = m_steps - 1;
}
