#ifndef DOCUMMENTBASE_H
#define DOCUMMENTBASE_H

#include "pdfControl/docview/commonstruct.h"
#include "pdfControl/docview/generatorclass.h"
#include "searchtask.h"
#include <DWidget>
#include <DScrollArea>
#include <DScrollBar>
#include <QList>
#include <QVBoxLayout>
#include <QColor>
#include <QVideoWidget>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QTimer>

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

#define     MAXPAGEHEIGHT       20000

class SearchTask;
class DocummentBasePrivate;

class DocummentBasePrivate: public QObject
{
    Q_OBJECT
public:
    DocummentBasePrivate(DocummentBase *parent): q_ptr(parent),
        m_pages(),
        m_widgets(),
        m_pagecountsearch()
    {
        m_widget = nullptr;
        m_vboxLayout = nullptr;
        m_magnifierwidget = nullptr;
        m_slidewidget = nullptr;
        pslidelabel = nullptr;
        pslideanimationlabel = nullptr;
        pblankwidget = nullptr;
        m_bModified = false;
        m_bslidemodel = false;
        m_slidepageno = -1;
        m_currentpageno = 0;
        m_scale = 1.0;
        m_maxzoomratio = 5.0;
        m_rotate = RotateType_0;
        donotneedreloaddoc = false;
        m_magnifierpage = -1;
        m_viewmode = ViewMode_SinglePage;
        m_lastmagnifierpagenum = -1;
        animationfirst = nullptr;
        animationsecond = nullptr;
        animationgroup = nullptr;
        m_bsearchfirst = true;
        m_findcurpage = -1;
        m_imagewidth = 0.1;
        m_imageheight = 0.1;
        m_cursearch = 1;
        bcloseing = false;
        m_searchTask = nullptr;
        bfindnext = true;
        m_bScanningcopy = false;
        showslidwaittimer = nullptr;
        qwfather = nullptr;
//        loadpagewaittimer = nullptr;
        m_fileinfo = new stFileInfo;
        bautoplayslide = false;
        autoplayslidtimer = nullptr;
        m_autoplayslidmsec = 0;
        m_bautoplayslidreset = false;
        m_label2pagenum.clear();
        m_pagenum2label.clear();
    }

    ~DocummentBasePrivate()
    {
        if (!animationfirst) {
            animationfirst->deleteLater();
            animationfirst = nullptr;
        }
        if (!animationsecond) {
            animationsecond->deleteLater();
            animationsecond = nullptr;
        }
        if (!animationgroup) {
            animationgroup->deleteLater();
            animationgroup = nullptr;
        }
        if (m_searchTask) {
            delete m_searchTask;
            m_searchTask = nullptr;
        }
        if (m_magnifierwidget) {
            m_magnifierwidget->deleteLater();
            m_magnifierwidget = nullptr;

        }
        if (m_slidewidget) {
            m_slidewidget->deleteLater();
            m_slidewidget = nullptr;
        }
        if (nullptr != m_fileinfo) {
            delete m_fileinfo;
            m_fileinfo = nullptr;
        }
    }

    QVector<PageBase *> m_pages;
    QList<DWidget *>m_widgets;
    QList<QRect>m_widgetrects;
    DWidget *m_widget;
    QVBoxLayout *m_vboxLayout;
    MagnifierWidget *m_magnifierwidget;
    SlidWidget *m_slidewidget;
    DLabel *pslideanimationlabel;
    DLabel *pslidelabel;
    DLabel *pblankwidget;
    ViewMode_EM m_viewmode;
    int m_lastmagnifierpagenum;
    int m_magnifierpage;
    int m_slidepageno;
    int m_currentpageno;
    double m_scale;
    double m_maxzoomratio;//最大放大比例
    mutable bool m_bModified;
    bool m_bslidemodel;
    //    ThreadLoadWords m_threadloadwords;
    RotateType_EM m_rotate;
    bool donotneedreloaddoc;
    //    bool m_wordsbload;
    QPoint m_magnifierpoint;
    QPropertyAnimation *animationfirst;
    QPropertyAnimation *animationsecond;
    QParallelAnimationGroup *animationgroup;
    SearchTask *m_searchTask;
    int m_findcurpage;
    QMap<int, int> m_pagecountsearch; //搜索结果页对应当前页个数
    int m_cursearch;
    bool m_bsearchfirst;
    double m_imagewidth;
    double m_imageheight;
    bool bcloseing;
    bool bfindnext;//上一次搜索结果是向前翻还是向后翻
    bool m_bScanningcopy;//当前打开的是否为扫描件
    QTimer *showslidwaittimer;
    DWidget *qwfather;
//    QTimer *loadpagewaittimer;
    stFileInfo *m_fileinfo;
//    ThreadLoadDocumment threadloaddoc;
    ThreadLoadData threadloaddata;
    bool bautoplayslide;
    QTimer *autoplayslidtimer;
    int m_autoplayslidmsec;
    bool m_bautoplayslidreset;//播放到最后一页重置状态从头可以从头开始播放
    QMap<QString, int> m_label2pagenum;
    QMap<int, QString> m_pagenum2label;


signals:
    void signal_docummentLoaded(bool);
protected slots:
    virtual void loadDocumment(QString filepath) {}
protected:
    DocummentBase *q_ptr;
    Q_DECLARE_PUBLIC(DocummentBase)
};


