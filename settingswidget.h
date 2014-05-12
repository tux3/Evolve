#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = 0);
    ~SettingsWidget();

private slots:
    void cancelClicked();
    void okClicked();

public:
    static bool isDefaultConfig;

private:
    Ui::SettingsWidget *ui;
};

#endif // SETTINGSWIDGET_H
