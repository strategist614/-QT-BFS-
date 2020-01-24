#ifndef OPEARTION_H
#define OPEARTION_H
#include "QDialog"
#include "QVector"
#include "vector"
#include "QDebug"
#include "QQueue"
#include "QFile"

#include "iostream"

class Operation
{
public:
    Operation();
    ~Operation();

    //图
    int map[30][30];

    int playerx,playery;
    int ex,ey;
    int mapH,mapW;
    QQueue<char> v;
    int readMapH,readMapW;

public:
    void AutoCreateMap(int w,int h);
    void CreatePath();

    void SetStartPosition(int w,int h);
    void SetEndPosition(int w,int h);
    void ChangePlayerPosition(char dir);
    bool FeasiblePosition(char dir);
    void SetMapSize(int w,int h);
    bool IsGameVictory();
    void Reset();
    void AutoSetStartPosition(int w,int h);
    bool IsFeasibleMap(QString path);
    void ReadMap();

private:
    QFile *mapFile;
    QTextStream mapTextStream;


};

#endif // OPEARTION_H
