/***************************************************************************
 *   Copyright (C) 2007 by Jean-Baptiste Mardelle (jb@kdenlive.org)        *
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


#include "changespeedcommand.h"
#include "customtrackview.h"

#include <KLocale>

ChangeSpeedCommand::ChangeSpeedCommand(CustomTrackView *view, ItemInfo info, double old_speed, double new_speed, const QString &clipId, QUndoCommand * parent) : QUndoCommand(parent), m_view(view), m_clipInfo(info), m_old_speed(old_speed), m_new_speed(new_speed), m_clipId(clipId)
{
    setText(i18n("Adjust clip length"));
}


// virtual
void ChangeSpeedCommand::undo()
{
    m_view->doChangeClipSpeed(m_clipInfo, m_old_speed, m_new_speed, m_clipId);
}
// virtual
void ChangeSpeedCommand::redo()
{
        m_view->doChangeClipSpeed(m_clipInfo, m_new_speed, m_old_speed, m_clipId);
}


