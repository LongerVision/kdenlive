/***************************************************************************
 *   Copyright (C) 2021 by Jean-Baptiste Mardelle (jb@kdenlive.org)        *
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

#ifndef SPEECHDIALOG_H
#define SPEECHDIALOG_H

#include "ui_speechdialog_ui.h"
#include "timeline2/model/timelineitemmodel.hpp"
#include "definitions.h"

/**
 * @class SpeechDialog
 * @brief A dialog for editing markers and guides.
 * @author Jean-Baptiste Mardelle
 */

class SpeechDialog : public QDialog, public Ui::SpeechDialog_UI
{
    Q_OBJECT

public:
    explicit SpeechDialog(const std::shared_ptr<TimelineItemModel> &timeline, QPoint zone, bool activeTrackOnly = false, bool selectionOnly = false, QWidget *parent = nullptr);
    ~SpeechDialog() override;

private:
    QMetaObject::Connection m_availableConnection;
    QMetaObject::Connection m_modelsConnection;
    void parseVoskDictionaries();

private slots:
    void slotProcessSpeech(const std::shared_ptr<TimelineItemModel> &timeline, QPoint zone);
    void updateAvailability();
};

#endif
