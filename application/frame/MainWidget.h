#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "subjectObserver/CustomWidget.h"
#include <DStackedWidget>

DWIDGET_USE_NAMESPACE

/**
 * @brief The MainWidget class
 * @brief   采用　栈式　显示窗口，　当前只显示某一特定窗口
 */


class MainWidget : public CustomWidget
{
    Q_OBJECT
public:
    MainWidget(CustomWidget *parent = nullptr);

private:
    void openFileOk();
    void openFileFail(const QString &);

private:
    DStackedWidget  *m_pStackedWidget = nullptr;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &);

    // CustomWidget interface
protected:
    void initWidget();
};

#endif // MAINSTACKWIDGET_H