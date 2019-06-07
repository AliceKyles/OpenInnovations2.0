#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDate>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    openSQL();
    container = new QWidget();
    container->resize(ui->plist->size());
    container->setStyleSheet("background: white;");
    ui->textBrowser->setStyleSheet("background: white;");
    ui->searchfield->setStyleSheet("background: white;");
    ui->pic->setStyleSheet("background: white;");
    ui->ppar->setStyleSheet("background: white;");
    ui->language->setStyleSheet("background: white;");
    layout = new QVBoxLayout(container);
    id=0;
    allp("SELECT name FROM Projectru");
    ui->searchfield->setHidden(1);
    ui->cancel->setHidden(1);
    connect(ui->language, SIGNAL(currentTextChanged(QString)), this, SLOT(lang(QString)));
    connect(ui->ppar, SIGNAL(currentTextChanged(QString)), this, SLOT(par(QString)));
    connect(ui->asearch, SIGNAL(clicked()), this, SLOT(adsearch()));
    connect(ui->search, SIGNAL(clicked()), this, SLOT(search()));
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(cancel()));
}

void MainWindow::cancel() // отменяет поиск
{
    if (ui->language->currentText()=="English")
       allp("SELECT name FROM Projecten");
    else allp("SELECT name FROM Projectru");
    ui->cancel->setHidden(1);
}


void MainWindow::search() // создаёт запрос по параметрам поиска
{
    QString q;
    if (ui->searchbar->text().isEmpty()&&ui->searchfield->isHidden())
        return void();
    if (ui->language->currentText()=="English")
       q="SELECT Projecten.name FROM Projecten INNER JOIN Organizationen on Projecten.OID = Organizationen.ID INNER JOIN Projectes on Projecten.ID = Projectes.ID WHERE Projecten.name LIKE '%"+ui->searchbar->text()+"%' ";
    else q="SELECT Projectru.name FROM Projectru INNER JOIN Organizationru on Projectru.OID = Organizationru.ID INNER JOIN Projectes on Projectru.ID = Projectes.ID WHERE Projectru.name LIKE '%"+ui->searchbar->text()+"%' ";
    if (!ui->searchfield->isHidden()){
        if (!ui->orgpr->text().isEmpty()){
            if (ui->language->currentText()=="English")
                q.append(" AND Organizationen.name LIKE '%"+ui->orgpr->text()+"%' ");
            else q.append(" AND Organizationru.name LIKE '%"+ui->orgpr->text()+"%' ");
        }
        if (!ui->autpr->text().isEmpty()){
            if (ui->language->currentText()=="English")
                q.append(" AND Projecten.author LIKE '%"+ui->autpr->text()+"%' ");
            else q.append(" AND Projectru.author LIKE '%"+ui->autpr->text()+"%' ");
        }
        if (!ui->inv1->text().isEmpty())
                q.append(" AND Projectes.invest >"+ui->inv1->text()+" ");
        if (!ui->inv2->text().isEmpty())
                q.append(" AND Projectes.invest <"+ui->inv2->text()+" ");
        if (!ui->categor->text().isEmpty()){
            if (ui->language->currentText()=="English")
                q.append(" AND Projecten.category LIKE '%"+ui->categor->text()+"%' ");
            if (ui->language->currentText()=="Русский")
                q.append(" AND Projectru.category LIKE '%"+ui->categor->text()+"%' ");
        }
        if (ui->datedf->date()!=QDate(2000,1,1))
                q.append(" AND Projectes.datedev >'"+ui->datedf->date().toString("yyyy-MM-dd")+"' ");
        if (ui->datedt->date()!=QDate(2000,1,1))
                q.append(" AND Projectes.datedev <'"+ui->datedt->date().toString("yyyy-MM-dd")+"' ");
        if (ui->dateif->date()!=QDate(2000,1,1))
                q.append(" AND Projectes.dateinv >'"+ui->dateif->date().toString("yyyy-MM-dd")+"' ");
        if (ui->dateit->date()!=QDate(2000,1,1))
                q.append(" AND Projectes.dateinv <'"+ui->dateit->date().toString("yyyy-MM-dd")+"' ");
        if (!ui->stag->text().isEmpty()){
            if (ui->language->currentText()=="English")
                q.append(" AND Projecten.stage LIKE '%"+ui->stag->text()+"%' ");
            else q.append(" AND Projectru.stage LIKE '%"+ui->stag->text()+"%' ");
        }
    }
    allp(q);
    ui->cancel->setHidden(0);
}

