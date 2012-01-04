/***************************************************************************
 *   Copyright (C) 2011 by Jean-Baptiste Mardelle (jb@kdenlive.org)        *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/


#ifndef RESOURCEWIDGET_H
#define RESOURCEWIDGET_H


#include "ui_freesound_ui.h"
#include "abstractservice.h"

#include <QDialog>
#include <QProcess>
#include <kio/jobclasses.h>


class ResourceWidget : public QDialog, public Ui::FreeSound_UI
{
    Q_OBJECT

public:
    ResourceWidget(const QString & folder, QWidget * parent = 0);
    ~ResourceWidget();


private slots:
    void slotStartSearch(int page = 0);
    void slotUpdateCurrentSound();
    void slotPlaySound();
    void slotForcePlaySound(bool play);
    void slotPreviewStatusChanged(QProcess::ProcessState state);
    void slotDisplayMetaInfo(QMap <QString, QString> metaInfo);
    void slotSaveItem(const QString originalUrl = QString());
    void slotOpenUrl(const QString &url);
    void slotChangeService();
    void slotOnline();
    void slotOffline();
    void slotNextPage();
    void slotPreviousPage();
    void slotGotMetaInfo(const QString info);
    void slotOpenLink(const QUrl &url);
    void slotLoadThumb(const QString url);
    /** @brief A file download is finished */
    void slotGotFile(KJob *job);

private:
    QString m_folder;
    AbstractService *m_currentService;
    void parseLicense(const QString &);
    OnlineItemInfo m_currentInfo;
   
signals:
    void addClip(KUrl, const QString &);
};


#endif

