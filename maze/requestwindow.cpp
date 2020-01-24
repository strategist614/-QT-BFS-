#include "requestwindow.h"
#include "ui_requestwindow.h"

requestWindow::requestWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::requestWindow)
{
    ui->setupUi(this);
}

requestWindow::~requestWindow()
{
    delete ui;
}
