#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"
#include "string"
#include "QString"
#include "requestwindow.h"
#include "QApplication"
#include "operation.h"
#include "QKeyEvent"
#include "QDebug"
#include "QThread"
#include "QPalette"
#include "QMovie"
#include "QPropertyAnimation"
#include "QFile"
#include "QException"
#include "QFileDialog"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //实例化timer 循环开启键盘事件锁 (未启动)
    this->keyEventtimer = new QTimer(this);
    connect(keyEventtimer,SIGNAL(timeout()),this,SLOT(SetEventStop()));
    this->autoPathtimer = new QTimer(this);
    connect(autoPathtimer,SIGNAL(timeout()),this,SLOT(AutoPath()));

    //初始化信息栏 操作数/宽/高
    ui->labelValueCount->setText(QString::number(this->countOfOperation,10));
    ui->labelValueHigh->setText("0");
    ui->labelValueWidth->setText("0");

    //初始化表格属性
    initializeTableWidget();

    //初始化逻辑数据
    opr.Reset();

    //初始化开始状态
    SetStartInformationLabel(0);

    //初始化玩家 隐藏控制人物
    ui->Player->setVisible(false);

    //初始化绑定玩家动画效果
    this->playerAnimation = new QPropertyAnimation(ui->Player,"geometry");

    //连接输入窗口
    connect(ui->pushButtonCreateMaze,SIGNAL(clicked()),this,SLOT(ChangeLabelHighAndWidth())); ///成立输入窗口
}

MainWindow::~MainWindow()
{
    delete ui;
}

//生成询问窗口 得到宽和高
void MainWindow::ChangeLabelHighAndWidth()
{
    //正在自动寻路 不允许生成新图
    if(this->isStartAutoPath == true)
    {
        QMessageBox *tempMessageBox = new QMessageBox();

        tempMessageBox->setWindowTitle("警告");
        tempMessageBox->setText(("请等待自动寻路完毕"));

        tempMessageBox->show();
        return;
    }

    //判断是否已经结束
    if(this->isStart == true)
    {
        QMessageBox *tempMessageBox = new QMessageBox();

        tempMessageBox->setWindowTitle("警告");
        tempMessageBox->setText(("游戏还未结束，你确定要生成地图?"));
        tempMessageBox->setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);

        tempMessageBox->show();
        if(tempMessageBox->exec()== QMessageBox::Cancel)
        {
            tempMessageBox->~QMessageBox();
            return;
        }
    }
    resDialog = new requestDialog(this);
    resDialog->setModal(true);
    connect(resDialog,SIGNAL(SendValue(int,int)),this,SLOT(ReceiveValue(int,int)));
    resDialog->show();
}

//宽高数据 接受端
void MainWindow::ReceiveValue(int w,int h)
{
    this->mazeWidth = w;
    this->mazeHigh = h;
    opr.SetMapSize(w,h);
    ui->labelValueWidth->setText(QString::number(w,10));
    ui->labelValueHigh->setText(QString::number(h,10));

    //操作有效时 开始新游戏
    if(ui->labelValueHigh->text().toInt() != 0)
    {
        opr.AutoCreateMap(w,h);
        StartNewGame();
    }

    this->setFocus();
}

