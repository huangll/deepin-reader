/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "MainTabWidgetEx.h"

#include <QVBoxLayout>
#include <QStackedLayout>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QUrl>

#include "MainTabBar.h"
#include "MainSplitter.h"
#include "MainWindow.h"
#include "MainTabWidgetExPrivate.h"

#include "business/AppInfo.h"
#include "business/SaveDialog.h"
#include "business/PrintManager.h"
#include "docview/docummentproxy.h"

#include "../FindWidget.h"
#include "../FileAttrWidget.h"
#include "../PlayControlWidget.h"
#include "../TitleWidget.h"
#include "docview/docummentproxy.h"

#include "gof/bridge/IHelper.h"

MainTabWidgetEx *MainTabWidgetEx::g_onlyApp = nullptr;

MainTabWidgetEx::MainTabWidgetEx(DWidget *parent)
    : CustomWidget(MAIN_TAB_WIDGET, parent)
{
    d_ptr = new MainTabWidgetExPrivate(this);
    m_pMsgList = {E_APP_MSG_TYPE, E_TABBAR_MSG_TYPE, MSG_FILE_IS_CHANGE};

    initWidget();
    InitConnections();
    g_onlyApp = this;

    dApp->m_pModelService->addObserver(this);
}

MainTabWidgetEx::~MainTabWidgetEx()
{
    dApp->m_pModelService->removeObserver(this);
}

MainTabWidgetEx *MainTabWidgetEx::Instance()
{
    return g_onlyApp;
}

int MainTabWidgetEx::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == E_TABBAR_MSG_TYPE) {
        OnTabBarMsg(msgContent);
    } else if (msgType == E_APP_MSG_TYPE) {     //  应用类消息
        OnAppMsgData(msgContent);
    } else if (msgType == MSG_FILE_IS_CHANGE) {
        OnTabFileChangeMsg(msgContent);
    } else {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(msgContent.toLocal8Bit().data(), &error);
        if (error.error == QJsonParseError::NoError) {
            QJsonObject obj = doc.object();
            QString sTo = obj.value("to").toString();

            if (sTo.contains(this->m_strObserverName)) {
                emit sigDealNotifyMsg(msgType, msgContent);
                return MSG_OK;
            }
        }
    }

    if (m_pMsgList.contains(msgType)) {
        return MSG_OK;
    }

    return MSG_NO_OK;
}

QStringList MainTabWidgetEx::qGetAllPath() const
{
    return d_ptr->GetAllPath();
}

QString MainTabWidgetEx::qGetCurPath() const
{
    return d_ptr->GetCurPath();
}

int MainTabWidgetEx::GetFileChange(const QString &sPath)
{
    return d_ptr->GetFileChange(sPath);
}

FileDataModel MainTabWidgetEx::qGetFileData(const QString &sPath) const
{
    return d_ptr->GetFileData(sPath);
}

void MainTabWidgetEx::SetFileData(const QString &sPath, const FileDataModel &fdm)
{
    d_ptr->SetFileData(sPath, fdm);
}

void MainTabWidgetEx::InsertPathProxy(const QString &sPath, DocummentProxy *proxy)
{
    d_ptr->InsertPathProxy(sPath, proxy);
}

DocummentProxy *MainTabWidgetEx::getCurFileAndProxy(const QString &sPath) const
{
    return d_ptr->getCurFileAndProxy(sPath);
}

void MainTabWidgetEx::showPlayControlWidget() const
{
    if (m_pctrlwidget) {
        int nScreenWidth = qApp->desktop()->geometry().width();
        int inScreenHeght = qApp->desktop()->geometry().height();
        m_pctrlwidget->activeshow((nScreenWidth - m_pctrlwidget->width()) / 2, inScreenHeght - 100);
    }
}

void MainTabWidgetEx::initWidget()
{
    m_pTabBar = new MainTabBar;
    connect(m_pTabBar, SIGNAL(sigTabBarIndexChange(const QString &)), SLOT(SlotSetCurrentIndexFile(const QString &)));
    connect(m_pTabBar, SIGNAL(sigAddTab(const QString &)), SLOT(SlotAddTab(const QString &)));
    connect(m_pTabBar, SIGNAL(sigCloseTab(const QString &)), SLOT(SlotCloseTab(const QString &)));

    connect(this, SIGNAL(sigRemoveFileTab(const QString &)), m_pTabBar, SLOT(SlotRemoveFileTab(const QString &)));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_pTabBar);

    m_pStackedLayout = new QStackedLayout;

    mainLayout->addItem(m_pStackedLayout);

    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    this->setLayout(mainLayout);
}

