#include "angitem.h"

//recordItem类实现
recordItem::recordItem(const QString &i,const QString &p,const QString &t,const QString &l,const orderStatus &s,QWidget *parent)
{
    this->setParent(parent);
    this->setFixedHeight(110);

    //对继承的子类设置样式时，一定要启用背景绘制
    this->setAttribute(Qt::WA_StyledBackground, true);

    setCursor(Qt::PointingHandCursor);

    //设置样式
    setStyleSheet(R"(recordItem{
                  background-color: rgb(255, 255, 255);
                  border:1px solid #d5d5d5;
                  border-radius:4px;}
                  QLabel{border:0;background-color: rgba(255, 255, 255, 0);}
                  recordItem:hover{border:2px solid rgb(0,171,149);background-color: rgba(0,171,149,0.2);})");

    id=new QLabel(this);
    patient=new QLabel(this);
    time=new QLabel(this);
    status=new QLabel(this);
    location=new QLabel(this);

    id->setText("订单号："+i);
    id->setStyleSheet("font: 8pt \"微软雅黑\";");

    patient->setText("就诊人:"+p);
    patient->setStyleSheet("font: 75 12pt \"微软雅黑\";");

    time->setText("就诊时间："+t);
    time->setStyleSheet("font: 75 9pt \"微软雅黑\";");

    location->setText("就诊地点："+l);
    location->setStyleSheet("font: 75 9pt \"微软雅黑\";");

    if(s==FINISH)
    {
        status->setText("已完成");
        status->setStyleSheet("font: 10pt \"微软雅黑\";color: rgb(0, 170, 0);");
    }
    else if(s==WAITING)
    {
        status->setText("待就诊");
        status->setStyleSheet("font: 10pt \"微软雅黑\";color: rgb(255, 0, 0);");
    }
    else if(s==CANCEL)
    {
        status->setText("已取消");
        status->setStyleSheet("font: 10pt \"微软雅黑\";color: rgb(0, 170, 255);");
    }
    else if(s==BREAK)
    {
        status->setText("已爽约");
        status->setStyleSheet("font: 10pt \"微软雅黑\";color: rgb(255, 170, 0);");
    }
    status->setAlignment(Qt::AlignCenter);
    status->setFixedWidth(100);

    //布局
    QHBoxLayout *hl1=new QHBoxLayout();
    hl1->addSpacing(20);
    hl1->addWidget(patient);
    hl1->addSpacing(50);
    hl1->addWidget(time);
    hl1->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    QHBoxLayout *hl2=new QHBoxLayout();
    hl2->addSpacing(20);
    hl2->addWidget(location);
    hl2->addSpacing(50);
    hl2->addWidget(id);
    hl2->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    QVBoxLayout *vl=new QVBoxLayout();
    vl->addLayout(hl1);
    vl->addLayout(hl2);
    vl->setSpacing(6);

    QHBoxLayout *hl3=new QHBoxLayout(this);
    hl3->setSpacing(6);
    hl3->addLayout(vl);
    hl3->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    hl3->addWidget(status);

    this->setLayout(hl3);
}

void recordItem::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked(this->id->text().mid(4)); // 发射信号并传递字符串
    }
}

