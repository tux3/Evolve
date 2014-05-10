/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QGridLayout *gridLayout;
    QLabel *imgBest;
    QLabel *imgOriginal;
    QPushButton *btnStart;
    QPushButton *btnOpen;
    QHBoxLayout *horizontalLayout;
    QLabel *label_5;
    QLabel *generationLabel;
    QLabel *label_4;
    QLabel *polysLabel;
    QLabel *label_2;
    QLabel *fitnessLabel;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_3;
    QLabel *label;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QStringLiteral("Widget"));
        Widget->resize(451, 295);
        gridLayout = new QGridLayout(Widget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, -1);
        imgBest = new QLabel(Widget);
        imgBest->setObjectName(QStringLiteral("imgBest"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(imgBest->sizePolicy().hasHeightForWidth());
        imgBest->setSizePolicy(sizePolicy);
        imgBest->setMinimumSize(QSize(200, 200));
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        palette.setBrush(QPalette::Active, QPalette::Window, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush);
        imgBest->setPalette(palette);
        imgBest->setAutoFillBackground(true);
        imgBest->setScaledContents(true);
        imgBest->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(imgBest, 3, 2, 1, 1);

        imgOriginal = new QLabel(Widget);
        imgOriginal->setObjectName(QStringLiteral("imgOriginal"));
        sizePolicy.setHeightForWidth(imgOriginal->sizePolicy().hasHeightForWidth());
        imgOriginal->setSizePolicy(sizePolicy);
        imgOriginal->setMinimumSize(QSize(200, 200));
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::Base, brush);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush);
        imgOriginal->setPalette(palette1);
        imgOriginal->setAutoFillBackground(true);
        imgOriginal->setScaledContents(true);
        imgOriginal->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(imgOriginal, 3, 1, 1, 1);

        btnStart = new QPushButton(Widget);
        btnStart->setObjectName(QStringLiteral("btnStart"));

        gridLayout->addWidget(btnStart, 4, 2, 1, 1);

        btnOpen = new QPushButton(Widget);
        btnOpen->setObjectName(QStringLiteral("btnOpen"));

        gridLayout->addWidget(btnOpen, 4, 1, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_5 = new QLabel(Widget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setMaximumSize(QSize(16777215, 15));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(label_5);

        generationLabel = new QLabel(Widget);
        generationLabel->setObjectName(QStringLiteral("generationLabel"));

        horizontalLayout->addWidget(generationLabel);

        label_4 = new QLabel(Widget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(label_4);

        polysLabel = new QLabel(Widget);
        polysLabel->setObjectName(QStringLiteral("polysLabel"));

        horizontalLayout->addWidget(polysLabel);

        label_2 = new QLabel(Widget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(label_2);

        fitnessLabel = new QLabel(Widget);
        fitnessLabel->setObjectName(QStringLiteral("fitnessLabel"));

        horizontalLayout->addWidget(fitnessLabel);


        gridLayout->addLayout(horizontalLayout, 5, 1, 1, 2);

        horizontalSpacer_2 = new QSpacerItem(10, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 0, 3, 6, 1);

        label_3 = new QLabel(Widget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setMaximumSize(QSize(16777215, 20));
        QFont font;
        font.setPointSize(11);
        font.setBold(false);
        font.setWeight(50);
        label_3->setFont(font);
        label_3->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_3, 2, 2, 1, 1);

        label = new QLabel(Widget);
        label->setObjectName(QStringLiteral("label"));
        label->setMinimumSize(QSize(0, 0));
        label->setMaximumSize(QSize(16777215, 20));
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 2, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(10, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 0, 6, 1);


        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "Evolution Vectorizer", 0));
        imgBest->setText(QString());
        imgOriginal->setText(QString());
        btnStart->setText(QApplication::translate("Widget", "Start", 0));
        btnOpen->setText(QApplication::translate("Widget", "Open Image", 0));
        label_5->setText(QApplication::translate("Widget", "Generation :", 0));
        generationLabel->setText(QApplication::translate("Widget", "0", 0));
        label_4->setText(QApplication::translate("Widget", "Polys :", 0));
        polysLabel->setText(QApplication::translate("Widget", "0", 0));
        label_2->setText(QApplication::translate("Widget", "Fitness :", 0));
        fitnessLabel->setText(QApplication::translate("Widget", "0", 0));
        label_3->setText(QApplication::translate("Widget", "Best", 0));
        label->setText(QApplication::translate("Widget", "Original", 0));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
