/***************************************************************************
                          kmmmonitor.cpp  -  description
                             -------------------
    begin                : Sun Mar 24 2002
    copyright            : (C) 2002 by Jason Wood
    email                : jasonwood@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kmmmonitor.h"

KMMMonitor::KMMMonitor(KdenliveApp *app, KdenliveDoc *document, QWidget *parent, const char *name ) :
										QVBox(parent,name),
										m_screen(app, this, name),
										m_editPanel(document, this, name)
{
	connect(&m_editPanel, SIGNAL(seekPositionChanged(GenTime)), &m_screen, SLOT(seek(GenTime)));
	connect(&m_editPanel, SIGNAL(seekPositionChanged(GenTime)), this, SIGNAL(seekPositionChanged(GenTime)));  
	connect(&m_editPanel, SIGNAL(playSpeedChanged(double)), &m_screen, SLOT(play(double)));
  connect(&m_screen, SIGNAL(rendererConnected()), &m_editPanel, SLOT(rendererConnected()));
  connect(&m_screen, SIGNAL(rendererDisconnected()), &m_editPanel, SLOT(rendererDisconnected()));
  connect(&m_screen, SIGNAL(seekPositionChanged(const GenTime &)), this, SLOT(screenPositionChanged(const GenTime &)));
}

KMMMonitor::~KMMMonitor(){
}

/** Sets the length of the clip held by
this montor. FIXME - this is a
temporary function, and will be changed in the future. */
void KMMMonitor::setClipLength(int frames)
{
	m_editPanel.setClipLength(frames);
}

/** Seek the monitor to the given time. */
void KMMMonitor::seek(GenTime time)
{
  m_editPanel.seek(time);
}

/** This slot is called when the screen changes position. */
void KMMMonitor::screenPositionChanged(const GenTime &time)
{
	disconnect(&m_editPanel, SIGNAL(seekPositionChanged(GenTime)), &m_screen, SLOT(seek(GenTime)));    
  m_editPanel.seek(time);
	connect(&m_editPanel, SIGNAL(seekPositionChanged(GenTime)), &m_screen, SLOT(seek(GenTime)));  
}