patientItem::patientItem(const int &pn,const QString &n, const ID_TYPE &type, const QString &num, const QString &p , const QString &c, QWidget *parent)
{
    this->setParent(parent);
    this->setFixedHeight(110);
    //对继承的子类设置样式时，一定要启用背景绘制
    this->setAttribute(Qt::WA_StyledBackground, true);

    //setCursor(Qt::PointingHandCursor);

    //设置样式
    setStyleSheet(R"(patientItem{
                  background-color: rgb(255, 255, 255);
                  border:1px solid #d5d5d5;
                  border-radius:4px;}
                  QLabel{border:0;background-color: rgba(255, 255, 255, 0);})");

    patientNum=pn;

    name=new QLabel(this);
    idType=new QLabel(this);
    idNum=new QLabel(this);
    phone=new QLabel(this);
    cons=new QLabel(this);

    name->setText(n);
    idNum->setText(num);
    phone->setText(p);

    if(type==ID_TYPE::DALU)
        idType->setText("居民身份证");
    else if(type==ID_TYPE::HUZHAO)
        idType->setText("护照");
    else if(type==ID_TYPE::GANGAO)
        idType->setText("港澳居民来往内地通行证");
    else if(type==ID_TYPE::TAIWAN)
        idType->setText("台湾居民来往大陆通行证");

    cons->setText("体质："+(c.isEmpty()?"未测试":c));

    name->setStyleSheet("font: 75 12pt \"微软雅黑\";");
    idType->setStyleSheet("color:#3d3d3d; font: 9pt \"微软雅黑\";");
    idNum->setStyleSheet("color:#3d3d3d; font: 9pt \"微软雅黑\";");
    phone->setStyleSheet("color:#3d3d3d; font: 9pt \"微软雅黑\";");
    cons->setStyleSheet("color:#3d3d3d; font: 9pt \"微软雅黑\";");

    delBTN=new QPushButton(this);

    delBTN->setText("删除");

    delBTN->setStyleSheet("border:0;background-color:#fff;color:rgb(255,0,0); font: 10pt \"微软雅黑\";");
    delBTN->setFixedWidth(60);
    delBTN->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    delBTN->setCursor(Qt::PointingHandCursor);

    modBTN=new QPushButton(this);

    modBTN->setText("修改");

    modBTN->setStyleSheet("border:0;background-color:#fff;color:rgb(0,170,255); font: 10pt \"微软雅黑\";");
    modBTN->setFixedWidth(60);
    modBTN->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    modBTN->setCursor(Qt::PointingHandCursor);

    //布局
    QHBoxLayout *hl1=new QHBoxLayout();
    hl1->addSpacing(20);
    hl1->addWidget(name);
    hl1->addSpacing(20);
    hl1->addWidget(phone);
    hl1->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    QHBoxLayout *hl2=new QHBoxLayout();
    hl2->addSpacing(20);
    hl2->addWidget(idType);
    hl2->addSpacing(5);
    hl2->addWidget(idNum);
    hl2->addSpacing(30);
    hl2->addWidget(cons);
    hl2->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    QVBoxLayout *vl=new QVBoxLayout();
    vl->addLayout(hl1);
    vl->addLayout(hl2);
    vl->setSpacing(6);

    QHBoxLayout *hl3=new QHBoxLayout(this);
    hl3->setSpacing(0);
    hl3->addLayout(vl);
    hl3->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    hl3->addWidget(modBTN);
    hl3->addSpacing(20);
    hl3->addWidget(delBTN);

    //绑定按钮槽函数
    connect(delBTN, &QPushButton::clicked, this, [=]() {
        emit delBTNclicked(patientNum);
    });

    connect(modBTN, &QPushButton::clicked, this, [=]() {
        int ty;
        if(idType->text()=="居民身份证")
            ty=0;
        else if(idType->text()=="护照")
            ty=1;
        else if(idType->text()=="港澳居民来往内地通行证")
            ty=2;
        else if(idType->text()=="台湾居民来往大陆通行证")
            ty=3;
        emit modBTNclicked(patientNum, name->text(),ty,idNum->text(),phone->text(),cons->text());
    });

    this->setLayout(hl3);
}

void patientItem::switchToSelectable()
{
    setStyleSheet(R"(patientItem{
                  background-color: rgb(255, 255, 255);
                  border:1px solid #d5d5d5;
                  border-radius:4px;}
                  QLabel{border:0;background-color: rgba(255, 255, 255, 0);}
                  patientItem:hover{border:2px solid rgb(0,171,149);background-color: rgba(0,171,149,0.2);})");
    delBTN->hide();
    modBTN->hide();
    setCursor(Qt::PointingHandCursor);
}

void patientItem::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit itemClicked(patientNum);
    }
}

