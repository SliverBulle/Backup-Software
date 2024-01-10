#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QButtonGroup>
#include <QMessageBox>


QT_BEGIN_NAMESPACE
namespace Ui {
class docbackup;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


private slots://槽函数
    void on_pushButton_execute_clicked();

    void on_groupBox_operation_clicked();

    void on_groupBox_method_clicked();

    void on_groupBox_encry_clicked();

private:
    Ui::docbackup *ui;
    QButtonGroup *groupButton_operation;
    QButtonGroup *groupButton_method;
    QButtonGroup *groupButton_encry;
    int op1=-1,op2=0,op3=0,op4=-1,opCode;
    int workcode = 0;
    QString source, target, password;
    QString worktext;
    QMessageBox msgBox;

};
#endif // WIDGET_H
