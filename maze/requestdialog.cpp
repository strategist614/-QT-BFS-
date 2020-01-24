#include "requestdialog.h"
#include "ui_requestdialog.h"
#include "QMessageBox"
#include "QString"

requestDialog::requestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::requestDialog)
{
    ui->setupUi(this);

    //固定窗口
    this->setFixedSize(this->size());
    this->setFocus();
}

requestDialog::~requestDialog()
{
    delete ui;
}

void requestDialog::on_buttonBox_accepted()
{
    if(ui->lineEditHigh->text() == "" || ui->lineEditWidth->text() == "")
    {
        QMessageBox::information(this,tr("Error"),tr("宽或高为空!"));
    }
    else if(ui->lineEditHigh->text().toInt()<5 || ui->lineEditWidth->text().toInt()<5||ui->lineEditHigh->text().toInt()>30||ui->lineEditWidth->text().toInt()>30)
    {
        QMessageBox::information(this,tr("Error"),tr("宽或高的值超出给定范围内\n给定范围[5,30]"));
    }
    else
    {
        //数据触发触发信号
        emit SendValue(ui->lineEditWidth->text().toInt(),ui->lineEditHigh->text().toInt());
    }

}

void requestDialog::on_buttonBox_rejected()
{
    this->~requestDialog();
}