class DocummentBase: public DScrollArea
{
    Q_OBJECT
public:
    DocummentBase(DocummentBasePrivate *ptr = nullptr, DWidget *parent = nullptr);
    ~DocummentBase();
    virtual bool loadDocumment(QString filepath) = 0;
    virtual bool bDocummentExist() = 0;
    virtual bool getImage(int pagenum, QImage &image, double width, double height);
    virtual bool getImage(int pagenum, QImage &image, double width);        //等比例获取 最高宽高比不低于0.5
    virtual bool getImageMax(int pagenum, QImage &image, double max);       //等比例获取 最长边不超过max
    virtual bool save(const QString &);
    virtual bool saveas(const QString &, bool)
    {
        return false;
    }
    virtual QString removeAnnotation(const QPoint &, AnnoteType_Em)
    {
        return QString();
    }
    virtual void removeAnnotation(const QString &struuid, int ipage = -1) {}
    virtual QString addAnnotation(const QPoint &, const QPoint &, QColor color = Qt::yellow)
    {
        return QString();
    }
    virtual void search(const QString &strtext, QColor color = Qt::yellow) {}
    virtual void getAllAnnotation(QList<stHighlightContent> &listres) {}
    virtual void clearSearch() {}
    virtual void docBasicInfo(stFileInfo &info);
    virtual void title(QString &title) {}
    virtual void setAnnotationText(int ipage, const QString &struuid, const QString &strtext) {}
    virtual void getAnnotationText(const QString &struuid, QString &strtext, int ipage) {}
    virtual bool annotationClicked(const QPoint &pos, QString &strtext, QString &struuid)
    {
        return false;
    }
    virtual bool freshFile(QString file)
    {
        return false;
    }
    virtual void jumpToHighLight(const QString &uuid, int ipage) {}
    virtual void changeAnnotationColor(int, const QString, const QColor &) {}
    virtual Outline outline()
    {
        return Outline();
    }
    virtual QString addTextAnnotation(const QPoint &, const QColor &color = Qt::yellow, TextAnnoteType_Em type = TextAnnoteType_Note)
    {
        return  QString();
    }
    virtual bool iconAnnotationClicked(const QPoint &, QString &, QString &)
    {
        return false;
    };
    void stopLoadPageThread();
    void waitThreadquit();
    bool openFile(QString filepath, unsigned int ipage = 0, RotateType_EM rotatetype = RotateType_0, double scale = 1.0, ViewMode_EM viewmode = ViewMode_SinglePage);
    bool setSelectTextStyle(QColor paintercolor = QColor(72, 118, 255, 100), QColor pencolor = QColor(72, 118, 255, 0), int penwidth = 0);
    void mouseSelectTextClear();
    bool mouseBeOverText(QPoint point);
    QPoint global2RelativePoint(QPoint globalpoint);
    bool showMagnifier(QPoint point);
    bool showMagnifierTest(QPoint point);
    int getCurrentPageNo();
    int currentPageNo();
    Page::Link *mouseBeOverLink(QPoint point);
    bool getSelectTextString(QString &st);
    bool getSelectTextString(QString &st, int &page);
    bool loadPages();
    double adaptWidthAndShow(double width);
    double adaptHeightAndShow(double height);
    void findNext();
    void findPrev();
    bool loadData();
    bool loadDoc(QString);
    int getPageSNum();
    bool exitSlideModel();
    QVector<PageBase *> *getPages();
    PageBase *getPage(int index);
    void magnifierClear();
    void pageMove(double mvx, double mvy);
    bool setMagnifierStyle(int magnifierradius = 100, int magnifierringwidth = 10, double magnifierscale = 3);
    bool showSlideModel();
    void cacularValueXY(int &xvalue, int &yvalue, int curpage, bool bsearch = true, QRectF rect = QRectF());
    int pointInWhichPage(QPoint &qpoint);
    bool setBookMarkState(int page, bool state);
    bool mouseSelectText(QPoint start, QPoint stop);
    void selectAllText();
    void setAutoPlaySlide(bool autoplay, int timemsec);
    bool getAutoPlaySlideStatu();
    void setViewFocus();
    double getMaxZoomratio();
    void jumpToOutline(const qreal  &realleft, const qreal &realtop, int ipage);
    QString pagenum2label(int index);
    int label2pagenum(QString label);
    bool haslabel();
    QPoint transformPoint(const QPoint &pt, RotateType_EM type, double scale);
    void setScaleRotateViewModeAndShow(double scale, RotateType_EM rotate, ViewMode_EM viewmode);

signals:
    void signal_pageChange(int);
    void signal_searchRes(stSearchRes);
    void signal_searchover();
    void signal_loadDocumment(QString);
    void sigPageBookMarkButtonClicked(int page, bool state);
    void signal_openResult(bool);
    void signal_autoplaytoend();

protected slots:
    void slot_vScrollBarValueChanged(int value);
    void slot_hScrollBarValueChanged(int value);
    void slot_MagnifierPixmapCacheLoaded(int pageno);
    void slot_searchValueAdd(stSearchRes res);
    void slot_searchover();
    void slot_docummentLoaded(bool result);
    void slot_dataLoaded(bool result);
    bool pageJump(int pagenum);
    void scaleAndShow(double scale, RotateType_EM rotate);
    bool setViewModeAndShow(ViewMode_EM viewmode);
    void showSlideModelTimerOut();
    void autoplayslidTimerOut();

protected:
    void showSinglePage();
    void showFacingPage();
    void initConnect();
    void wheelEvent(QWheelEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;

    DocummentBasePrivate *d_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), DocummentBase)
};


#endif // DOCUMMENTBASE_H