//获取键盘按键W/A/S/D  控制人物 操作入口**********************
void MainWindow::keyPressEvent(QKeyEvent *e)
{
    //未开始 则直接跳过
    if(this->isStart == false)
    {
        return;
    }

    //若已开始自动寻路 锁定键盘事件
    if(this->isStartAutoPath == true)
    {
        return;
    }

    if(this->keyEventStop == false)
    {
    //设置窗体焦点
    this->setFocus();
    //字符型按键
    char charkey;

    switch(e->key())
    {
    case Qt::Key_W:
    {
        ChangePushBoxText('w',1);
        charkey = 'w';
        break;
    }
    case Qt::Key_A:
    {
        ChangePushBoxText('a',1);
        charkey = 'a';
        break;
    }
    case Qt::Key_S:
    {
        ChangePushBoxText('s',1);
        charkey = 's';
        break;
    }
    case Qt::Key_D:
    {
        ChangePushBoxText('d',1);
        charkey = 'd';
        break;
    }
    default:
        charkey = 'p';
        break;
    }

    //更改人物朝向 当方向未更改时 不必更改人物朝向
    if(lastKey != charkey)
    {
    SetPlayerMovie(charkey);
    }

    //有效移动时 从逻辑和视图中更改玩家位置 并判断是否到达终点
    if(opr.FeasiblePosition(charkey))
    {
        AddCountOfOperation();
        opr.ChangePlayerPosition(charkey);
        UIChangePlayerPosition(charkey);
        //若到达终点
        if(opr.IsGameVictory())
        {
            QMessageBox vir;
            vir.setWindowTitle("胜利");
            vir.setText("你好像到了终点");
            vir.setModal(true);
            vir.show();
            if(vir.exec())
            {
                GameVictory();
            }
        }
    }
    //记录本次key值
    this->lastKey = charkey;
    }
    this->keyEventStop = true;
}

//循环解锁键盘
void MainWindow::SetEventStop()
{
    //当移动动画播放完毕后 解锁键盘事件
    if(playerAnimation->state() == QPropertyAnimation::Stopped)
    {
        this->keyEventStop = false;
    }
    //按键还原
    {
        ChangePushBoxText('w',0);
        ChangePushBoxText('a',0);
        ChangePushBoxText('s',0);
        ChangePushBoxText('d',0);
    }
}

//改变Key提示按键文本 dir 1为触发 0为还原
void MainWindow::ChangePushBoxText(char key,int dir)
{
    //初始化 调色板以及字形
    QPalette paPress,paRelease;
    QFont fPress,fRelease;

    //触发
    paPress.setColor(QPalette::ButtonText,Qt::red);
    fPress.setPointSize(16);
    //返回
    paRelease.setColor(QPalette::ButtonText,Qt::black);
    fRelease.setPointSize(12);

    switch(dir)
    {
    //触发
    case 1:
    {
      switch(key)
      {
          case 'w':
          {
              ui->pushButtonUp->setPalette(paPress);
              ui->pushButtonUp->setFont(fPress);
              break;
          }
          case 's':
          {
              ui->pushButtonDown->setPalette(paPress);
              ui->pushButtonDown->setFont(fPress);
              break;
          }
          case 'a':
          {
              ui->pushButtonLeft->setPalette(paPress);
              ui->pushButtonLeft->setFont(fPress);
              break;
          }
          case 'd':
          {
              ui->pushButtonRight->setPalette(paPress);
              ui->pushButtonRight->setFont(fPress);
              break;
          }
              default:break;
          }
      break;
    }
        //还原
        case 0:
        {
            switch(key)
            {
                case 'w':
                {
                    ui->pushButtonUp->setPalette(paRelease);
                    ui->pushButtonUp->setFont(fRelease);
                    break;
                }
                case 's':
                {
                    ui->pushButtonDown->setPalette(paRelease);
                    ui->pushButtonDown->setFont(fRelease);
                    break;
                }
                case 'a':
                {
                    ui->pushButtonLeft->setPalette(paRelease);
                    ui->pushButtonLeft->setFont(fRelease);
                    break;
                }
                case 'd':
                {
                    ui->pushButtonRight->setPalette(paRelease);
                    ui->pushButtonRight->setFont(fRelease);
                    break;
                }
                default:break;
                }
         break;
        }
    }
}

