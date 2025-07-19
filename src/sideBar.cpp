#include "sideBar.h"
#include <QFile>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSpacerItem>
    
SideBar::SideBar(QWidget *widget):QWidget(widget)
{
    this->setFixedWidth(80);

    icon_name_list << "ai_boot" << "open_sidebar" << "new_session";
    
    btnGroup = new QButtonGroup(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(30);
    layout->setAlignment(Qt::AlignCenter);
    btnGroup->setExclusive(true); // 设置互斥

    QSpacerItem *spacer = new QSpacerItem(10, 30, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addSpacerItem(spacer);

    for (int i = 0; i < 3; ++i) {
        QPushButton *btn = new QPushButton(this);
        QString iconPath;

		iconPath = QString(":/Resource/ico/%1.png").arg(icon_name_list.at(i));
		lastBtn = btn;
		lastIndex = i;

        btn->setIcon(QIcon(iconPath));                      // 设置初始ICON
        if(i==0){
            btn->setIconSize(QSize(44, 44));  
        }
        else if(i==1){
             btn->setIconSize(QSize(40, 40));  
        }
        else{
            btn->setIconSize(QSize(44, 44));  
        }
       
        btn->setObjectName(icon_name_list.at(i));           // 设置objectName方便QSS
        btnGroup->addButton(btn, i);
        layout->addWidget(btn);
    }
    
    layout->addStretch();

    connect(btnGroup, &QButtonGroup::idClicked, this, [&](int id){
        emit sendSidebarIndex(id);
    });

    this->setStyleSheet(R"(
		QWidget{
			background-color:rgba(255,255,255,0);	
			border:none;
		}
	)");
}

SideBar::~SideBar()
{
}
