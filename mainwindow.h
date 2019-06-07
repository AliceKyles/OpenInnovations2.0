#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QVBoxLayout>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private slots:

    void fillp(QString q);
    void lang(QString text);
    void par(QString text);
    void adsearch();
    void search();
    void cancel();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QSqlDatabase db;
    int id;
    QWidget* container;
    QVBoxLayout* layout;
    Ui::MainWindow *ui;
    void allp(QString q);
    void openSQL();
    void ppic();
    void addpic(QByteArray q);
    void desc(QString q, QString q1, QString q2, QString q3);
    void organ(QString q, QString q1, QString q2);
    void onep(QString q, QString q1);
    void econ(QString q, QString q1, QString q2, QString q3, QString q4);
};

#endif // MAINWINDOW_H
