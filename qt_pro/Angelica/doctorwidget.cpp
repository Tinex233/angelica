#include "doctorwidget.h"
#include "ui_doctorwidget.h"

doctorWidget::doctorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::doctorWidget)
{
    ui->setupUi(this);
    setWindowTitle("就诊医师");
    setParent(parent);
    setWindowModality(Qt::ApplicationModal);
    setMinimumSize(700,900);
    setWindowIcon(QIcon(":/new/icon/resources/Angelica+.ico"));
    setAttribute(Qt::WA_DeleteOnClose);

    QMovie *mv=new QMovie(this);
    mv->setFileName(":/new/ai/resources/ai/loading.gif");
    ui->loading_label->setMovie(mv);
    ui->loading_label->movie()->stop();

    ui->doc_tableWidget->setColumnCount(1); // 只需一列（每个 recordItem 占满整行）
    ui->doc_tableWidget->horizontalHeader()->hide(); // 隐藏水平表头
    ui->doc_tableWidget->verticalHeader()->hide();   // 隐藏垂直表头
    ui->doc_tableWidget->setShowGrid(false);         // 隐藏表格线
    ui->doc_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); // 整行选中
    ui->doc_tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->doc_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->doc_tableWidget->verticalHeader()->setDefaultSectionSize(123);
    ui->doc_tableWidget->setFocusPolicy(Qt::NoFocus);

    connect(ui->return_btn,&QPushButton::clicked,this,[=]{ui->stackedWidget->setCurrentIndex(0);});

    connect(ui->submit_btn,&QPushButton::clicked,this,[=]{
        emit submitDoctor(curDoctor,ui->name->text(),ui->price->text());
        close();
    });

    this->setState(loadingState::LOADING);
}

doctorWidget::~doctorWidget()
{
    delete ui;
}

void doctorWidget::setState(const loadingState &state)
{
    if(state==loadingState::LOADING)
    {
        ui->loading_label->setHidden(false);
        ui->loading_label->movie()->start();
        ui->noData_label->hide();
        ui->doc_tableWidget->hide();
        ui->failLoad_btn->hide();
    }
    else if(state==loadingState::FAIL)
    {
        ui->loading_label->hide();
        ui->loading_label->movie()->stop();
        ui->noData_label->hide();
        ui->doc_tableWidget->hide();
        ui->failLoad_btn->setHidden(false);
    }
    else if(state==loadingState::NO_RESULT)
    {
        ui->loading_label->hide();
        ui->loading_label->movie()->stop();
        ui->noData_label->setHidden(false);
        ui->doc_tableWidget->hide();
        ui->failLoad_btn->hide();
    }
    else if(state==loadingState::SUCCESS)
    {
        ui->loading_label->hide();
        ui->loading_label->movie()->stop();
        ui->noData_label->hide();
        ui->doc_tableWidget->setHidden(false);
        ui->failLoad_btn->hide();
    }
}

void doctorWidget::addItem(doctorItem *doctor)
{
    int row=ui->doc_tableWidget->rowCount();
    ui->doc_tableWidget->setRowCount(row+1);
    ui->doc_tableWidget->setCellWidget(row++, 0, doctor);
}

void doctorWidget::handleItemClicked(const doctor &doc)
{
    curDoctor=doc.id;

    ui->stackedWidget->setCurrentIndex(1);
    ui->head->setFixedSize(80, 80);
    ui->head->setAlignment(Qt::AlignCenter);

    // 设置圆形遮罩
    QRegion maskRegion(QRect(0, 0, 80, 80), QRegion::Ellipse);
    ui->head->setMask(maskRegion);

    // 加载并缩放图片
    QPixmap pixmap(doc.path);
    pixmap = pixmap.scaled(
        ui->head->size(),
        Qt::KeepAspectRatioByExpanding,  // 保持比例扩展
        Qt::SmoothTransformation         // 平滑缩放
    );
    ui->head->setPixmap(pixmap);

    ui->name->setText(doc.name);
    ui->title->setText(doc.title);
    ui->department->setText(doc.department);
    ui->intro->setText("\n    "+doc.intro);
    ui->exp->setText("\n  "+doc.exp);
    ui->price->setText("￥"+QString::number(doc.price));
}
