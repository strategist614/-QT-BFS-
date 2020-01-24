#include "operation.h"
#include "QKeyEvent"
#include "QVector"
#include "vector"
#include "time.h"
#include "QQueue"
#include "QFile"
#include "QException"
#include "QMessageBox"
#include "QIcon"

Operation::Operation()
{

}

Operation::~Operation()
{
}

//自动建图
void Operation::AutoCreateMap(int w,int h)
{
       int n=w-2,m=h-2;

       const int N = 35;
       QVector <int> block_row;
       QVector <int> block_column;
       QVector <int> block_direct;
       typedef struct point
       {
        int x;
        int y;
       }Point;
       Point start,End;
       int x_num=1;
       int y_num=1;
       int a[N][N];
        start.x=1;//定义起始点
        start.y=1;
        End.x=m;
        End.y=n;
        for(int i=0;i<=m+1;i++){
            for(int j=0;j<=n+1;j++){
                a[i][j]=1;//墙
            }
        }
        srand((unsigned)time(NULL));//随机种子
        if(x_num+1<=m){
            block_row.push_back(x_num+1);
            block_column.push_back(y_num);
            block_direct.push_back(1);
        } //向下
        if(y_num+1<=n){
            block_row.push_back(x_num);
            block_column.push_back(y_num+1);
            block_direct.push_back(2);
        } //向右
        if(x_num-1>=1){
            block_row.push_back(x_num-1);
            block_column.push_back(y_num);
            block_direct.push_back(8);

        } //向上
        if(y_num-1>=1){
            block_row.push_back(x_num);
            block_column.push_back(y_num-1);
            block_direct.push_back(4);
         } //向左
        a[1][1]=2;
        while(block_row.size()){//第一步压入两堵墙（起点右边和起点下面）进入循环
            int num=block_row.size();
            int randnum=rand()%num;//生成0-num-1之间的随机数，同时也是vector里的下标
            x_num=block_row[randnum];
            y_num=block_column[randnum];
            switch(block_direct[randnum]){//选择一个方向进行后续操作，起始点 邻块 目标块 三块区域在同一直线上 随后移动到目标块的位置
                case 1:{
                    x_num=block_row[randnum]+1;
                    y_num=block_column[randnum];
                    break;
                }
                case 2:{
                    x_num=block_row[randnum];
                    y_num=block_column[randnum]+1;
                    break;
                }
                case 4:{
                    x_num=block_row[randnum];
                    y_num=block_column[randnum]-1;
                    break;
                }
                case 8:{
                    x_num=block_row[randnum]-1;
                    y_num=block_column[randnum];
                    break;
                }
            }
            if(a[x_num][y_num]==1){//目标块如果是墙
                a[block_row[randnum]][block_column[randnum]]=2;//打通墙
                a[x_num][y_num]=2;//打通目标块
            if(x_num+1<=m){
            block_row.push_back(x_num+1);
            block_column.push_back(y_num);
            block_direct.push_back(1);
        } //向下
        if(y_num+1<=n){
            block_row.push_back(x_num);
            block_column.push_back(y_num+1);
            block_direct.push_back(2);
        } //向右
        if(x_num-1>=1){
            block_row.push_back(x_num-1);
            block_column.push_back(y_num);
            block_direct.push_back(8);

        } //向上
        if(y_num-1>=1){
            block_row.push_back(x_num);
            block_column.push_back(y_num-1);
            block_direct.push_back(4);
         } //向左//再次计算当前位置周围的邻墙个数并保存进vector
            }
            block_row.erase(block_row.begin()+randnum);//删除这堵墙(把用不了的墙删了，已经删过的墙不用删)
            block_column.erase(block_column.begin()+randnum);
            block_direct.erase(block_direct.begin()+randnum);
        }
        for(int i=0;i<=m+1;i++){
            for(int j=0;j<=n+1;j++){
                if(a[i][j]==2){
                    map[i][j] = 1;
                }
                else{
                    map[i][j] = 0;
                }
            }
        }
        map[m+1][n] = 1;
}

//自动寻路
void Operation::CreatePath()
{
        this->v.clear();

        const int maxn = 30,directsize=4;
        int sx=playerx,sy=playery;//sx,sy 是起点坐标，ex,ey是终点坐标//

        struct dir
        { //存方向
           int x,y;
        }dir[directsize]={{0,1},{0,-1},{1,0},{-1,0}};
        struct node //存放点的坐标
        {
           int x,y;
        };
        int vis[maxn+1][maxn+1]={0};
        node fa[maxn+1][maxn+1],path[maxn*maxn]; //用于输出
        QQueue<node> q;

        memset(vis,0,sizeof(vis));

        node start,front,v;
        start.x=sx;start.y=sx;
        q.enqueue(start);
        vis[start.x][start.y]=1;
        while(!q.empty())
        {
            front=q.front();
            q.dequeue();

            if(front.x==ex&&front.y==ey)//到达终点时
            {
                 int count=0;
                 path[count].x=ex;
                 path[count].y=ey;
                 for(;;)
                 {
                    if(path[count].x==sx&&path[count].y==sy)
                    break;
                    path[count+1]=fa[path[count].x][path[count].y];//递归记录走过的路径
                    count++;
                  }
                for(int i=count;i>=0;i--)
                {

                   if(abs(path[i].x - path[i-1].x) == 1)
                   {
                      if(path[i].x - path[i-1].x == 1)  this->v.enqueue('w');//向上移动
                      else this->v.enqueue('s');//向下移动
                   }
                   if(abs(path[i].y - path[i-1].y) == 1)
                   {
                      if(path[i].y - path[i-1].y == 1) this->v.enqueue('a');//向左移动
                      else this->v.enqueue('d');//向右移动
                   }
                }
                return;
            }

            for(int i=0;i<directsize;i++)
            {
                v.x=front.x+dir[i].x;//向四周尝试走
                v.y=front.y+dir[i].y;

                if(v.x<0||v.x>=this->mapH||v.y<0||v.y>=this->mapW||!(this->map[v.x][v.y])||vis[v.x][v.y])//走过的、是墙、越界均跳过
                    continue;

                q.enqueue(v);//可行的方向入队

                fa[v.x][v.y]=front;//将当前的节点记录下来

            }
            vis[front.x][front.y]=1;//标记当前点已走过
        }
}

