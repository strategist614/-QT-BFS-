#ifndef REQUESTWINDOW_H
#define REQUESTWINDOW_H

#include <QWidget>

namespace Ui {
class requestWindow;
}

class requestWindow : public QWidget
{
    Q_OBJECT

public:
    explicit requestWindow(QWidget *parent = nullptr);
    ~requestWindow();

private:
    Ui::requestWindow *ui;
};

#endif // REQUESTWINDOW_H
