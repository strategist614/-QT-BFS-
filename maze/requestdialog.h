#ifndef REQUESTDIALOG_H
#define REQUESTDIALOG_H

#include <QDialog>

namespace Ui {
class requestDialog;
}

class requestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit requestDialog(QWidget *parent = nullptr);
    ~requestDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

signals:
    void SendValue(int w,int h);

private:
    Ui::requestDialog *ui;

};

#endif // REQUESTDIALOG_H
