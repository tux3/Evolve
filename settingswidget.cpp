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
    ui->focusLeft->setValue(FOCUS_LEFT);
    ui->focusRight->setValue(FOCUS_RIGHT);
    ui->focusTop->setValue(FOCUS_TOP);
    ui->focusBottom->setValue(FOCUS_BOTTOM);
    ui->coreSpin->setValue(N_CORES);
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
    FOCUS_LEFT = ui->focusLeft->value();
    FOCUS_RIGHT = ui->focusRight->value();
    FOCUS_TOP = ui->focusTop->value();
    FOCUS_BOTTOM = ui->focusBottom->value();
    N_CORES = ui->coreSpin->value();
    close();
}
