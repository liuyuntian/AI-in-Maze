//Yuntian Liu
//c00151315
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <glut.h>
#include <assert.h>
#include <algorithm>
#include "maze.h"

#define WINDOW_WIDTH_DEFAULT    800
#define WINDOW_HEIGHT_DEFAULT   500

struct WindowStatus
{
    int width;  //current window width
    int height;  //current window height
    char *title; //window title
}g_windowStatus = 
{
    WINDOW_WIDTH_DEFAULT,
    WINDOW_HEIGHT_DEFAULT,
    "Game"
};

CMaze *pMaze = NULL;
int mazeLevel = 10;
bool bPlayerMove = true;

void InitGL()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClearDepth(1.0f);
    glDisable(GL_DEPTH_TEST);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void UpdateMaze()
{
    if (pMaze->IsPlayerOut())
    {
        //player exit the maze, add maze level
        mazeLevel += 5;
        if (mazeLevel > 200)
            mazeLevel = 200;

        pMaze->Resize(mazeLevel, mazeLevel);

        bPlayerMove = true;
    }
}

void DrawText(const char *text, int length, int x, int y ) 

{
    int windowWidth, windowHeight;
    windowWidth = glutGet( GLUT_WINDOW_WIDTH );
    windowHeight = glutGet( GLUT_WINDOW_HEIGHT );

    glMatrixMode (GL_PROJECTION);
    double *matrix = new double [16];
    glGetDoublev(GL_PROJECTION_MATRIX, matrix);
    glLoadIdentity();
    glOrtho(0, windowWidth, 0, windowHeight, -5, 5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2i(x,y);
    for ( int i =0; i < length ;i++ ){
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int) text [i]);
    }
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(matrix);
    glMatrixMode(GL_MODELVIEW);

    delete[] matrix;
}

void DrawInformation()
{
    char strBuf[128];
    int i = 10;
    int x, y;

    glColor3f(0.0f, 1.0f, 1.0f);


    memset(strBuf, 0, 128);
    sprintf(strBuf, "Level : %d", (mazeLevel - 10) / 5 + 1);
    DrawText(strBuf, strlen(strBuf), g_windowStatus.width - 250, g_windowStatus.height - 80);

    memset(strBuf, 0, 128);
    sprintf(strBuf, "Current : %s", bPlayerMove ? "Player" : "Computer");
    DrawText(strBuf, strlen(strBuf), g_windowStatus.width - 250, g_windowStatus.height - 100);

    x = g_windowStatus.width - 250;
    y = g_windowStatus.height - 300;
    memset(strBuf, 0, 128);
    sprintf(strBuf, "F1 : player/computer");
    DrawText(strBuf, strlen(strBuf), x, y - 0);

    memset(strBuf, 0, 128);
    sprintf(strBuf, "A/LEFT: player move left");
    DrawText(strBuf, strlen(strBuf), x, y - 20);

    memset(strBuf, 0, 128);
    sprintf(strBuf, "D/RIGHT : player move right");
    DrawText(strBuf, strlen(strBuf), x, y - 40);

    memset(strBuf, 0, 128);
    sprintf(strBuf, "W/UP : player move up");
    DrawText(strBuf, strlen(strBuf), x, y - 60);

    memset(strBuf, 0, 128);
    sprintf(strBuf, "S/DOWN : player move down");
    DrawText(strBuf, strlen(strBuf), x, y - 80);
}

void Display()
{
    if (pMaze)
    {
        UpdateMaze();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(20.0f, 20.0f, -1.0f); //2D quad, z coordinate of all vertices is 0, so do this modelview transform.

    int drawSize = __min(g_windowStatus.width, g_windowStatus.height);

    drawSize -= 50;

    pMaze->Draw(drawSize, drawSize);

    DrawInformation();

    glFinish();
    glutSwapBuffers();
}

void Resize(int width, int height)
{
    g_windowStatus.width = width;
    g_windowStatus.height = height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //because this is a 2D game, so we use ortho projection
    glOrtho(0.0, (double)width, 0, (double)height, 0.1, 1000.0);
}

/* 
** Timer function, it will be called by GLUT each 100 milliseconds 
** Some functions of game will use timer, such as time counter.
*/
void Clock(int value)
{
    if (pMaze && pMaze->IsAutoMode())
    {
        pMaze->NextStepAuto();
    }
    glutPostRedisplay();
    glutTimerFunc(500, Clock, 0); //this is needed from GLUT spec.
}

/*
** keyboard function will called when user press any key
** we just handle escape key now.
*/
void Keyboard( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27: //escape ASCII,
        {
            exit(1);
        }break;
    case 'w':
    case GLUT_KEY_UP:
        if (pMaze)
            pMaze->MovePlayer(UP);
        break;
    case 's': 
        if (pMaze)
            pMaze->MovePlayer(BOTTOM);
        break;
    case 'a':
        if (pMaze)
            pMaze->MovePlayer(LEFT);
        break;
    case 'd':
        if (pMaze)
            pMaze->MovePlayer(RIGHT);
        break;
    }
}

/*
** Special keys, such as F1~F12 handle function.
*/
void SpecialInput(int key, int x, int y)
{
    switch(key)
    {
    case GLUT_KEY_F1:
        {
            if (pMaze)
            {
                pMaze->FindWayAuto();
                bPlayerMove = false;
            }
        }break;
    case GLUT_KEY_F2:
        {
        }break;
    case GLUT_KEY_UP:
        if (pMaze)
            pMaze->MovePlayer(UP);
        break;
    case GLUT_KEY_DOWN: 
        if (pMaze)
            pMaze->MovePlayer(BOTTOM);
        break;
    case GLUT_KEY_LEFT:
        if (pMaze)
            pMaze->MovePlayer(LEFT);
        break;
    case GLUT_KEY_RIGHT:
        if (pMaze)
            pMaze->MovePlayer(RIGHT);
        break;

    default:
        break;
    }
}

/*
** Mouse function will be cause will player press, release or move mouse
*/
void Mouse(int button, int state, int x, int y)
{
    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        {
            if (state == GLUT_DOWN)
            {
            }
        }
        break;
    default:
        break;
    }
}


void Idle()
{
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    srand(time(NULL));
    pMaze = new CMaze(mazeLevel, mazeLevel);

    glutInit(&argc, argv);

    //setup double buffering
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    //set up the window
    glutInitWindowSize(g_windowStatus.width, g_windowStatus.height);
    glutInitWindowPosition(0, 0);
    glutCreateWindow (g_windowStatus.title);

    //assign the callback functions
    glutDisplayFunc(Display);
    glutSpecialFunc(SpecialInput); //this is not use now
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutReshapeFunc(Resize);
    glutIdleFunc(Idle);
    glutTimerFunc(500, Clock, 0);

    InitGL();

    glutMainLoop();

    delete pMaze;

    return 0;
}