//将地图数组显示到表格中 生成可视化地图  随机生成地图修改处*********************
void MainWindow::ShowMap(int h,int w,bool isChangeSize)
{
    //非修改尺寸引起的地图可视化则生成随机参数
    if(isChangeSize == false)
    {
        //读取随机数以生成随机地图
        this->mapThemeFactor = qrand() % 5;
        while(mapThemeFactor == 0)
        {
            mapThemeFactor = qrand() % 5;
        }
    }
    QString randChar = QString::number(mapThemeFactor,10);
    QString randWall =":/img/wall" + randChar+ ".png";
    QString randRoad =":/img/road" + randChar+ ".png";

    for(int i=0;i<h;i++)
    {
        for(int j=0;j<w;j++)
        {
            //路障
            if(opr.map[i][j] == 0)
            {
                QLabel *lb = new QLabel("");
                lb->setPixmap(QPixmap(randWall).scaled(this->tableWidgetItemSize,this->tableWidgetItemSize));
                ui->tableWidgetGameForm->setCellWidget(i,j,lb);
            }
            //通路
            else if(opr.map[i][j] == 1)
            {
                QLabel *lb = new QLabel("");
                lb->setPixmap(QPixmap(randRoad).scaled(this->tableWidgetItemSize,this->tableWidgetItemSize));
                ui->tableWidgetGameForm->setCellWidget(i,j,lb);
            }
        }
    }

    //保留静态终点样式
    //QLabel *lb = new QLabel("");
    //lb->setPixmap(QPixmap(":/img/endPoint.png").scaled(this->tableWidgetItemSize,this->tableWidgetItemSize));
    //ui->tableWidgetGameForm->setCellWidget(h-1,w-2,lb);

    //生成随机终点
    int randNumber;
    randNumber = qrand() % 3;
    while(randNumber==0)
    {
        randNumber = qrand() % 3;
    }
    QLabel *lb = new QLabel();
    QMovie *qm = new QMovie();
    randChar = QString::number(randNumber,10);
    QString randEndPoint = ":/img/endPoint" + randChar + ".gif";
    lb->setGeometry(gameFormx+(w-2)*this->tableWidgetItemSize,gameFormy+(h-1)*this->tableWidgetItemSize,this->tableWidgetItemSize,this->tableWidgetItemSize);
    qm->setFileName(randEndPoint);
    lb->setMovie(qm);
    lb->setScaledContents(true);
    qm->start();
    ui->tableWidgetGameForm->setCellWidget(h-1,w-2,lb);
}