//逻辑修改 玩家控制人物位置改变
void Operation::ChangePlayerPosition(char dir)
{
    switch(dir)
    {
        case 'w':
        {
            playerx-=1;
            break;
        }
        case 'a':
        {
            playery-=1;
            break;
        }
        case 's':
        {
            playerx+=1;
            break;
        }
        case 'd':
        {
            playery+=1;
            break;
        }
    default:
    {
        return;
    }
    }
}

//手动设定逻辑起始点-临时使用
void Operation::SetStartPosition(int x,int y)
{
    this->playerx = x;
    this->playery = y;
}

//自动设定逻辑终点
void Operation::SetEndPosition(int w,int h)
{
    this->ex = h-1;
    this->ey = w-2;
}

//判断下一个逻辑位置是否可行
bool Operation::FeasiblePosition(char dir)
{
    int nx = playerx,ny = playery;

    switch(dir)
    {
    case 'w':
    {
        nx -= 1;
        break;
    }
    case 'a':
    {
        ny -= 1;
        break;
    }
    case 's':
    {
        nx += 1;
        break;
    }
    case 'd':
    {
        ny += 1;
        break;
    }
    default:
    {
        return false;
    }
    }

    if(nx<0||ny<0||nx>=mapH||ny>=mapW||map[nx][ny] == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

//判断是否到达终点
bool Operation::IsGameVictory()
{
    if(playerx == ex && playery == ey)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//自动生成起点
void Operation::AutoSetStartPosition(int w,int h)
{
    int x,y;
    x = qrand()%(h/2);
    y = qrand()%(w/2);
    //合法起点
    while(x<0||y<0||x>=h/2||y>=w/2||map[x][y]==0)
    {
        x = qrand()%(h/2);
        y = qrand()%(w/2);
    }
    playerx = x;
    playery = y;
}

//重设逻辑数据
void Operation::Reset()
{
      playerx = playery = 0;
      mapH = mapW = 0;
      ex = ey = 0;
}

//设定逻辑图的宽和高
void Operation::SetMapSize(int w,int h)
{
    this->mapH = h;
    this->mapW = w;
}

//判断从本地读取的地图是否可行
bool Operation::IsFeasibleMap(QString path)
{
    mapFile = new QFile(path);
    mapTextStream.setDevice(mapFile);

    int tempnum;
    int mapSize = 1;
    int oprNum = 0;
    int mapStartPos;
    int w = -1,h = -1;

    //若无法打开文件QFile
    if(!mapFile->open(QIODevice::ReadOnly))
    {
        return false;
    }

    //读取宽和高
    try {
        while(!mapTextStream.atEnd())
        {
            //首先读取两个数字
            mapTextStream>>tempnum;
            mapSize *= tempnum;
            oprNum++;
            if(oprNum == 1) w = tempnum;
            else if(oprNum == 2)
            {
                h = tempnum;
                break;
            }
        }
    } catch (QException) {
        return false;
    }

    //宽高不再合规范围之内
    if(w<5||w>30||h<5||h>30)
    {
        return false;
    }

    //操作数归0
    oprNum = 0;
    //保留开始指针
    mapStartPos = mapTextStream.pos();
    //遍历地图
    while(!mapTextStream.atEnd())
    {
        mapTextStream>>tempnum;

        //超出范围的字符
        if(tempnum!=0&&tempnum!=1)
        {
            return false;
        }

        oprNum++;
    }

    //操作数与地图尺寸不符
    if(mapSize != oprNum)
    {
        return false;
    }

    //重设读取地图指针 用于读取地图
    mapTextStream.seek(mapStartPos);
    //赋值 为读取地图做准备
    readMapH = h;
    readMapW = w;
    return true;
}

//创建从本地读取到的图
void Operation::ReadMap()
{
    int tempNum;

    for(int i = 0;i<readMapH;i++)
    {
        for(int j = 0;j<readMapW;j++)
        {
           mapTextStream>>tempNum;
           map[i][j] = tempNum;
        }
    }

    //终点强行设置为可通路
    map[readMapH-1][readMapW-2] = 1;
    //关闭文件
    mapFile->close();

}
