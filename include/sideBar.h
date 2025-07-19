#pragma once

#include <QPushButton>
#include <QButtonGroup>
#include <QStringList>
#include <QWidget>

class SideBar:public QWidget{
    Q_OBJECT
    public:
        SideBar(QWidget *widget = nullptr);
        ~SideBar();

    private:
        QStringList icon_name_list;
        QPushButton *lastBtn;
        int lastIndex;
        QButtonGroup *btnGroup;
    
    signals:
        void sendSidebarIndex(int index);
};