void MainTabWidgetEx::resizeEvent(QResizeEvent *event)
{
    auto findWidget = this->findChild<FindWidget *>();
    if (findWidget && findWidget->isVisible()) {
        int nParentWidth = this->width();
        findWidget->showPosition(nParentWidth);
    }
    CustomWidget::resizeEvent(event);
}

void MainTabWidgetEx::InitConnections()
{
}

void MainTabWidgetEx::onShowFileAttr()
{
    auto pFileAttrWidget = new FileAttrWidget;
    pFileAttrWidget->showScreenCenter();
}

void MainTabWidgetEx::OnAppMsgData(const QString &sText)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(sText.toLocal8Bit().data(), &error);
    if (error.error == QJsonParseError::NoError) {
        QJsonObject obj = doc.object();
        QString sType = obj.value("type").toString();
        if (sType == "exit_app") {
            OnAppExit();
        } else if (sType == "ShortCut") {
            QString sKey = obj.value("key").toString();
            OnAppShortCut(sKey);
        } else if (sType == "keyPress") {
            QString sKey = obj.value("key").toString();
            OnKeyPress(sKey);
        }
    }
}

//  应用退出, 删除所有文件
void MainTabWidgetEx::OnAppExit()
{
    QStringList saveFileList;
    QStringList noChangeFileList;

    auto splitterList = this->findChildren<MainSplitter *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        int iChange = s->qGetFileChange();
        if (iChange == 1) {
            saveFileList.append(sSplitterPath);
        } else {
            noChangeFileList.append(sSplitterPath);
        }
    }

    if (saveFileList.size() > 0) {
        SaveDialog sd;
        int nRes = sd.showDialog();
        if (nRes <= 0) {
            return;
        }

        int nMsgType = MSG_NOT_SAVE_FILE;
        if (nRes == 2) {     //  保存
            nMsgType = MSG_SAVE_FILE;
        }

        foreach (auto s, splitterList) {
            QString sSplitterPath = s->qGetPath();
            if (saveFileList.contains(sSplitterPath)) {
                SaveFile(nMsgType, sSplitterPath);
            }
        }
    }

    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        if (noChangeFileList.contains(sSplitterPath)) {
            SaveFile(MSG_NOT_CHANGE_SAVE_FILE, sSplitterPath);
        }
    }
    auto mapIt = d_ptr->getOpenFileAndProxy();
    for (auto it = mapIt.begin(); it != mapIt.end(); it++) {
        if (it.value() != nullptr)
            it.value()->closeFileAndWaitThreadClearEnd();
    }
    topLevelWidget()->hide();
    dApp->exit(0);
}

void MainTabWidgetEx::OnAppShortCut(const QString &s)
{
    auto children = this->findChildren<MainSplitter *>();
    if (children.size() == 0)
        return;

    if (s == KeyStr::g_ctrl_p) {
        OnPrintFile();
    } else if (s == KeyStr::g_ctrl_s) {
        OnSaveFile();
    } else if (s == KeyStr::g_alt_1 || s == KeyStr::g_alt_2 || s == KeyStr::g_ctrl_m  ||
               s == KeyStr::g_ctrl_1 || s == KeyStr::g_ctrl_2 || s == KeyStr::g_ctrl_3 ||
               s == KeyStr::g_ctrl_r || s == KeyStr::g_ctrl_shift_r ||
               s == KeyStr::g_ctrl_larger || s == KeyStr::g_ctrl_equal || s == KeyStr::g_ctrl_smaller) {
        TitleWidget::Instance()->qDealWithShortKey(s);
    } else if (s == KeyStr::g_ctrl_f) {     //  搜索
        ShowFindWidget();
    } else if (s == KeyStr::g_ctrl_h) {     //  开启幻灯片
        OnOpenSliderShow();
    } else if (s == KeyStr::g_ctrl_shift_s) {   //  另存为
        OnSaveAsFile();
    } else if (s == KeyStr::g_esc) {   //  esc 统一处理
        OnShortCutKey_Esc();
    } else {
        emit sigDealNotifyMsg(MSG_NOTIFY_KEY_MSG, s);
    }
}

void MainTabWidgetEx::OnTabBarMsg(const QString &s)
{
    if (s == "New window") {

    } else if (s == "New tab") {
        notifyMsg(E_OPEN_FILE);
    } else if (s == "Save") { //  保存当前显示文件
        OnSaveFile();
    } else if (s == "Save as") {
        OnSaveAsFile();
    } else if (s == "Print") {
        OnPrintFile();
    } else if (s == "Slide show") { //  开启幻灯片
        OnOpenSliderShow();
    } else if (s == "Magnifer") {   //  开启放大镜
        OnOpenMagnifer();
    } else if (s == "Document info") {
        onShowFileAttr();
    } else if (s == "Display in file manager") {    //  文件浏览器 显示
        d_ptr->OpenCurFileFolder();
    }
}

