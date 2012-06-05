/*
Copyright (C) 2012  Till Theato <root@ttill.de>
This file is part of kdenlive. See www.kdenlive.org.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
*/

#include "mainwindow.h"
#include "core.h"
#include "kdenlivesettings.h"
#include "project/project.h"
#include "bin/bin.h"
#include "monitor/monitorview.h"
#include "timelineview/timelinewidget.h"
#include <KLocale>
#include <QDockWidget>
#include <QGraphicsView>
#include <QLocale>
#include <locale.h>

#include <KDebug>


MainWindow::MainWindow(const QString &MltPath, const KUrl &Url, const QString & clipsToLoad, QWidget* parent) :
    KXmlGuiWindow(parent)
{
    initLocale();

    Core::initialize(this);

    QDockWidget *binDock = new QDockWidget(i18n("Bin"), this);
    binDock->setObjectName("bin");
    m_bin = new Bin();
    binDock->setWidget(m_bin);
    addDockWidget(Qt::TopDockWidgetArea, binDock);

    QDockWidget *monitorDock = new QDockWidget(i18n("Monitor"), this);
    monitorDock->setObjectName("monitor");
    m_monitor = new MonitorView();

    monitorDock->setWidget(m_monitor);
    addDockWidget(Qt::TopDockWidgetArea, monitorDock);

    m_timeline = new TimelineWidget(this);
    setCentralWidget(m_timeline);


    setupGUI();


    pCore->setCurrentProject(new Project(Url));
}

MainWindow::~MainWindow()
{
    delete m_bin;
    delete m_monitor;
    delete m_timeline;
}

void MainWindow::initLocale()
{
    QLocale systemLocale;
    setlocale(LC_NUMERIC, NULL);
    char *separator = localeconv()->decimal_point;
    if (separator != systemLocale.decimalPoint()) {
        kDebug()<<"------\n!!! system locale is not similar to Qt's locale... be prepared for bugs!!!\n------";
        // HACK: There is a locale conflict, so set locale to at least have correct decimal point
        if (strncmp(separator, ".", 1) == 0) {
            systemLocale = QLocale::c();
        } else if (strncmp(separator, ",", 1) == 0) {
            systemLocale = QLocale("fr_FR.UTF-8");
        }
    }

    systemLocale.setNumberOptions(QLocale::OmitGroupSeparator);
    QLocale::setDefault(systemLocale);
}

Bin* MainWindow::bin()
{
    return m_bin;
}

MonitorView* MainWindow::monitorWidget()
{
    return m_monitor;
}

TimelineWidget* MainWindow::timelineWidget()
{
    return m_timeline;
}

void MainWindow::loadDocks()
{

}

#include "mainwindow.moc"
