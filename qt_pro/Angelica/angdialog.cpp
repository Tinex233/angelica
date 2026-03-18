#include "angdialog.h"

AngTxtDialog::AngTxtDialog(QWidget *parent)
{   
    // 创建一个新的对话框窗口
    textWindow = new QDialog(parent);
    textWindow->resize(1000, 600);

    // 创建文本框并设置属性
    textEdit = new QTextEdit(textWindow);
    textEdit->setStyleSheet("QScrollBar:vertical{"
            "width:8px;"
            "margin:0px,0px,0px,0px;"
            "padding-top:9px;"
            "padding-bottom:9px;"
            "background-color:rgb(0,171,149);}");
    textEdit->setReadOnly(true);//只读
    textEdit->setWordWrapMode(QTextOption::WrapAnywhere);

    // 设置布局（确保文本框填满窗口）
    QVBoxLayout *layout = new QVBoxLayout(textWindow);
    layout->addWidget(textEdit);
    textWindow->setLayout(layout);
}

void AngTxtDialog::setText(const QString &text)
{
    if(textEdit)
        textEdit->setText(text);
}

void AngTxtDialog::setTitle(const QString &str)
{
    if(textWindow)
        textWindow->setWindowTitle(str);
}

void AngTxtDialog::show()
{
    if(textWindow)
        textWindow->show();
}

AngTxtDialog::~AngTxtDialog()
{
    if(!textEdit)
    {
        delete textEdit;
        textEdit=nullptr;
    }

    if(!textWindow)
    {
        delete textWindow;
        textWindow=nullptr;
    }
}


AngTableDialog::AngTableDialog(QWidget *parent,const QString &labelTitle)
{
    setParent(parent);
    setMinimumSize(700,900);
    setWindowIcon(QIcon(":/new/icon/resources/Angelica+.ico"));
    setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);

    loading = new QLabel(this);
    loading->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    loading->setAlignment(Qt::AlignCenter);
    loading->setMovie(new QMovie(":/new/ai/resources/ai/loading.gif"));
    loading->movie()->stop();

    failLoading = new QPushButton(this);
    failLoading->setIcon(QIcon(":/new/general/resources/refresh-30.png"));
    failLoading->setIconSize(QSize(17,17));
    failLoading->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    failLoading->setCursor(Qt::PointingHandCursor);
    failLoading->setText(" 加载失败，请点击重新加载");
    failLoading->setStyleSheet("border:0;"
            "font: 12pt \"微软雅黑\";"
            "color:#999999;");
    connect(failLoading,&QPushButton::clicked,this,[=]{
        //qDebug()<<"signal: AngTableDialog reload";
        setState(loadingState::LOADING);
        emit reload();
    });

    noData = new QLabel(this);
    noData->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    noData->setText("暂无相关信息");
    noData->setAlignment(Qt::AlignCenter);
    noData->setStyleSheet("border:0;"
            "font: 12pt \"微软雅黑\";"
            "color:#999999;");

    titleLabel = new QLabel(labelTitle,this);

    titleLabel->setStyleSheet("font: 75 14pt \"微软雅黑\";"
            "color:#fff;"
            "background-color:rgb(0,171,149);"
            "border-radius:0;"
            "border-top-left-radius:14px;"
            "border-top-right-radius:14px;"
            "border:1px solid #d5d5d5;"
            "border-bottom:0;");

    titleLabel->setFixedSize(160,50);
    titleLabel->setAlignment(Qt::AlignCenter);

    QHBoxLayout *hl1=new QHBoxLayout();
    hl1->addSpacing(15);
    hl1->addWidget(titleLabel);
    hl1->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    mainLayout->addLayout(hl1);

    QFrame *frame = new QFrame(this);
    frame->setStyleSheet("QFrame{background-color: #F7F8FA;"
            "border:1px solid #d5d5d5;"
            "border-radius:0;"
            "border-top-left-radius:6px;"
            "border-top-right-radius:6px;}");

    mainLayout->addWidget(frame);

    //设置框架内部的垂直布局
    QVBoxLayout *fvl = new QVBoxLayout(frame);

    // 创建表格部件（层级5）
    tw = new QTableWidget(frame);
    tw->setStyleSheet(R"(QTableWidget{border:0;}
                      QScrollBar:vertical
                      {width:8px;
                      margin:0px,0px,0px,0px;
                      padding-top:9px;
                      padding-bottom:9px;
                      background-color:rgb(0,171,149);})");
    tw->setColumnCount(1); // 只需一列（每个 recordItem 占满整行）
    tw->horizontalHeader()->hide(); // 隐藏水平表头
    tw->verticalHeader()->hide();   // 隐藏垂直表头
    tw->setShowGrid(false);         // 隐藏表格线
    tw->setSelectionBehavior(QAbstractItemView::SelectRows); // 整行选中
    tw->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    tw->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tw->verticalHeader()->setDefaultSectionSize(123);
    tw->setFocusPolicy(Qt::NoFocus);

    fvl->addWidget(tw);
    fvl->addWidget(loading);
    fvl->addWidget(noData);
    fvl->addWidget(failLoading);

}