void MainTabWidgetEx::OnTabFileChangeMsg(const QString &sVale)
{
    QString sCurPath = qGetCurPath();

    d_ptr->SetFileChange(sCurPath, sVale.toInt());
}

void MainTabWidgetEx::SaveFile(const int &iType, const QString &sPath)
{
    QString sRes = dApp->m_pHelper->qDealWithData(iType, sPath);
    if (sRes != "") {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(sRes.toLocal8Bit().data(), &error);
        if (error.error == QJsonParseError::NoError) {
            QJsonObject obj = doc.object();
            int nReturn = obj.value("return").toInt();
            if (nReturn == MSG_OK) {
                DWidget *w = m_pStackedLayout->currentWidget();
                if (w) {
                    auto pSplitter = qobject_cast<MainSplitter *>(w);
                    if (pSplitter) {
                        pSplitter->saveData();
                    }
                }
            }
        }
    }
}

//  保存当前显示文件
void MainTabWidgetEx::OnSaveFile()
{
    DWidget *w = m_pStackedLayout->currentWidget();
    if (w) {
        auto pSplitter = qobject_cast<MainSplitter *>(w);
        if (pSplitter) {
            int nChange = pSplitter->qGetFileChange();
            if (nChange == 1) {
                QString sPath = pSplitter->qGetPath();
                SaveFile(MSG_SAVE_FILE, sPath);
            }
        }
    }
}

void MainTabWidgetEx::OnSaveAsFile()
{
    QString sRes = dApp->m_pHelper->qDealWithData(MSG_SAVE_AS_FILE_PATH, "");
}

//  打印
void MainTabWidgetEx::OnPrintFile()
{
    QString sPath = qGetCurPath();
    if (sPath != "") {
        PrintManager p(sPath);
        p.showPrintDialog(this);
    }
}

void MainTabWidgetEx::OnShortCutKey_Esc()
{
    int nState = getCurrentState();
    if (nState == SLIDER_SHOW) {  //  当前是幻灯片模式
        OnExitSliderShow();
    } else if (nState == Magnifer_State) {
        OnExitMagnifer();
    }

    setCurrentState(Default_State);
}

void MainTabWidgetEx::OnKeyPress(const QString &sKey)
{
    int nState = getCurrentState();
    if (nState == SLIDER_SHOW && m_pctrlwidget) {
        if (sKey == KeyStr::g_space) {
            QString sPath = d_ptr->getSliderPath();
            auto helper = getCurFileAndProxy(sPath);
            if (helper) {
                if (helper->getAutoPlaySlideStatu()) {
                    helper->setAutoPlaySlide(false);
                } else  {
                    helper->setAutoPlaySlide(true);
                }
            }
        }

        m_pctrlwidget->PageChangeByKey(sKey);
    }
}


//  切换文档, 需要取消之前文档 放大镜模式
void MainTabWidgetEx::SlotSetCurrentIndexFile(const QString &sPath)
{
    int nState = getCurrentState();
    if (nState == Magnifer_State) {
        setCurrentState(Default_State);

        QString sMagniferPath = d_ptr->getMagniferPath();
        auto proxy = getCurFileAndProxy(sMagniferPath);
        if (proxy) {
            proxy->closeMagnifier();
        }
    }

    auto splitterList = this->findChildren<MainSplitter *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        if (sSplitterPath == sPath) {
            int iIndex = m_pStackedLayout->indexOf(s);
            m_pStackedLayout->setCurrentIndex(iIndex);

            notifyMsg(MSG_TAB_SHOW_FILE_CHANGE, sPath);

            break;
        }
    }
}

void MainTabWidgetEx::SlotAddTab(const QString &sPath)
{
    if (m_pStackedLayout) {
        MainSplitter *splitter = new MainSplitter(this);
        connect(this, SIGNAL(sigDealNotifyMsg(const int &, const QString &)), splitter, SLOT(SlotNotifyMsg(const int &, const QString &)));
        splitter->qSetPath(sPath);
        m_pStackedLayout->addWidget(splitter);
    }
}