void MainWindow::allp(QString q) //выводит проекты по запросу
{
    while(!layout->isEmpty()) {
      delete layout->takeAt(0)->widget();
    }
    QSqlQuery query(db);
    query.exec(q);
    while (query.next()){
        QString s, t;
        s=t=query.value(0).toString();
        while (s.size()>70){
            s.chop(s.length()-s.lastIndexOf(" "));
            s.append("...");
        }
        QPushButton * button1 = new QPushButton(" "+s,container);
        connect(button1, &QPushButton::clicked, this, [=] () { fillp(t); });
        button1->setStyleSheet ("text-align: left;" "background: lightgrey;");
        button1->setFixedWidth(ui->plist->width()-40);
        layout->addWidget(button1);
    }
    ui->plist->setWidget(container);
}

void MainWindow::fillp(QString q) //выводит данные по выбранному проекту
{
    QSqlQuery query(db);
    if (ui->language->currentText()=="English")
       query.prepare("SELECT ID FROM Projecten WHERE name = '"+q+"'");
    else query.prepare("SELECT ID FROM Projectru WHERE name = '"+q+"'");
    query.exec();
    query.next();
    id=query.value(0).toInt();
    while (q.size()>40){
        q.chop(q.length()-q.lastIndexOf(" "));
        q.append("...");
    }
    ui->pname->setText(q);
    if (id>0){
        if (ui->language->currentText()=="English")
           query.prepare("SELECT category FROM Projecten WHERE ID =:id");
        else query.prepare("SELECT category FROM Projectru WHERE ID =:id");
        query.bindValue(":id", id);
        query.exec();
        if (query.next()){
            q=query.value(0).toString();
            while (q.size()>40){
                q.chop(q.length()-q.lastIndexOf(" "));
                q.append("...");
            }
            if (ui->language->currentText()=="English")
               ui->categ->setText("Category: "+q );
            if (ui->language->currentText()=="Русский")
               ui->categ->setText("Категория: "+q );
        }
        ui->ppar->setCurrentIndex(2);
        ui->ppar->setCurrentIndex(0);
        ppic();
    }
    else {
        ui->textBrowser->clear();
        ui->pic->clear();
        ui->pname->clear();
    }
}

void MainWindow::lang(QString text) //меняет язык программы
{
    QSqlQuery query(db);
    if (ui->language->currentText()=="English")
       query.prepare("SELECT name FROM Projecten WHERE ID =:id");
    else query.prepare("SELECT name FROM Projectru WHERE ID =:id");
    query.bindValue(":id", id);
    query.exec();
    query.next();
    QString q=query.value(0).toString();
    if (text=="English"){
        allp("SELECT name FROM Projecten");
        ui->search->setText("Search");
        ui->asearch->setText("Advanced search");
        ui->cancel->setText("Cancel");
        ui->orp->setText("Name of organization");
        ui->authp->setText("Authors");
        ui->invest->setText("Amount of investments");
        ui->cat->setText("Category");
        ui->datedev->setText("Project completion date");
        ui->dateinv->setText("Project payback date");
        ui->stage->setText("Stage");
        ui->finv->setText("from");
        ui->tinv->setText("to");
        ui->fddev->setText("from");
        ui->tddev->setText("to");
        ui->fdinv->setText("from");
        ui->tdinv->setText("to");
        ui->inv1->setGeometry(ui->inv1->x()+5,ui->inv1->y(),ui->inv1->width(),ui->inv1->height());
        ui->datedf->setGeometry(ui->datedf->x()+5,ui->datedf->y(),ui->datedf->width(),ui->datedf->height());
        ui->dateif->setGeometry(ui->dateif->x()+5,ui->dateif->y(),ui->dateif->width(),ui->dateif->height());
        ui->ppar->setItemText(0,"Project description");
        ui->ppar->setItemText(1,"Organization");
        ui->ppar->setItemText(2,"Actuality");
        ui->ppar->setItemText(3,"Example of application");
        ui->ppar->setItemText(4,"Authors");
        ui->ppar->setItemText(5,"Legal protection");
        ui->ppar->setItemText(6,"Economic information");
        ui->categ->setText("Category: ");
    }
    else {
        allp("SELECT name FROM Projectru");
        ui->search->setText("Поиск");
        ui->asearch->setText("Расширенный поиск");
        ui->cancel->setText("Отменить");
        ui->orp->setText("Название организации");
        ui->authp->setText("Авторы проекта");
        ui->invest->setText("Сумма инвестиций");
        ui->cat->setText("Категория");
        ui->datedev->setText("Дата завершения проекта");
        ui->dateinv->setText("Дата оккупаемости проекта");
        ui->stage->setText("Стадия проекта");
        ui->finv->setText("от");
        ui->tinv->setText("до");
        ui->fddev->setText("от");
        ui->tddev->setText("до");
        ui->fdinv->setText("от");
        ui->tdinv->setText("до");
        ui->inv1->setGeometry(ui->inv1->x()-5,ui->inv1->y(),ui->inv1->width(),ui->inv1->height());
        ui->datedf->setGeometry(ui->datedf->x()-5,ui->datedf->y(),ui->datedf->width(),ui->datedf->height());
        ui->dateif->setGeometry(ui->dateif->x()-5,ui->dateif->y(),ui->dateif->width(),ui->dateif->height());
        ui->ppar->setItemText(0,"Описание проекта");
        ui->ppar->setItemText(1,"Организация");
        ui->ppar->setItemText(2,"Актуальность");
        ui->ppar->setItemText(3,"Примеры применения");
        ui->ppar->setItemText(4,"Авторы проекта");
        ui->ppar->setItemText(5,"Правовая защита");
        ui->ppar->setItemText(6,"Экономическая информация");
        ui->categ->setText("Категория: ");
    }
    fillp(q);
}

