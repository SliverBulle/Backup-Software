#include "widget.h"
#include "./ui_widget.h"

#include "path.h"
#include <QDebug>
#include <cstdlib>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::docbackup)
{
    ui->setupUi(this);
    msgBox.setStyleSheet("QLabel{"
                         "min-width: 150px;"
                         "min-height: 50px; "
                         "}");
    groupButton_operation = new QButtonGroup(this);
    groupButton_operation->addButton(ui->backup_button,0);
    groupButton_operation->addButton(ui->reduct_button,1);
    groupButton_method = new QButtonGroup(this);
    groupButton_method->addButton(ui->tar_button,0);
    groupButton_method->addButton(ui->tar_zip_button,1);
    groupButton_encry = new QButtonGroup(this);
    groupButton_encry->addButton(ui->encry_button,0);
    groupButton_encry->addButton(ui->noencry_button,1);

    connect(ui->pushButton_execute,SIGNAL(clicked(bool)),this,SLOT(on_pushButton_execute_clicked()));
    connect(ui->backup_button,SIGNAL(clicked(bool)),this,SLOT(on_groupBox_operation_clicked()));
    connect(ui->reduct_button,SIGNAL(clicked(bool)),this,SLOT(on_groupBox_operation_clicked()));
    connect(ui->tar_button,SIGNAL(clicked(bool)),this,SLOT(on_groupBox_method_clicked()));
    connect(ui->tar_zip_button,SIGNAL(clicked(bool)),this,SLOT(on_groupBox_method_clicked()));
    connect(ui->encry_button,SIGNAL(clicked(bool)),this,SLOT(on_groupBox_encry_clicked()));
    connect(ui->noencry_button,SIGNAL(clicked(bool)),this,SLOT(on_groupBox_encry_clicked()));

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_execute_clicked() //确认按钮
{
    int workcode = 0;
    QString worktext;
    source = ui->lineEdit_source->text();
    target = ui->lineEdit_target->text();
    password = ui->lineEdit_password->text();

    if(op1<0 || op4<0 || (op4>0 && op1==0 && op2==0)){
        workcode = 1;
    }
    else opCode = 8*op1 + 4*op2 + 2*op3 + op4;

    if(source.isEmpty()){
        workcode = 2;
    }
    else if(target.isEmpty()){
        workcode = 3;
    }
    else if(op4==1&&password.isEmpty()){
        workcode = 4;
    }

    ui->lineEdit_source->clear();
    ui->lineEdit_target->clear();
    ui->lineEdit_password->clear();

    Init();//全体初始化
    QByteArray s, t;
    s = source.toLocal8Bit();
    t = target.toLocal8Bit();
    char *S = s.data();
    char *T = t.data();
    int PW = password.toInt();
    QString pw_ = QString::number(PW);
    if(op4==1 && pw_!=password && workcode!=4 && workcode!=1) workcode = 8;

    if(workcode==0){
        workcode = ReadIn(opCode, S, T, PW);
        if(workcode==0) workcode = Execute(opCode);
    }

    switch(workcode){   //判断操作码并反馈
    case 0:
        worktext = "操作成功";
        break;
    case 1:
        worktext = "指令错误";
        break;
    case 2:
        worktext = "源地址为空";
        break;
    case 3:
        worktext = "目的地址为空";
        break;
    case 4:
        worktext = "密码为空";
        break;
    case 5:
        worktext = "源地址错误";
        break;
    case 6:
        worktext = "目的地址错误";
        break;
    case 9:
        worktext = "密码错误";
        break;
    case 8:
        worktext = "密码应是整数";
        break;
    }

    msgBox.setText(worktext);
    msgBox.exec();
    exit(0);
    return ;
}


void Widget::on_groupBox_operation_clicked()  //“操作区”按钮
{
    switch(groupButton_operation->checkedId())
    {
    case 0:
        op1 = 0;
        break;
    case 1:
        op1 = 1;
        break;
    }
}


void Widget::on_groupBox_method_clicked()   //“备份方式”区按钮
{
    switch(groupButton_method->checkedId())
    {
    case 0:
        op2 = 1;
        op3 = 0;
        break;
    case 1:
        op2 = 1;
        op3 = 1;
        break;
    default:
        op2 = 0;
        op3 = 0;
        break;
    }
}


void Widget::on_groupBox_encry_clicked()  //“是否加密”区按钮
{
    switch(groupButton_encry->checkedId())
    {
    case 0:
        op4 = 1;
        break;
    case 1:
        op4 = 0;
        break;
    }
}

