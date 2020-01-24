#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "requestdialog.h"
#include "QTimer"
#include "QPalette"
#include "operation.h"
#include "QPropertyAnimation"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

//槽函数定义入口
public slots:
    void ChangeLabelHighAndWidth();
    void ReceiveValue(int w,int h);
    void AutoPath();
    void SetEventStop();

private slots:
    void on_pushButtonTest_clicked();
    void on_pushButtonPath_clicked();
    void on_pushButtonReadMap_clicked();

    void on_pushButtonChangeFormSize_clicked();

private:
    void ChangePushBoxText(char key,int dir);
    void ShowMap(int h,int w,bool isChangeSize);
    void initializeTableWidget();
    void StartNewGame();
    void AddCountOfOperation();
    void UIChangePlayerPosition(char dir);
    void SetUIPlayerStartPosition(int x,int y);
    void SetStartInformationLabel(int opr);
    void GameVictory();
    void ResetTableWidget();
    void SetPlayerMovie(char key);
    void SetPlayerAnimation(int sx,int sy,int ex,int ey);

    void TestShowPosition();

protected:
    void keyPressEvent(QKeyEvent *e);

private:
    bool isStart = false;
    Ui::MainWindow *ui;
    requestDialog *resDialog;
    QTimer *keyEventtimer;
    QTimer *autoPathtimer;
    QPalette pa;
    Operation opr;
    bool keyEventStop = false;
    bool isStartAutoPath =false;
    int gameFormx;
    int gameFormy;
    int mapThemeFactor;

    //重要变量 实际窗口区域 以及 固定的 单元格尺寸
    int maxGameFormSize = 900; //实际游戏区域 更改处****************
    int tableWidgetItemSize = 30;  //标准尺寸 更改处**************

    QString readMapPath;
    int mazeHigh;
    int mazeWidth;
    int countOfOperation = 0;
    char lastKey = 'o';
    QPropertyAnimation *playerAnimation;

    //窗体尺寸因素 0-大 1-中 2-小
    int FormSize = 0;
};

#endif // MAINWINDOW_H