//  删除单个文件
void MainTabWidgetEx::SlotCloseTab(const QString &sPath)
{
    auto splitterList = this->findChildren<MainSplitter *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        if (sSplitterPath == sPath) {
            QString sRes  = "";
            int iChange = s->qGetFileChange();
            if (iChange == 1) {
                SaveDialog sd;
                int nRes = sd.showDialog();
                if (nRes <= 0) {
                    return;
                }

                int nMsgType = MSG_NOT_SAVE_FILE;
                if (nRes == 2) {     //  保存
                    nMsgType = MSG_SAVE_FILE;
                }

                QString s = sPath + Constant::sQStringSep;
                sRes = dApp->m_pHelper->qDealWithData(nMsgType, s);
            } else {
                QString s = sPath + Constant::sQStringSep;
                sRes = dApp->m_pHelper->qDealWithData(MSG_NOT_CHANGE_SAVE_FILE, s);
            }

            if (sRes != "") {
                QJsonParseError error;
                QJsonDocument doc = QJsonDocument::fromJson(sRes.toLocal8Bit().data(), &error);
                if (error.error == QJsonParseError::NoError) {
                    QJsonObject obj = doc.object();
                    int nReturn = obj.value("return").toInt();
                    if (nReturn == MSG_OK) {
                        //  保存成功
                        s->saveData();

                        d_ptr->RemovePath(sPath);

                        emit sigRemoveFileTab(sPath);

                        m_pStackedLayout->removeWidget(s);

                        delete  s;
                        s = nullptr;
                    }
                }
            }
            break;
        }
    }
}

void MainTabWidgetEx::setCurrentState(const int &nCurrentState)
{
    d_ptr->setCurrentState(nCurrentState);
}

int MainTabWidgetEx::getCurrentState() const
{
    return d_ptr->getCurrentState();
}

//  搜索框
void MainTabWidgetEx::ShowFindWidget()
{
    int nState = getCurrentState();
    if (nState == SLIDER_SHOW)
        return;

    if (m_pFindWidget == nullptr) {
        m_pFindWidget = new FindWidget(this);
    }

    int nParentWidth = this->width();
    m_pFindWidget->showPosition(nParentWidth);
    m_pFindWidget->setSearchEditFocus();
}

//  开启 幻灯片
void MainTabWidgetEx::OnOpenSliderShow()
{
    int nState = getCurrentState();
    if (nState != SLIDER_SHOW) {
        setCurrentState(SLIDER_SHOW);

        m_pTabBar->setVisible(false);

        auto splitter = qobject_cast<MainSplitter *>(m_pStackedLayout->currentWidget());
        if (splitter) {
            splitter->OnOpenSliderShow();

            MainWindow::Instance()->SetSliderShowState(0);

            QString sPath = splitter->qGetPath();

            d_ptr->setSliderPath(sPath);

            auto _proxy = getCurFileAndProxy(sPath);
            _proxy->setAutoPlaySlide(true);
            _proxy->showSlideModel();

            if (m_pctrlwidget == nullptr) {
                m_pctrlwidget = new PlayControlWidget(this);
            }

            m_pctrlwidget->setSliderPath(sPath);
            int nScreenWidth = qApp->desktop()->geometry().width();
            int inScreenHeght = qApp->desktop()->geometry().height();
            m_pctrlwidget->activeshow((nScreenWidth - m_pctrlwidget->width()) / 2, inScreenHeght - 100);
        }
    }
}

//  退出幻灯片
void MainTabWidgetEx::OnExitSliderShow()
{
    setCurrentState(Default_State);
    MainWindow::Instance()->SetSliderShowState(1);
    m_pTabBar->setVisible(true);

    auto splitter = qobject_cast<MainSplitter *>(m_pStackedLayout->currentWidget());
    if (splitter) {
        splitter->OnExitSliderShow();

        QString sPath = d_ptr->getSliderPath();
        DocummentProxy *_proxy = getCurFileAndProxy(sPath);
        if (!_proxy) {
            return;
        }
        _proxy->exitSlideModel();

        delete m_pctrlwidget;
        m_pctrlwidget = nullptr;
    }

    d_ptr->setSliderPath("");
}

void MainTabWidgetEx::OnOpenMagnifer()
{
    int nState = getCurrentState();
    if (nState != Magnifer_State) {
        setCurrentState(Magnifer_State);
        d_ptr->setMagniferPath(qGetCurPath());
    }
}

//  取消放大镜
void MainTabWidgetEx::OnExitMagnifer()
{
    int nState = getCurrentState();
    if (nState == Magnifer_State) {
        setCurrentState(Default_State);

        auto splitter = qobject_cast<MainSplitter *>(m_pStackedLayout->currentWidget());
        if (splitter) {
            QString sPath = splitter->qGetPath();
            DocummentProxy *_proxy = getCurFileAndProxy(sPath);
            if (!_proxy) {
                return;
            }
            _proxy->closeMagnifier();
        }
    }
}