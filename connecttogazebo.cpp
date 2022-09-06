#include "connecttogazebo.h"
#include "ui_connecttogazebo.h"

ConnectToGazebo::ConnectToGazebo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectToGazebo)
{
    ui->setupUi(this);

   // ui->OKButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOkButton));
   // ui->CancelButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCancelButton));

}

ConnectToGazebo::~ConnectToGazebo()
{
    delete ui;
}

void ConnectToGazebo::on_OKButton_clicked()
{
    emit gazeboServer(ui->server->toPlainText(), ui->port->toPlainText());
    hide();
}

void ConnectToGazebo::on_CancelButton_clicked()
{
    hide();
}
