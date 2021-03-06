﻿#include "calculateandmove.h"
#include "ui_calculateandmove.h"
#include <QDebug>
#include <QtConcurrent>
#include <QThread>
#include <QWindow>
#include <QTimer>
CalculateAndMove::CalculateAndMove(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CalculateAndMove),process(nullptr)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_NativeWindow, true);
}

CalculateAndMove::~CalculateAndMove()
{
    if(hwnWindow!=0)
        SendMessage(hwnWindow,WM_CLOSE,0,0);
    if(process){
        if(process->state()==QProcess::Running){
            process->terminate();
            process->waitForFinished(30000);
       }
       delete process;
   }
    delete ui;
}

void CalculateAndMove::on_insetUe4_clicked()
{
    startUe4();
}

void CalculateAndMove::on_deleteUe4_clicked()
{
    if(hwnWindow==0)
        return;
    SendMessage(hwnWindow,WM_CLOSE,0,0);
}

void CalculateAndMove::startUe4(){
    //启动程序
    QString unreal4Path{"D:/QtProject/QtWithUnreal4/WindowsNoEditor/DemoGame.exe"};
    QStringList arguments;
    arguments << "-WINDOWED";
    process=new QProcess;
    process->start(unreal4Path,arguments);

    QtConcurrent::run([this]{
        while (true) {
            //通过窗口名称取得窗口句柄
            //hwnWindow=FindWindow(NULL,L"DemoGame  ");
            hwnWindow=FindWindow(NULL,L"DemoGame (32-bit, PCD3D_SM5) ");
            qDebug()<<hwnWindow;
            if(hwnWindow!=0)
            {
                connect(this,SIGNAL(insetUe4Complete()),this,SLOT(insetUe4()));
                emit insetUe4Complete();
                break;
            }
        }
    });
}

void CalculateAndMove::insetUe4(){  
////----------------------------------------------
////方法一，进qt后需要调用解除客户区锁定函数（Ue4
//    ue4Window=QWindow::fromWinId(WId(hwnWindow));
//    ue4Window->setParent( this->windowHandle());
//    ue4Window->setGeometry(0,0,ui->label->width(),ui->label->height());
//    ue4Window->show();

 //----------------------------------------------
 //方法二，进qt后需要调用解除客户区锁定函数（Ue4
    QRect rect=ui->label->geometry();
    QPoint pos=ui->label->mapToGlobal(ui->label->pos());
    qDebug()<<"rect:"<<rect;
    qDebug()<<"worldPos:"<<pos;
    QRect worldRect={rect.x()+pos.x(),rect.y()+pos.y(),rect.width()+pos.x(),rect.height()+pos.y()};

    SetParent(hwnWindow,(HWND)QWidget::winId());

    MoveWindow(hwnWindow,rect.x(),rect.y(), rect.width(), rect.height(), true);
    ::SetWindowPos( hwnWindow, nullptr,rect.x(), rect.y(), rect.width(), rect.height(), SWP_NOZORDER | SWP_FRAMECHANGED| SWP_NOCOPYBITS );


    qDebug()<<"worldRect:"<<worldRect;
////调整客户区大小
//    WINDOWINFO WindowInfo;
//    ZeroMemory(&WindowInfo,sizeof(WindowInfo));
//    WindowInfo.cbSize = sizeof(WindowInfo);
//    ::GetWindowInfo(hwnWindow, &WindowInfo);

//    QPoint pos=ui->label->mapToGlobal(ui->label->pos());

//    RECT TestRect;
//    TestRect.left =pos.x();
//    TestRect.right =pos.x()+rect.width();
//    TestRect.top =pos.y();
//    TestRect.bottom = pos.y()+rect.height();
//    AdjustWindowRectEx(&TestRect, WindowInfo.dwStyle, false, WindowInfo.dwExStyle);

//----------------------------------------------


    LPRECT lprect;
    GetClientRect(hwnWindow,lprect);
    qDebug()<<(int)lprect->left<<(int)lprect->top<<(int)lprect->right<<(int)lprect->bottom;

    InvalidateRect(hwnWindow,0,true) ;
    UpdateWindow(hwnWindow);

    this->repaint();
}

void CalculateAndMove::on_pushButton_clicked()
{
    LPRECT lprect;
//    GetClientRect(hwnWindow,lprect);
//    qDebug()<<(int)lprect->left<<(int)lprect->top<<(int)lprect->right<<(int)lprect->bottom;
    InvalidateRect(hwnWindow,0,true) ;
    UpdateWindow(hwnWindow);

//    RedrawWindow(hwndWindow,);

//    BringWindowToTop (hwnWindow);
//    SetForegroundWindow(hwnWindow);
}

void CalculateAndMove::moveEvent(QMoveEvent *event){

    event->accept();
}