void MainWindow::par(QString q) // выводит выбранную часть информации о проекте
{
    if (id==0){
        ui->textBrowser->clear();
        ui->pic->clear();
        return void();
    }
    QStringList myOptions;
    QSqlQuery query(db);
    myOptions << "Описание проекта" << "Организация" << "Актуальность"<< "Примеры применения"<< "Авторы проекта"<< "Правовая защита"<< "Экономическая информация"<<"Project description" << "Organization" << "Actuality"<< "Example of application"<< "Authors"<< "Legal protection"<< "Economic information";
    switch(myOptions.indexOf(q)){
      case 0:
        desc("ru","Название проекта: ","Проблема: ","Решение: ");
        break;

      case 1:
        organ("ru","Название организации: ","Контактная информация: ");
        break;

      case 2:
        onep("ru", "actuality");
        break;

      case 3:
        onep("ru", "examples");
        break;

      case 4:
        onep("ru", "author");
        break;

      case 5:
        onep("ru", "protection");
        break;

      case 6:
        econ("ru", "Сумма инвестиций: ","Стадия проекта: ","Ожидаемая дата завершения проекта: ","Ожидаемая дата окупаемости проекта: ");
        break;

      case 7:
        desc("en","Project name: ","Problem: ","Solution: ");
        break;

      case 8:
        organ("en","Name of organization: ","Contact information: ");
        break;

      case 9:
        onep("en", "actuality");
        break;

      case 10:
        onep("en", "examples");
        break;

      case 11:
        onep("en", "author");
        break;

      case 12:
        onep("en", "protection");
        break;

      case 13:
        econ("en", "Amount of investments: ","Project stage: ","Expected project completion date: ","Expected project payback date: ");
        break;

      default:
        ui->textBrowser->setText("Sorry, some error has occured");
        break;
    }
}

void MainWindow::desc(QString q, QString q1, QString q2, QString q3)//выводит описание проекта
{
    QSqlQuery query(db);
    query.prepare("SELECT name FROM Project"+q+" WHERE ID =:id");
    query.bindValue(":id", id);
    query.exec();
    query.next();
    ui->textBrowser->setText(q1+query.value(0).toString());
    ui->textBrowser->append("");
    query.prepare("SELECT problem FROM Project"+q+" WHERE ID =:id");
    query.bindValue(":id", id);
    query.exec();
    query.next();
    ui->textBrowser->append(q2+query.value(0).toString());
    ui->textBrowser->append("");
    query.prepare("SELECT solution FROM Project"+q+" WHERE ID =:id");
    query.bindValue(":id", id);
    query.exec();
    query.next();
    ui->textBrowser->append(q3+query.value(0).toString());
    ppic();
    }

