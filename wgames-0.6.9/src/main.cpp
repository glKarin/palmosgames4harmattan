#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QString>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

#include "config.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString localeName = QLocale::system().name();

    QTranslator translator;
    translator.load("wgames_" + localeName,
#ifndef _DBG
                    BASE_DIR
#else
										"."
#endif
                    "/translations/");
    a.installTranslator(&translator);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + localeName,
                 QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

#ifdef _HARMATTAN
		QApplication::setStyle("plastique"); //QApplication::setStyle("cleanlooks");
		//QApplication::setStyle("cde");
		//QApplication::setStyle("motif");
#endif

    MainWindow w;
#ifndef _HARMATTAN
    w.show();
#else
		w.showFullScreen();
#endif

    return a.exec();
}
