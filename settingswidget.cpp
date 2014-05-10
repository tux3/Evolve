#include "settingswidget.h"
#include "ui_settingswidget.h"
#include "settings.h"

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);
    connect(ui->okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(cancelClicked()));

    // Setup values
    ui->colorVarSpin->setValue(N_COLOR_VAR);
    ui->nPointsSpin->setValue(N_POLY_POINTS);
    ui->minPolySpin->setValue(N_MIN_POLYS);
    ui->maxPolySpin->setValue(N_MAX_POLYS);
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}

void SettingsWidget::cancelClicked()
{
    close();
}

void SettingsWidget::okClicked()
{
    N_POLY_POINTS = ui->nPointsSpin->value();
    N_COLOR_VAR = ui->colorVarSpin->value();
    close();
}
