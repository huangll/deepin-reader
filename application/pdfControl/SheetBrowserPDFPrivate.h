#ifndef FILEVIEWWIDGETPRIVATE_H
#define FILEVIEWWIDGETPRIVATE_H

#include <QObject>
#include <QMouseEvent>
#include <QPointer>

#include "application.h"
#include "MsgHeader.h"
#include "pdfControl/docview/pagebase.h"
#include "ModuleHeader.h"
#include "widgets/FindWidget.h"

class Annotation;
class DocummentProxy;
class ProxyViewDisplay;
class ProxyNotifyMsg;
class ProxyMouseMove;
class ProxyFileDataModel;
class TipsWidget;
class NoteViewWidget;
class TextOperationMenu;
class DefaultOperationMenu;
class ProxyData;
class SheetBrowserPDF;
class DocSheet;
class SheetBrowserPDFPrivate: public QObject
{
    Q_OBJECT

public:
    SheetBrowserPDFPrivate(DocSheet *sheet, SheetBrowserPDF *parent);
    ~SheetBrowserPDFPrivate() override;

public:
    void hidetipwidget();

    bool hasOpened();

private slots:
    void slotDealWithMenu(const int &, const QString &);

    void SlotNoteViewMsg(const int &, const QString &);

    void onAddHighLightAnnotation(const QString &msgContent);

    void SlotDeleteAnntation(const int &, const QString &);

    void onFileOpenResult(bool);

    void slotCustomContextMenuRequested(const QPoint &point);

    void onPageBookMarkButtonClicked(int, bool);

    void onPageChanged(int page);

    void setFindWidget(FindWidget *findWidget);

private:
    void wheelEvent(QWheelEvent *event);

private:
    void mouseMoveEvent(QMouseEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void AddHighLightAnnotation(const QString &msgContent);

private:
    void DocFile_ctrl_l();
    void DocFile_ctrl_i();
    void DocFile_ctrl_c();
    double handleResize(const QSize &size);

private:
    void initConnections();
private:
    void showNoteViewWidget(const QString &, const QString &t_strUUid = "", const QString &sText = "", const int &nType = NOTE_HIGHLIGHT);
    void onOpenNoteWidget(const QString &msgContent);
    void onShowNoteWidget(const QString &contant);
    void __ShowNoteTipWidget(const QString &sText);
    void __CloseFileNoteWidget();
    void __SetCursor(const QCursor &cursor);
    void OpenFilePath(const QString &sPath);
private:
    bool m_hasOpened = false;
    NoteViewWidget          *m_pNoteViewWidget = nullptr;   //  注释内容显示框
    TipsWidget           *m_pTipWidget = nullptr;        //  注释提示框
    TextOperationMenu       *m_operatemenu = nullptr;
    DefaultOperationMenu    *m_pDefaultMenu = nullptr;
    ProxyData               *m_pProxyData = nullptr;            //  唯一 数据区
    ProxyFileDataModel      *m_pProxyFileDataModel = nullptr;
    ProxyMouseMove          *m_pProxyMouseMove = nullptr;       //  移动操作
    ProxyNotifyMsg          *m_pProxyNotifyMsg = nullptr;       //  通知消息
    ProxyViewDisplay        *m_pDocViewProxy = nullptr;         //  文档操作 旋转\单双页\自适应
    Annotation              *m_pAnnotation = nullptr;           //  高亮 注释操作
    DocummentProxy          *m_pProxy = nullptr;                //  文档代理类
    QPoint                  m_popwidgetshowpoint;
    DocSheet                *m_sheet;
    QPointer<FindWidget>    m_findWidget = nullptr;
    friend class Annotation;
    friend class ProxyViewDisplay;
    friend class ProxyMouseMove;
    friend class ProxyNotifyMsg;
    friend class ProxyFileDataModel;

private:
    SheetBrowserPDF *const q_ptr = nullptr;
    Q_DECLARE_PUBLIC(SheetBrowserPDF)
};

#endif // FILEVIEWWIDGETPRIVATE_H
