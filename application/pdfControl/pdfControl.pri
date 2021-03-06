PKGCONFIG += x11 poppler-qt5 ddjvuapi libspectre
INCLUDEPATH+=/usr/include/KF5/KArchive
LIBS+=-lKF5Archive -luuid -ltiff

HEADERS += \
    $$PWD/note/notesdelegate.h \
    $$PWD/search/searchresdelegate.h \
    $$PWD/thumbnail/ThumbnailDelegate.h \
    $$PWD/bookmark/bookmarkdelegate.h \
    $$PWD/imagelistview.h \
    $$PWD/imageviewmodel.h \
    $$PWD/note/NotesWidget.h \
    $$PWD/catalog/CatalogTreeView.h \
    $$PWD/catalog/CatalogWidget.h \
    $$PWD/threadmanager/readerimagethreadpoolmanager.h \
    $$PWD/thumbnail/PagingWidget.h \
    $$PWD/thumbnail/ThumbnailWidget.h \
    $$PWD/search/SearchResWidget.h \
    $$PWD/bookmark/BookMarkWidget.h \
    $$PWD/note/NoteViewWidget.h \
    $$PWD/note/TransparentTextEdit.h \
    $$PWD/DataStackedWidget.h \
    $$PWD/menu/BookMarkMenu.h \
    $$PWD/menu/ColorWidgetAction.h \
    $$PWD/menu/DefaultOperationMenu.h \
    $$PWD/menu/NoteMenu.h \
    $$PWD/menu/TextOperationMenu.h \
    $$PWD/controller/Annotation.h \
    $$PWD/controller/ProxyData.h \
    $$PWD/controller/ProxyFileDataModel.h \
    $$PWD/controller/ProxyMouseMove.h \
    $$PWD/controller/ProxyNotifyMsg.h \
    $$PWD/controller/ProxyViewDisplay.h \
    $$PWD/SheetSidebarPDF.h \
    $$PWD/SheetBrowserPDF.h \
    $$PWD/SheetBrowserPDFPrivate.h \
    $$PWD/DocSheetPDF.h \
    $$PWD/docview/commonstruct.h \
    $$PWD/docview/docummentbase.h \
    $$PWD/docview/docummentfactory.h \
    $$PWD/docview/docummentproxy.h \
    $$PWD/docview/generatorclass.h \
    $$PWD/docview/pagebase.h \
    $$PWD/docview/publicfunc.h \
    $$PWD/docview/searchtask.h \
    $$PWD/docview/pdf/pagepdf.h \
    $$PWD/docview/pdf/docummentpdf.h

SOURCES += \
    $$PWD/note/notesdelegate.cpp \
    $$PWD/search/searchresdelegate.cpp \
    $$PWD/thumbnail/ThumbnailDelegate.cpp \
    $$PWD/bookmark/bookmarkdelegate.cpp \
    $$PWD/imagelistview.cpp \
    $$PWD/imageviewmodel.cpp \
    $$PWD/note/NotesWidget.cpp \
    $$PWD/catalog/CatalogTreeView.cpp \
    $$PWD/catalog/CatalogWidget.cpp \
    $$PWD/threadmanager/readerimagethreadpoolmanager.cpp \
    $$PWD/thumbnail/PagingWidget.cpp \
    $$PWD/thumbnail/ThumbnailWidget.cpp \
    $$PWD/search/SearchResWidget.cpp \
    $$PWD/bookmark/BookMarkWidget.cpp \
    $$PWD/note/NoteViewWidget.cpp \
    $$PWD/note/TransparentTextEdit.cpp \
    $$PWD/DataStackedWidget.cpp \
    $$PWD/menu/BookMarkMenu.cpp \
    $$PWD/menu/ColorWidgetAction.cpp \
    $$PWD/menu/DefaultOperationMenu.cpp \
    $$PWD/menu/NoteMenu.cpp \
    $$PWD/menu/TextOperationMenu.cpp \
    $$PWD/controller/Annotation.cpp \
    $$PWD/controller/ProxyData.cpp \
    $$PWD/controller/ProxyFileDataModel.cpp \
    $$PWD/controller/ProxyMouseMove.cpp \
    $$PWD/controller/ProxyNotifyMsg.cpp \
    $$PWD/controller/ProxyViewDisplay.cpp \
    $$PWD/SheetSidebarPDF.cpp \
    $$PWD/SheetBrowserPDF.cpp \
    $$PWD/SheetBrowserPDFPrivate.cpp \
    $$PWD/DocSheetPDF.cpp \
    $$PWD/docview/docummentbase.cpp \
    $$PWD/docview/docummentfactory.cpp \
    $$PWD/docview/docummentproxy.cpp \
    $$PWD/docview/generatorclass.cpp \
    $$PWD/docview/pagebase.cpp \
    $$PWD/docview/publicfunc.cpp \
    $$PWD/docview/searchtask.cpp \
    $$PWD/docview/pdf/pagepdf.cpp \
    $$PWD/docview/pdf/docummentpdf.cpp
