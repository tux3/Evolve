/********************************************************************************
** Form generated from reading UI file 'settingswidget.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSWIDGET_H
#define UI_SETTINGSWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsWidget
{
public:
    QGridLayout *gridLayout;
    QPushButton *closeButton;
    QSpinBox *colorVarSpin;
    QLabel *label_2;
    QSpacerItem *verticalSpacer;
    QPushButton *okButton;
    QSpacerItem *horizontalSpacer;
    QSpinBox *nPointsSpin;
    QLabel *label;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QSpinBox *focusRight;
    QLabel *label_5;
    QLabel *label_4;
    QLabel *label_3;
    QSpinBox *focusLeft;
    QLabel *label_6;
    QSpinBox *focusTop;
    QLabel *label_7;
    QSpinBox *focusBottom;

    void setupUi(QWidget *SettingsWidget)
    {
        if (SettingsWidget->objectName().isEmpty())
            SettingsWidget->setObjectName(QStringLiteral("SettingsWidget"));
        SettingsWidget->resize(400, 300);
        gridLayout = new QGridLayout(SettingsWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        closeButton = new QPushButton(SettingsWidget);
        closeButton->setObjectName(QStringLiteral("closeButton"));

        gridLayout->addWidget(closeButton, 4, 2, 1, 1);

        colorVarSpin = new QSpinBox(SettingsWidget);
        colorVarSpin->setObjectName(QStringLiteral("colorVarSpin"));
        colorVarSpin->setMinimum(1);
        colorVarSpin->setMaximum(255);
        colorVarSpin->setValue(5);

        gridLayout->addWidget(colorVarSpin, 1, 2, 1, 1);

        label_2 = new QLabel(SettingsWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 3, 0, 1, 3);

        okButton = new QPushButton(SettingsWidget);
        okButton->setObjectName(QStringLiteral("okButton"));

        gridLayout->addWidget(okButton, 4, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 4, 0, 1, 1);

        nPointsSpin = new QSpinBox(SettingsWidget);
        nPointsSpin->setObjectName(QStringLiteral("nPointsSpin"));
        nPointsSpin->setMinimum(3);
        nPointsSpin->setMaximum(100);

        gridLayout->addWidget(nPointsSpin, 0, 2, 1, 1);

        label = new QLabel(SettingsWidget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 2);

        groupBox = new QGroupBox(SettingsWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        focusRight = new QSpinBox(groupBox);
        focusRight->setObjectName(QStringLiteral("focusRight"));
        focusRight->setMaximum(100);
        focusRight->setSingleStep(5);
        focusRight->setValue(100);

        gridLayout_2->addWidget(focusRight, 1, 3, 1, 1);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout_2->addWidget(label_5, 1, 2, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_2->addWidget(label_4, 1, 0, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_2->addWidget(label_3, 0, 0, 1, 4);

        focusLeft = new QSpinBox(groupBox);
        focusLeft->setObjectName(QStringLiteral("focusLeft"));
        focusLeft->setMaximum(100);
        focusLeft->setSingleStep(5);

        gridLayout_2->addWidget(focusLeft, 1, 1, 1, 1);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout_2->addWidget(label_6, 2, 0, 1, 1);

        focusTop = new QSpinBox(groupBox);
        focusTop->setObjectName(QStringLiteral("focusTop"));
        focusTop->setMaximum(100);
        focusTop->setSingleStep(5);

        gridLayout_2->addWidget(focusTop, 2, 1, 1, 1);

        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout_2->addWidget(label_7, 2, 2, 1, 1);

        focusBottom = new QSpinBox(groupBox);
        focusBottom->setObjectName(QStringLiteral("focusBottom"));
        focusBottom->setMaximum(100);
        focusBottom->setSingleStep(5);
        focusBottom->setValue(100);

        gridLayout_2->addWidget(focusBottom, 2, 3, 1, 1);


        gridLayout->addWidget(groupBox, 2, 0, 1, 3);


        retranslateUi(SettingsWidget);

        QMetaObject::connectSlotsByName(SettingsWidget);
    } // setupUi

    void retranslateUi(QWidget *SettingsWidget)
    {
        SettingsWidget->setWindowTitle(QApplication::translate("SettingsWidget", "Settings", 0));
        closeButton->setText(QApplication::translate("SettingsWidget", "Cancel", 0));
        label_2->setText(QApplication::translate("SettingsWidget", "Color variation (lower is better, but slower)", 0));
        okButton->setText(QApplication::translate("SettingsWidget", "Ok", 0));
        label->setText(QApplication::translate("SettingsWidget", "Number of points per polygon", 0));
        groupBox->setTitle(QApplication::translate("SettingsWidget", "Focus", 0));
        focusRight->setSuffix(QApplication::translate("SettingsWidget", "%", 0));
        label_5->setText(QApplication::translate("SettingsWidget", "Right", 0));
        label_4->setText(QApplication::translate("SettingsWidget", "Left", 0));
        label_3->setText(QApplication::translate("SettingsWidget", "Focus on a part of the image only.\n"
"The program will only try to improve this part of the image.", 0));
        focusLeft->setSuffix(QApplication::translate("SettingsWidget", "%", 0));
        label_6->setText(QApplication::translate("SettingsWidget", "Top", 0));
        focusTop->setSuffix(QApplication::translate("SettingsWidget", "%", 0));
        label_7->setText(QApplication::translate("SettingsWidget", "Bottom", 0));
        focusBottom->setSuffix(QApplication::translate("SettingsWidget", "%", 0));
    } // retranslateUi

};

namespace Ui {
    class SettingsWidget: public Ui_SettingsWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSWIDGET_H