void AngTableDialog::setState(const loadingState &state)
{
    if(state==loadingState::LOADING)
    {
        loading->setHidden(false);
        loading->movie()->start();
        noData->hide();
        tw->hide();
        failLoading->hide();
    }
    else if(state==loadingState::FAIL)
    {
        loading->hide();
        loading->movie()->stop();
        noData->hide();
        tw->hide();
        failLoading->setHidden(false);
    }
    else if(state==loadingState::NO_RESULT)
    {
        loading->hide();
        loading->movie()->stop();
        noData->setHidden(false);
        tw->hide();
        failLoading->hide();
    }
    else if(state==loadingState::SUCCESS)
    {
        loading->hide();
        loading->movie()->stop();
        noData->hide();
        tw->setHidden(false);
        failLoading->hide();
    }
}

void AngTableDialog::clickReloadBtn()
{
    if(failLoading!=nullptr)
        failLoading->click();
}

QTableWidget *AngTableDialog::getTableWidget()
{
    return this->tw;
}

AngTableDialog::~AngTableDialog()
{

}

AngMessageWindow::AngMessageWindow(const QString &message, QWidget *parent)
{
    setParent(parent);
    setFixedSize(350,200);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    showTimer = new QTimer(this);
    stayTimer = new QTimer(this);
    closeTimer = new QTimer(this);
    m_nDesktopHeight=QApplication::desktop()->height();
    connect(showTimer, SIGNAL(timeout()), this, SLOT(onMove()));
    connect(stayTimer, SIGNAL(timeout()), this, SLOT(onStay()));
    connect(closeTimer, SIGNAL(timeout()), this, SLOT(onClose()));

    // 创建顶部布局（关键修改）
    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(0);  // 控件间距

    // 1. 左侧图标（固定大小）
    QLabel *iconLabel = new QLabel(this);
    iconLabel->setPixmap(QPixmap(":/new/icon/resources/Angelica+.ico").scaled(24, 24, Qt::KeepAspectRatio));
    iconLabel->setFixedSize(35, 35);  // 固定图标尺寸
    headerLayout->addWidget(iconLabel);

    // 2. 中间软件名（自动拉伸）
    QLabel *titleLabel = new QLabel("芷宁 — 消息提醒", this);
    titleLabel->setObjectName("title");
    titleLabel->setStyleSheet("font: 8pt \"微软雅黑\";");
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    titleLabel->setFixedSize(160,20);
    headerLayout->addWidget(titleLabel, 1);
    headerLayout->setSpacing(0);
    headerLayout->addSpacerItem(new QSpacerItem(12, 12, QSizePolicy::Expanding, QSizePolicy::Minimum));

    // 3. 右侧关闭按钮（固定大小）
    QPushButton *closeButton = new QPushButton("×", this);
    closeButton->setFixedSize(24, 24);  // 固定按钮尺寸
    closeButton->setStyleSheet(R"(
        QPushButton {
            font-size: 18px;
            color: #666;
            background: transparent;
            border: none;
            min-width: 24px;
            min-height: 24px;
        }
        QPushButton:hover { color: rgb(0, 171, 149); }
    )");
    closeButton->setCursor(Qt::PointingHandCursor);
    connect(closeButton, &QPushButton::clicked, this, &QWidget::close);
    headerLayout->addWidget(closeButton);

    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background-color:rgba(0, 171, 149, 0.2);");

    // 主布局结构优化
    QVBoxLayout *vl = new QVBoxLayout();
    vl->setSpacing(0);  // 主布局内控件间距
    vl->addLayout(headerLayout);
    vl->addSpacing(5);
    vl->addWidget(line);

    // 内容区域优化
    QLabel *contentLabel = new QLabel(message, this);
    contentLabel->setWordWrap(true);
    contentLabel->setAlignment(Qt::AlignCenter);
    contentLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    contentLabel->setStyleSheet("font: 9pt \"微软雅黑\";");
    vl->addWidget(contentLabel);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    QFrame *frame=new QFrame(this);
    frame->setLayout(vl);
    frame->setStyleSheet("background-color:rgb(229, 244, 242);border-radius:8px;");
    mainLayout->addWidget(frame);
}

AngMessageWindow::~AngMessageWindow()
{
    delete showTimer;
    delete stayTimer;
    delete closeTimer;
}

void AngMessageWindow::showMsg()
{
    QRect rect = QApplication::desktop()->availableGeometry();
    point.setX(rect.width() - width());
    point.setY(rect.height() - height());
    move(point.x(), point.y());
    showTimer->start(3);

    show();
}

// 移动
void AngMessageWindow::onMove()
{
    m_nDesktopHeight--;
    move(point.x(), m_nDesktopHeight);
    if (m_nDesktopHeight <= point.y())
    {
        showTimer->stop();
        stayTimer->start(5000);
    }
}

// 驻留
void AngMessageWindow::onStay()
{
    stayTimer->stop();
    closeTimer->start(100);
}

// 关闭
void AngMessageWindow::onClose()
{
    transparent -= 0.1;
    if (transparent <= 0.0)
    {
        closeTimer->stop();
        close();
    }
    else
    {
        setWindowOpacity(transparent);
    }
}
