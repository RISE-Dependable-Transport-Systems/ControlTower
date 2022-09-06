#ifndef CONNECTTOGAZEBO_H
#define CONNECTTOGAZEBO_H

#include <QDialog>

namespace Ui {
class ConnectToGazebo;
}

class ConnectToGazebo : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectToGazebo(QWidget *parent = nullptr);
    ~ConnectToGazebo();


signals:
    void gazeboServer(QString ip, QString port);

private slots:
    void on_CancelButton_clicked();
    void on_OKButton_clicked();

private:
    Ui::ConnectToGazebo *ui;
};

#endif // CONNECTTOGAZEBO_H
