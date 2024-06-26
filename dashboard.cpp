#include "dashboard.h"
#include "ui_dashboard.h"
#include "ui_goboard.h"
#include "ui_replay.h"

dashboard::dashboard(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::dashboard)
{
    ui->setupUi(this);

    this->b = new goBoard;
    this->r = new rule;
    this->re = new replay;

    connect(this->b,SIGNAL(back()),this,SLOT(comeBackToPrev()));
    connect(this, &dashboard::sendCID, b, &goBoard::setUserID);
    connect(this, &dashboard::initializeBoard, b->ui->label, &myLabel::initialization);
    connect(ui->tableView, &QTableView::doubleClicked, this, &dashboard::on_tableView_doubleClicked);
    connect(this->re, SIGNAL(finish()), this, SLOT(finishReplay()));  // 连接复盘完成信号到槽函数

}

dashboard::~dashboard()
{
    delete ui;
}

void dashboard::on_start_clicked()
{
    if(!connectToSQL(db))
    {
        return;
    }

    if(!createTable_c(username, db))
    {
        return;
    }

    QSqlQuery query(db);

    QString tableName = username + "_c";

    query.prepare("INSERT INTO " + tableName + " (user_id) VALUES (?)");
    query.addBindValue(user_id);

    if(!query.exec())
    {
        qDebug() << "Update failed:" << query.lastError();
        return;
    }

    c_id = query.lastInsertId().toInt();

    emit sendCID(c_id, username);
    emit initializeBoard();

    this->close();

    b->showFullScreen();
}

void dashboard::comeBackToPrev()
{
    this->b->close();
    this->show();
}

void dashboard::setUserID(int id, QString name)
{
    user_id = id;
    username = name;
    qDebug() << user_id << " " << username;
}

void dashboard::on_show_clicked()
{
    if (!connectToSQL(db))
    {
        //QMessageBox::critical(this, "Database Error", "Failed to connect to the database.");
        return;
    }

    if (!createTable_c(username, db))
    {
        //QMessageBox::critical(this, "Database Error", "Failed to create or populate the table.");
        return;
    }

    // Create the model
    QSqlQueryModel *model = new QSqlQueryModel(this);

    // Set the query to the model
    QString tableName = username + "_c";
    QSqlQuery query(db);

    query.prepare("SELECT id, winner, c_time AS time FROM " + tableName);
    if (!query.exec())
    {
        qDebug() << "Query execution failed:" << query.lastError();
        //QMessageBox::critical(this, "Query Error", "Failed to execute the query.");
        return;
    }

    model->setQuery(query);

    if (model->lastError().isValid())
    {
        qDebug() << "Model query failed:" << model->lastError();
        //QMessageBox::critical(this, "Model Error", "Failed to set query on the model.");
        return;
    }

    // Print number of rows to debug
    qDebug() << "Number of rows in the result set:" << model->rowCount();

    // Create a view and set the model
    ui->tableView->setModel(model);

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}


void dashboard::on_notice_clicked()
{
    //this->close();
    r->show();
}


void dashboard::on_tableView_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    c_id = index.sibling(index.row(), 0).data().toInt(); // 获取点击行的对局ID

    qDebug() << c_id;

    startReplay(c_id, username);
}

void dashboard::startReplay(int c_id, QString username)
{
    this->re->setCID(c_id, username); // 设置复盘的对局ID和用户名
    this->close();
    this->re->ui->label->initialization();
    this->re->showFullScreen();
}

void dashboard::finishReplay()
{
    this->re->close();
    this->show();
}

void dashboard::paintEvent(QPaintEvent *)
{
    QPainter painter_dash(this);
    QPixmap pix_dash;
    int nImageWidth_dash=width();

    pix_dash.load("C:/Users/steph/Desktop/dashboard.jpg");

    painter_dash.drawPixmap(0,0,nImageWidth_dash,height(),pix_dash);
}