void MainWindow::organ(QString q, QString q1, QString q2)//выводит описание организации разрабатывающей проект
{
    QSqlQuery query(db);
    int i=0;
    QByteArray data;
    query.prepare("SELECT OID FROM Projectes WHERE ID =:id");
    query.bindValue(":id", id);
    query.exec();
    query.next();
    i=query.value(0).toInt();
    query.prepare("SELECT name FROM Organization"+q+" WHERE ID =:id");
    query.bindValue(":id", i);
    query.exec();
    query.next();
    ui->textBrowser->setText(q1+query.value(0).toString());
    ui->textBrowser->append("");
    query.prepare("SELECT desc FROM Organization"+q+" WHERE ID =:id");
    query.bindValue(":id", i);
    query.exec();
    query.next();
    ui->textBrowser->append(query.value(0).toString());
    ui->textBrowser->append("");
    query.prepare("SELECT contact FROM Organization"+q+" WHERE ID =:id");
    query.bindValue(":id", i);
    query.exec();
    query.next();
    ui->textBrowser->append(q2+query.value(0).toString());
    query.prepare("SELECT picture FROM Organizationes WHERE ID =:id");
    query.bindValue(":id", i);
    query.exec();
    query.next();
    data=query.value(0).toByteArray();
    addpic(data);
    }

void MainWindow::onep(QString q, QString q1) //выводит соответствующую одному параметру часть описания проекта
{
    QSqlQuery query(db);
    query.prepare("SELECT "+q1+" FROM Project"+q+" WHERE ID =:id");
    query.bindValue(":id", id);
    query.exec();
    query.next();
    ui->textBrowser->setText(query.value(0).toString());
    ppic();
    }

void MainWindow::econ(QString q, QString q1, QString q2, QString q3, QString q4) //выводит экономическую информацию о проекте
{
    QSqlQuery query(db);
    query.prepare("SELECT invest FROM Projectes WHERE ID =:id");
    query.bindValue(":id", id);
    query.exec();
    query.next();
    ui->textBrowser->setText(q1+query.value(0).toString());
    ui->textBrowser->append("");
    query.prepare("SELECT stage FROM Project"+q+" WHERE ID =:id");
    query.bindValue(":id", id);
    query.exec();
    query.next();
    ui->textBrowser->append(q2+query.value(0).toString());
    ui->textBrowser->append("");
    query.prepare("SELECT datedev FROM Projectes WHERE ID =:id");
    query.bindValue(":id", id);
    query.exec();
    query.next();
    QDate d=query.value(0).toDate();
    ui->textBrowser->append(q3+d.toString("dd.MM.yyyy"));
    ui->textBrowser->append("");
    query.prepare("SELECT dateinv FROM Projectes WHERE ID =:id");
    query.bindValue(":id", id);
    query.exec();
    query.next();
    d=query.value(0).toDate();
    ui->textBrowser->append(q4+d.toString("dd.MM.yyyy"));
    ppic();
    }

void MainWindow::ppic() //отображает изображение проекта
{
    QSqlQuery query(db);
    query.prepare("SELECT picture FROM Projectes WHERE ID=:1");
    query.bindValue(":1", id);
    query.exec();
    query.next();
    QByteArray data;
    data=query.value(0).toByteArray();
    addpic(data);
    }


void MainWindow::addpic(QByteArray q) //меняет изображение
{
    QPixmap picr;
    if (!q.isEmpty()){
        picr.loadFromData(q);
        picr.size()=ui->pic->size();
        picr=picr.scaled(ui->pic->size(), Qt::KeepAspectRatio);
        ui->pic->setPixmap(picr);
    }
    else ui->pic->clear();
}

void MainWindow::adsearch() //выводит и убирает поле для расширенного поиска
{
    if (ui->searchfield->isHidden()){
        ui->searchfield->setHidden(0);
    }
    else {
        ui->searchfield->setHidden(1);
        ui->orgpr->clear();
        ui->autpr->clear();
        ui->inv1->clear();
        ui->inv2->clear();
        ui->categor->clear();
        ui->stag->clear();
        ui->datedf->setDate(QDate(2000,1,1));
        ui->datedt->setDate(QDate(2000,1,1));
        ui->dateif->setDate(QDate(2000,1,1));
        ui->dateit->setDate(QDate(2000,1,1));
    }
}

void MainWindow::openSQL() // находит базу данных и открывает её
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("proj.db");
    db.open();
}

MainWindow::~MainWindow()
{
    db.close();
    delete layout;
    delete container;
    delete ui;
}