hospitalItem::hospitalItem(const int &i,const QString &n,const QString &r,const QString &a,const QString &l,const QString &path,QWidget *parent)
{
    this->setParent(parent);
    this->setFixedHeight(110);
    //对继承的子类设置样式时，一定要启用背景绘制
    this->setAttribute(Qt::WA_StyledBackground, true);

    setCursor(Qt::PointingHandCursor);

    //设置样式
    setStyleSheet(R"(hospitalItem{
                  background-color: rgb(255, 255, 255);
                  border:1px solid #d5d5d5;
                  border-radius:4px;}
                  QLabel{border:0;background-color: rgba(255, 255, 255, 0);}
                  hospitalItem:hover{border:2px solid rgb(0,171,149);background-color: rgba(0,171,149,0.2);})");

    id=i;

    name=new QLabel(n,this);
    region=new QLabel(r,this);
    address=new QLabel(a,this);
    level=new QLabel(" "+l+" ",this);

    name->setStyleSheet("font: 75 12pt \"微软雅黑\";");
    region->setStyleSheet("color:#3d3d3d; font: 9pt \"微软雅黑\";");
    address->setStyleSheet("color:#3d3d3d; font: 9pt \"微软雅黑\";");
    level->setStyleSheet("color:rgb(0,170,255); font: 7pt \"微软雅黑\";background-color:rgba(0,170,255,0.2);border-radius:2px;");
    level->setFixedHeight(22);
    level->setAlignment(Qt::AlignCenter);

    pic = new QLabel(this);
    pic->setFixedSize(70, 70);
    pic->setAlignment(Qt::AlignCenter);

    // 设置圆形遮罩
    QRegion maskRegion(QRect(0, 0, 70, 70), QRegion::Ellipse);
    pic->setMask(maskRegion);

    // 加载并缩放图片
    QPixmap pixmap(path);
    pixmap = pixmap.scaled(
        pic->size(),
        Qt::KeepAspectRatioByExpanding,  // 保持比例扩展
        Qt::SmoothTransformation         // 平滑缩放
    );
    pic->setPixmap(pixmap);

    //布局
    QHBoxLayout *hl1=new QHBoxLayout();
    hl1->addSpacing(20);
    hl1->addWidget(name);
    hl1->addSpacing(20);
    hl1->addWidget(region);
    hl1->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    QHBoxLayout *hl2=new QHBoxLayout();
    hl2->addSpacing(20);
    hl2->addWidget(level);
    hl2->addSpacing(30);
    hl2->addWidget(address);
    hl2->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    QVBoxLayout *vl=new QVBoxLayout();
    vl->addLayout(hl1);
    vl->addLayout(hl2);
    vl->setSpacing(6);

    QHBoxLayout *hl3=new QHBoxLayout(this);
    hl3->addSpacing(20);
    hl3->addWidget(pic);
    hl3->setSpacing(20);
    hl3->addLayout(vl);
    hl3->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    this->setLayout(hl3);
}
void hospitalItem::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked(id);
    }
}

doctorItem::doctorItem(const doctor doc, QWidget *parent)
{
    this->setParent(parent);
    this->setFixedHeight(110);
    //对继承的子类设置样式时，一定要启用背景绘制
    this->setAttribute(Qt::WA_StyledBackground, true);

    setCursor(Qt::PointingHandCursor);

    //设置样式
    setStyleSheet(R"(doctorItem{
                  background-color: rgb(255, 255, 255);
                  border:1px solid #d5d5d5;
                  border-radius:4px;}
                  QLabel{border:0;background-color: rgba(255, 255, 255, 0);}
                  doctorItem:hover{border:2px solid rgb(0,171,149);background-color: rgba(0,171,149,0.2);})");

    this->doc=doc;

    id=doc.id;

    this->name=new QLabel(doc.name,this);
    this->hospital=new QLabel(doc.hospital,this);
    this->department=new QLabel(doc.department,this);
    this->title=new QLabel(doc.title,this);
    this->price=new QLabel("￥"+QString::number(doc.price),this);

    pic = new QLabel(this);
    pic->setFixedSize(70, 70);
    pic->setAlignment(Qt::AlignCenter);

    // 设置圆形遮罩
    QRegion maskRegion(QRect(0, 0, 70, 70), QRegion::Ellipse);
    pic->setMask(maskRegion);

    // 加载并缩放图片
    QPixmap pixmap(doc.path);
    pixmap = pixmap.scaled(
        pic->size(),
        Qt::KeepAspectRatioByExpanding,  // 保持比例扩展
        Qt::SmoothTransformation         // 平滑缩放
    );
    pic->setPixmap(pixmap);

    name->setStyleSheet("font: 75 12pt \"微软雅黑\";");
    title->setStyleSheet("color:#3d3d3d; font: 9pt \"微软雅黑\";");
    department->setStyleSheet("color:#3d3d3d; font: 9pt \"微软雅黑\";");
    hospital->setStyleSheet("color:#3d3d3d; font: 9pt \"微软雅黑\";");
    price->setStyleSheet("color:#ff0000; font: 10pt \"微软雅黑\";");
    price->setAlignment(Qt::AlignCenter);
    price->setFixedWidth(60);

    //布局
    QHBoxLayout *hl1=new QHBoxLayout();
    hl1->addSpacing(20);
    hl1->addWidget(name);
    hl1->addSpacing(20);
    hl1->addWidget(title);
    hl1->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    QHBoxLayout *hl2=new QHBoxLayout();
    hl2->addSpacing(20);
    hl2->addWidget(hospital);
    hl2->addSpacing(30);
    hl2->addWidget(department);
    hl2->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    QVBoxLayout *vl=new QVBoxLayout();
    vl->addLayout(hl1);
    vl->addLayout(hl2);
    vl->setSpacing(6);

    QHBoxLayout *hl3=new QHBoxLayout(this);
    hl3->addSpacing(20);
    hl3->addWidget(pic);
    hl3->setSpacing(20);
    hl3->addLayout(vl);
    hl3->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    hl3->addWidget(price);

    this->setLayout(hl3);
}

void doctorItem::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked(this->doc);
    }
}
