#include "settingswidget.h"
#include "ui_settingswidget.h"
#include "settings.h"
#include "widget.h"
#include <QMessageBox>

bool SettingsWidget::isDefaultConfig = true;

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);
    connect(ui->okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(cancelClicked()));

    // Setup values
    ui->colorVarSpin->setValue(N_COLOR_VAR);
    ui->shapeVarSpin->setValue(N_POS_VAR);
    ui->nPointsSpin->setValue(N_POLY_POINTS);
    ui->focusLeft->setValue(FOCUS_LEFT);
    ui->focusRight->setValue(FOCUS_RIGHT);
    ui->focusTop->setValue(FOCUS_TOP);
    ui->focusBottom->setValue(FOCUS_BOTTOM);
    ui->shapeOptFreqSpin->setValue(SHAPE_OPT_FREQ);
    ui->guiRefreshSpin->setValue(GUI_REFRESH_RATE);
    ui->coreSpin->setValue(N_CORES);
    ui->minPolySpin->setValue(POLYS_MIN);
    ui->maxPolySpin->setValue(POLYS_MAX);
    ui->autofocusEnable->setChecked(AUTOFOCUS_ENABLED);
    ui->autofocusSubdivs->setValue(AUTOFOCUS_SUBDIVS);
    ui->autofocusDelay->setValue(AUTOFOCUS_DELAY);
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
    if (ui->focusLeft->value() >= ui->focusRight->value()
        || ui->focusTop->value() >= ui->focusBottom->value()
        || ui->minPolySpin->value() > ui->maxPolySpin->value())
    {
        QMessageBox::critical(this, "Error", "Invalid Focus settings");
        close();
        return;
    }

    isDefaultConfig = false;

    N_POLY_POINTS = ui->nPointsSpin->value();
    N_COLOR_VAR = ui->colorVarSpin->value();
    N_POS_VAR = ui->shapeVarSpin->value();
    FOCUS_LEFT = ui->focusLeft->value();
    FOCUS_RIGHT = ui->focusRight->value();
    FOCUS_TOP = ui->focusTop->value();
    FOCUS_BOTTOM = ui->focusBottom->value();
    SHAPE_OPT_FREQ = ui->shapeOptFreqSpin->value();
    GUI_REFRESH_RATE = ui->guiRefreshSpin->value();
    N_CORES = ui->coreSpin->value();
    POLYS_MIN = ui->minPolySpin->value();
    POLYS_MAX = ui->maxPolySpin->value();
    AUTOFOCUS_ENABLED = ui->autofocusEnable->isChecked();
    AUTOFOCUS_SUBDIVS = ui->autofocusSubdivs->value();
    AUTOFOCUS_DELAY = ui->autofocusDelay->value();
    Widget::setAutofocus(AUTOFOCUS_ENABLED);
    close();
}