//表格设置初始化
void MainWindow::initializeTableWidget()
{
    //置零几何大小
    ui->tableWidgetGameForm->setGeometry(0,0,0,0);

    //隐藏表头
    ui->tableWidgetGameForm->verticalHeader()->hide();
    ui->tableWidgetGameForm->horizontalHeader()->hide();

    //隐藏网格线
    ui->tableWidgetGameForm->setShowGrid(false);

    //单元格不可选择
    ui->tableWidgetGameForm->setSelectionMode(QAbstractItemView::NoSelection);

    //禁用滚动条
    ui->tableWidgetGameForm->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
    ui->tableWidgetGameForm->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //不可编辑
    ui->tableWidgetGameForm->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

//开始新游戏 表格数据预处理 逻辑数据预处理 ///timer设定入口 起点终点设定入口**************************
void MainWindow::StartNewGame()
{
    //关闭自动寻路
    this->isStartAutoPath = false;
    this->autoPathtimer->stop();

    //初始化操作数
    this->countOfOperation = 0;
    ui->labelValueCount->setText(QString::number(this->countOfOperation,10));
    this->isStart = true;

    //修改提示信息至'已开始'
    SetStartInformationLabel(1);

    //表格数据预处理
    try
    {
    //清除表
    ui->tableWidgetGameForm->clear();

    //创建表 行与列
    ui->tableWidgetGameForm->setRowCount(this->mazeHigh);
    ui->tableWidgetGameForm->setColumnCount(this->mazeWidth);

    //设定图长宽以及单元格长宽
    int sizew = this->mazeWidth*this->tableWidgetItemSize;
    int sizeh = this->mazeHigh*this->tableWidgetItemSize;
    gameFormx = (this->maxGameFormSize-sizew)/2;
    gameFormy = (this->maxGameFormSize-sizeh)/2;

    //自适应处理
    {
        float floatsizew = sizew;
        float floatsizeh = sizeh;
        float adapativeFactor;
        //正方形
        if(sizew == sizeh)
        {
            adapativeFactor = this->maxGameFormSize/floatsizew;
            this->tableWidgetItemSize*=adapativeFactor;
            sizew = this->mazeWidth*this->tableWidgetItemSize;
            sizeh = this->mazeHigh*this->tableWidgetItemSize;
            gameFormx=gameFormy=0;
        }
        //宽大于高的矩形处理
        else if(sizew>sizeh)
        {
            adapativeFactor = this->maxGameFormSize/floatsizew;
            this->tableWidgetItemSize*=adapativeFactor;
            sizew = this->mazeWidth*this->tableWidgetItemSize;
            sizeh = this->mazeHigh*this->tableWidgetItemSize;
            gameFormx=0;
            gameFormy = (this->maxGameFormSize-sizeh)/2;
        }
        //高大于宽的处理
        else if(sizeh>sizew)
        {
            adapativeFactor = this->maxGameFormSize/floatsizeh;
            this->tableWidgetItemSize*=adapativeFactor;
            sizew = this->mazeWidth*this->tableWidgetItemSize;
            sizeh = this->mazeHigh*this->tableWidgetItemSize;
            gameFormy=0;
            gameFormx = (this->maxGameFormSize-sizew)/2;
        }
    }

    ui->tableWidgetGameForm->setGeometry(gameFormx,gameFormy,sizew,sizeh);

    //初始化人物img
    ui->Player->setPixmap(QPixmap(":/img/rabbit_front.png").scaled(this->tableWidgetItemSize,this->tableWidgetItemSize));

    //初始化上次key值
    this->lastKey = NULL;

    //设定单元格宽和高
    ui->tableWidgetGameForm->verticalHeader()->setDefaultSectionSize(this->tableWidgetItemSize);
    ui->tableWidgetGameForm->horizontalHeader()->setDefaultSectionSize(this->tableWidgetItemSize);

    //测试代码
    qDebug()<<"CreateMap w:"<<sizew<<" h:"<<sizeh;
    qDebug()<<"x:"<<gameFormx<<" y:"<<gameFormy;

    //填充可视化图
    ShowMap(this->mazeHigh,this->mazeWidth,false);

    //逻辑数据预处理

    //初始化玩家坐标以及显示控制人物 初始化终点
    opr.SetEndPosition(this->mazeWidth,this->mazeHigh);

    opr.AutoSetStartPosition(this->mazeWidth,this->mazeHigh);

    SetUIPlayerStartPosition(gameFormx + opr.playery*this->tableWidgetItemSize,gameFormy + opr.playerx*this->tableWidgetItemSize);

    ui->Player->setVisible(true);
    }
    catch(QException e)
    {
        qDebug()<<e.what();
    }

    //启用循环解锁键盘timer 设置反应时间
    if(!keyEventtimer->isActive())
    {
    keyEventtimer->start(100);
    }

}

//增加操作数
void MainWindow::AddCountOfOperation()
{
    this->countOfOperation++;
    ui->labelValueCount->setText(QString::number(this->countOfOperation,10));
}

//响应按键 在图中修改人物位置 (动态移动)
void MainWindow::UIChangePlayerPosition(char dir)
{
    //获得坐标
    int sx = ui->Player->x(),ex = ui->Player->x();
    int sy = ui->Player->y(),ey = ui->Player->y();

    //处理人物矩形位置
    switch(dir)
    {
        case 'w':
        {
            ey -= this->tableWidgetItemSize;
            break;
        }
        case 'a':
        {
            ex -= this->tableWidgetItemSize;
            break;
        }
        case 's':
        {
            ey += this->tableWidgetItemSize;
            break;
        }
        case 'd':
        {
            ex += this->tableWidgetItemSize;
            break;
        }
        default:
        {
            return;
        }
    }
    //动态移动
    SetPlayerAnimation(sx,sy,ex,ey);
    //静态移动
    //ui->Player->setGeometry(ex,ey,this->tableWidgetItemSize,this->tableWidgetItemSize);
}

//测试用 输出玩家逻辑位置与UI位置
void MainWindow::TestShowPosition()
{
    qDebug()<<"x:"<<opr.playerx<<" y:"<<opr.playery;
    qDebug()<<"UIx:"<<ui->Player->x()<<" UIy:"<<ui->Player->y();
}

//设定人物初始化位置(UI)
void MainWindow::SetUIPlayerStartPosition(int x,int y)
{
    ui->Player->setGeometry(x,y,this->tableWidgetItemSize,this->tableWidgetItemSize);
}

//修改开始状态信息(Label) 1-开始 0-未开始
void MainWindow::SetStartInformationLabel(int opr)
{
    QPalette pa;

    switch(opr)
    {
    case 0:
    {
        this->pa.setColor(QPalette::WindowText,Qt::red);
        ui->labelStatusIsStart->setPalette(this->pa);
        ui->labelStatusIsStart->setText("未开始");
        break;
    }
    case 1:
    {
        this->pa.setColor(QPalette::WindowText,Qt::green);
        ui->labelStatusIsStart->setPalette(this->pa);
        ui->labelStatusIsStart->setText("已开始");
        break;
    }
    default:
    {
        break;
    }
    }
}

//游戏胜利 初始化信息
void MainWindow::GameVictory()
{
    //触发测试按钮点击信号
    emit ui->pushButtonTest->clicked();
}

//重设表格设置
void MainWindow::ResetTableWidget()
{
    //清除图并将表格几何归零
    ui->tableWidgetGameForm->clear();
    ui->tableWidgetGameForm->setGeometry(0,0,0,0);

    //表格坐标归零
    this->gameFormx = this->gameFormy = 0;

    //初始化单元格尺寸
    this->tableWidgetItemSize = 30;

    //图宽/高 归零
    this->mazeHigh = 0;
    this->mazeWidth = 0;
    ui->labelValueHigh->setText(QString::number(0,10));
    ui->labelValueWidth->setText(QString::number(0,10));
}

//设定人物动态/静态img
void MainWindow::SetPlayerMovie(char key)
{
    QMovie *qm = new QMovie();

    //根据按键判断选择的动态图
    switch(key)
    {
        case 'w':
        {
            qm->setFileName(":/img/rabbit_back_dynamic.gif");
            break;
        }
        case 'a':
        {
            qm->setFileName(":/img/rabbit_left_dynamic.gif");
            break;
        }
        case 's':
        {
            qm->setFileName(":/img/rabbit_front_dynamic.gif");
            break;
        }
        case 'd':
        {
            qm->setFileName(":/img/rabbit_right_dynamic.gif");
            break;
        }
        default:
        {
        return;
        }
    }

    ui->Player->setMovie(qm);
    qm->start();
}

//设定人物移动动画效果
void MainWindow::SetPlayerAnimation(int sx,int sy,int ex,int ey)
{
    this->playerAnimation->setDuration(200);//移动时间为200ms
    this->playerAnimation->setStartValue(QRect(sx,sy,this->tableWidgetItemSize,this->tableWidgetItemSize));
    this->playerAnimation->setEndValue(QRect(ex,ey,this->tableWidgetItemSize,this->tableWidgetItemSize));
    this->playerAnimation->start();
}

//测试按钮 强制结束 信息归零 (用于到达终点时信息处理)
void MainWindow::on_pushButtonTest_clicked()
{
    //重设读取地图路径
    readMapPath = nullptr;

    //关闭人物
    ui->Player->setVisible(false);

    //重设逻辑 宽高 起始位置 终点位置
    opr.Reset();

    //停止游戏声明 阻塞键盘事件
    this->isStart = false;
    this->keyEventStop = true;
    this->isStartAutoPath = false;

    //关闭keyEventtimer 循环解锁键盘事件
    if(this->keyEventtimer->isActive())
    {
        ChangePushBoxText('w',0);
        ChangePushBoxText('a',0);
        ChangePushBoxText('s',0);
        ChangePushBoxText('d',0);
        this->keyEventtimer->stop();
    }

    //关闭自动寻路
    this->autoPathtimer->stop();

    //解锁键盘事件
    this->keyEventStop = false;

    //重设表格
    ResetTableWidget();
    //开始提示修改至未开始状态
    SetStartInformationLabel(0);
}

//自动寻路的循环操作-timerout槽
void MainWindow::AutoPath()
{
    //获取按键 若按键有效
    char k = 'p';
    qDebug()<<"QueueSize:"<<opr.v.size();

    if(!opr.v.empty())
    {
        k = opr.v.dequeue();

        if(lastKey != k)
        {
        SetPlayerMovie(k);
        }
        qDebug()<<"AutoMove:"<<k;
        opr.ChangePlayerPosition(k);
        UIChangePlayerPosition(k);
        AddCountOfOperation();
        lastKey = k;

        if(opr.IsGameVictory())
        {
            QMessageBox vir;
            vir.setWindowTitle("胜利");
            vir.setText("你好像到了终点");
            vir.setModal(true);
            vir.show();
            if(vir.exec())
            {
                GameVictory();
            }
        }
    }
    else
    {
        return;
    }
}

//按钮-开启自动寻路
void MainWindow::on_pushButtonPath_clicked()
{
        try {
            //只有在未在自动寻路时才开始
            if(this->isStartAutoPath == false)
            {
                //只有游戏开始时 才可以自动寻路
                if(this->isStart == true)
                {
                    //重设操作数
                    this->countOfOperation = 0;
                    ui->labelValueCount->setText(QString::number(this->countOfOperation,10));

                    //手动设置起点为1*1 ********************* 防bug手段
                    opr.SetStartPosition(1,1);
                    this->SetUIPlayerStartPosition(gameFormx+this->tableWidgetItemSize,gameFormy+this->tableWidgetItemSize);
                    this->SetPlayerMovie('s');

                    //生成自动路径
                    opr.CreatePath();

                    //不存在路径时 跳出
                    if(opr.v.empty())
                    {
                        QMessageBox *noPath = new QMessageBox();
                        noPath->setWindowTitle("Error");
                        noPath->setText("此图不存在路径!");
                        noPath->show();
                        return;
                    }

                /*
                qDebug()<<opr.v.size();
                opr.v.clear();
                return;
                */

                qDebug()<<"Starting Auto Path";
                //锁定键盘事件
                this->isStartAutoPath = true;

                this->autoPathtimer->start(250);
                }
        }
    } catch (QException e) {
        qDebug()<<e.what();
    }

}

//按键-读取地图
void MainWindow::on_pushButtonReadMap_clicked()
{
    //
    this->readMapPath = QFileDialog::getOpenFileName(this, tr("open file"), " ",  tr("txt.file(*.txt)"));

    //空路径 返回
    if(readMapPath == nullptr)
    {
        return;
    }

    //图可行
    if(opr.IsFeasibleMap(readMapPath))
    {
        //设置数组宽高以及标签宽高
        this->mazeWidth = opr.readMapW;
        this->mazeHigh = opr.readMapH;
        opr.SetMapSize(opr.readMapW,opr.readMapH);
        ui->labelValueWidth->setText(QString::number(opr.readMapW,10));
        ui->labelValueHigh->setText(QString::number(opr.readMapH,10));

        opr.ReadMap();
        StartNewGame();
    }
    //违规图
    else
    {
        QString errorMessage = readMapPath + "该位置地图不符合要求!";
        QMessageBox *error = new QMessageBox();
        error->setWindowTitle("Error");
        error->setText(errorMessage);
        error->setIcon(QMessageBox::Icon::Warning);
        error->show();
    }
}

//用于更改窗体尺寸
void MainWindow::on_pushButtonChangeFormSize_clicked()
{
    //若正在自动寻路时 不允许修改窗体尺寸
    if(this->isStartAutoPath == true)
    {
        return;
    }


    //修改尺寸参数
    this->FormSize = (this->FormSize + 1) % 3;

    //根据参数修改窗体 0-大 1-中 2-小
    switch(FormSize)
    {
        case 0: //1200*900
        {
            this->maxGameFormSize = 900;
            this->tableWidgetItemSize = 30;
            this->setMinimumSize(1200,900);
            this->setMaximumSize(1200,900);
            ui->tableWidgetGameForm->setGeometry(0,0,900,900);
            ui->widgetGameForm->setGeometry(0,0,900,900);
            ui->widgetInformation->setGeometry(900,0,300,900);
            break;
        }
        case 1: //1000*750
            {
            this->maxGameFormSize = 750;
            this->tableWidgetItemSize = 25;
            this->setMinimumSize(1000,750);
            this->setMaximumSize(1000,750);
            ui->tableWidgetGameForm->setGeometry(0,0,750,750);
            ui->widgetGameForm->setGeometry(0,0,750,750);
            ui->widgetInformation->setGeometry(750,0,250,750);
            break;
            }
        case 2: //800*600
            {
                this->maxGameFormSize = 600;
                this->tableWidgetItemSize = 20;
                this->setMinimumSize(800,600);
                this->setMaximumSize(800,600);
                ui->tableWidgetGameForm->setGeometry(0,0,600,600);
                ui->widgetGameForm->setGeometry(0,0,600,600);
                ui->widgetInformation->setGeometry(600,0,200,600);
                break;
            }
        default:
            break;
    }

    //若游戏正在进行时修改则重新输出地图
    if(isStart == true)
    {

        //清除表
        ui->tableWidgetGameForm->clear();

        //创建表 行与列
        ui->tableWidgetGameForm->setRowCount(this->mazeHigh);
        ui->tableWidgetGameForm->setColumnCount(this->mazeWidth);

        //设定图长宽以及单元格长宽
        int sizew = this->mazeWidth*this->tableWidgetItemSize;
        int sizeh = this->mazeHigh*this->tableWidgetItemSize;
        gameFormx = (this->maxGameFormSize-sizew)/2;
        gameFormy = (this->maxGameFormSize-sizeh)/2;

        //自适应处理
        {
            float floatsizew = sizew;
            float floatsizeh = sizeh;
            float adapativeFactor;
            //正方形
            if(sizew == sizeh)
            {
                adapativeFactor = this->maxGameFormSize/floatsizew;
                this->tableWidgetItemSize*=adapativeFactor;
                sizew = this->mazeWidth*this->tableWidgetItemSize;
                sizeh = this->mazeHigh*this->tableWidgetItemSize;
                gameFormx=gameFormy=0;
            }
            //宽大于高的矩形处理
            else if(sizew>sizeh)
            {
                adapativeFactor = this->maxGameFormSize/floatsizew;
                this->tableWidgetItemSize*=adapativeFactor;
                sizew = this->mazeWidth*this->tableWidgetItemSize;
                sizeh = this->mazeHigh*this->tableWidgetItemSize;
                gameFormx=0;
                gameFormy = (this->maxGameFormSize-sizeh)/2;
            }
            //高大于宽的处理
            else if(sizeh>sizew)
            {
                adapativeFactor = this->maxGameFormSize/floatsizeh;
                this->tableWidgetItemSize*=adapativeFactor;
                sizew = this->mazeWidth*this->tableWidgetItemSize;
                sizeh = this->mazeHigh*this->tableWidgetItemSize;
                gameFormy=0;
                gameFormx = (this->maxGameFormSize-sizew)/2;
            }
        }

        //设定单元格宽和高
        ui->tableWidgetGameForm->verticalHeader()->setDefaultSectionSize(this->tableWidgetItemSize);
        ui->tableWidgetGameForm->horizontalHeader()->setDefaultSectionSize(this->tableWidgetItemSize);

        this->ShowMap(this->mazeHigh,this->mazeWidth,true);

        //重新输出玩家大小极其位置
        SetUIPlayerStartPosition(gameFormx + opr.playery*this->tableWidgetItemSize,gameFormy + opr.playerx*this->tableWidgetItemSize);
    }
}
