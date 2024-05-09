#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QMessageBox>

#include"dashboard.h"
#include"toregister.h"

class dashboard;
class toRegister;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    dashboard *d = NULL;
    toRegister *r = NULL;

private slots:

    void on_login_clicked();

    void on_register_2_clicked();
    void comeBackToPrev();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
};
#endif // MAINWINDOW_H
