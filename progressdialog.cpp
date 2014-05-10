#include "progressdialog.h"
#include "ui_progressdialog.h"

ProgressDialog::ProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::setMax(int max)
{
    ui->progressBar->setMaximum(max);
}

void ProgressDialog::increment()
{
    ui->progressBar->setValue(ui->progressBar->value()+1);
}

void ProgressDialog::setProgress(int progress)
{
    ui->progressBar->setValue(progress);
}
