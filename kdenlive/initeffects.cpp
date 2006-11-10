/***************************************************************************
                          createslideshowclip.cpp  -  description
                             -------------------
    begin                :  Jul 2006
    copyright            : (C) 2006 by Jean-Baptiste Mardelle
    email                : jb@ader.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <qfile.h>
#include <qregexp.h>

#include <kdebug.h>
#include <kglobal.h>
#include <kstandarddirs.h>

#include "initeffects.h"
#include "krender.h"

initEffects::initEffects()
{
}

initEffects::~initEffects()
{
}

//static 
void initEffects::initializeEffects(EffectDescriptionList *effectList)
{
    // Build effects. Retrieve the list of MLT's available effects first.

    QString inigoPath = KStandardDirs::findExe("inigo");
    inigoPath = inigoPath.section('/', 0, -3) + "/share/mlt/modules";

    KGlobal::dirs()->addResourceType("mlt_data", "share/mlt/modules");
    KGlobal::dirs()->addResourceDir("mlt_data", "/usr/share/mlt/modules");
    KGlobal::dirs()->addResourceDir("mlt_data", "/usr/local/share/mlt/modules");
    KGlobal::dirs()->addResourceDir("mlt_data", inigoPath);

    QString datFile = locate("mlt_data", "filters.dat");
    
    QStringList filtersList;

    QFile file( datFile );
    if ( file.open( IO_ReadOnly ) ) {
        QTextStream stream( &file );
        QString line;
        while ( !stream.atEnd() ) {
            line = stream.readLine(); // line of text excluding '\n'
            filtersList<<line.section(QRegExp("\\s+"), 0, 0);
        }
        file.close();
    }


    /**

	This lists all effects that will be available in  Kdenlive.
	To add a new effect, do something like this:

	1) create effect entry
	
	EffectDesc *my_effect = new EffectDesc(i18n("display_name"), "mlt_name", "effect_type");
	
	where display_name is the name of the effect that will be displayed in Kdenlive
	mlt_name is the effect name in MLT, which will be used during creation of the playlist
	effect_type is either "audio" or "video", depending on whether the effect affects auido or video,  which allows kdenlive to sort effects according to their type.

	2) Add parameters.
	
	Each effect can have several parameters. The parameter types are:
	
	* fixed : There are no parameters to this effect
	* constant : a integer number that will remains constant during all effect duration.
		Constant effects have parameters: name is the MLT param name, description is the param name to be displayed in ui, max, min and default are self describing, factor is the number that will divide your param before passing it to MLT. For example if you need a parameter between 0.0 and 1.0, you give a min of 0, a max of 100 and a factor of 100.
	* double : The value of this parameter will change during the effect. User can create keyframes. NB: currently, there can only be one double parameter in your effect, and it must be inserted as the first parameter of the effect.
	* complex : this is a special case that was created for the MLT obscure effect
	* bool: a boolean value that will be represented ba a checkbox

    **/

    QXmlAttributes xmlAttr;
    EffectParamDescFactory effectDescParamFactory;

    if (filtersList.findIndex("greyscale") != -1) {
        // Grayscale
        EffectDesc *grey = new EffectDesc(i18n("Greyscale"), "greyscale", "video");
        xmlAttr.append("type", QString::null, QString::null, "fixed");
        grey->addParameter(effectDescParamFactory.createParameter(xmlAttr));
        effectList->append(grey);
    }

    if (filtersList.findIndex("invert") != -1) {
        // Invert
        EffectDesc *invert = new EffectDesc(i18n("Invert"), "invert", "video");
        xmlAttr.clear();
        xmlAttr.append("type", QString::null, QString::null, "fixed");
        invert->addParameter(effectDescParamFactory.createParameter(xmlAttr));
        effectList->append(invert);
    }


    if (filtersList.findIndex("sepia") != -1) {
        // Sepia
        EffectDesc *sepia = new EffectDesc(i18n("Sepia"), "sepia", "video");
        xmlAttr.clear();
        xmlAttr.append("type", QString::null, QString::null, "constant");
        xmlAttr.append("name", QString::null, QString::null, "u");
        xmlAttr.append("description", QString::null, QString::null, "Chrominance U");
        xmlAttr.append("max", QString::null, QString::null, "255");
        xmlAttr.append("min", QString::null, QString::null, "0");
        xmlAttr.append("default", QString::null, QString::null, "75");
	sepia->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "constant");
	xmlAttr.append("name", QString::null, QString::null, "v");
	xmlAttr.append("description", QString::null, QString::null, i18n("Chrominance V"));
	xmlAttr.append("max", QString::null, QString::null, "255");
	xmlAttr.append("min", QString::null, QString::null, "0");
	xmlAttr.append("default", QString::null, QString::null, "150");
	sepia->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	effectList->append(sepia);
    }

    if (filtersList.findIndex("charcoal") != -1) {
	// Charcoal
	EffectDesc *charcoal = new EffectDesc(i18n("Charcoal"), "charcoal", "video");
	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "constant");
	xmlAttr.append("name", QString::null, QString::null, "x_scatter");
	xmlAttr.append("description", QString::null, QString::null, i18n("Horizontal scatter"));
	xmlAttr.append("max", QString::null, QString::null, "10");
	xmlAttr.append("min", QString::null, QString::null, "0");
	xmlAttr.append("default", QString::null, QString::null, "2");
	charcoal->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "constant");
	xmlAttr.append("name", QString::null, QString::null, "y_scatter");
	xmlAttr.append("description", QString::null, QString::null, i18n("Vertical scatter"));
	xmlAttr.append("max", QString::null, QString::null, "10");
	xmlAttr.append("min", QString::null, QString::null, "0");
	xmlAttr.append("default", QString::null, QString::null, "2");
	charcoal->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "constant");
	xmlAttr.append("name", QString::null, QString::null, "scale");
	xmlAttr.append("description", QString::null, QString::null, i18n("Scale"));
	xmlAttr.append("max", QString::null, QString::null, "10");
	xmlAttr.append("min", QString::null, QString::null, "0");
	xmlAttr.append("default", QString::null, QString::null, "1");
	charcoal->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "constant");
	xmlAttr.append("name", QString::null, QString::null, "mix");
	xmlAttr.append("description", QString::null, QString::null, i18n("Mix"));
	xmlAttr.append("max", QString::null, QString::null, "10");
	xmlAttr.append("min", QString::null, QString::null, "0");
	xmlAttr.append("default", QString::null, QString::null, "0");
	charcoal->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "bool");
	xmlAttr.append("name", QString::null, QString::null, "invert");
	xmlAttr.append("description", QString::null, QString::null, i18n("Invert"));
	xmlAttr.append("default", QString::null, QString::null, "0");
	charcoal->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	effectList->append(charcoal);
    }

    if (filtersList.findIndex("brightness") != -1) {
	// Brightness
	EffectDesc *bright = new EffectDesc(i18n("Brightness"), "brightness", "video");
	xmlAttr.clear();

	xmlAttr.append("type", QString::null, QString::null, "double");
	xmlAttr.append("name", QString::null, QString::null, "Intensity");
	xmlAttr.append("max", QString::null, QString::null, "300");
	xmlAttr.append("min", QString::null, QString::null, "0");
	xmlAttr.append("default", QString::null, QString::null, "100");
	xmlAttr.append("factor", QString::null, QString::null, "100");
	bright->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	effectList->append(bright);
    }

    if (filtersList.findIndex("volume") != -1) {
        // Audio volume
	EffectDesc *volume = new EffectDesc(i18n("Volume"), "volume", "audio");
	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "double");
	xmlAttr.append("name", QString::null, QString::null, "gain");
	xmlAttr.append("starttag", QString::null, QString::null, "gain");
	xmlAttr.append("max", QString::null, QString::null, "300");
	xmlAttr.append("min", QString::null, QString::null, "0");
	xmlAttr.append("default", QString::null, QString::null, "100");
	xmlAttr.append("factor", QString::null, QString::null, "100");
	volume->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	effectList->append(volume);
    
	// Audio muting
	EffectDesc *mute = new EffectDesc(i18n("Mute"), "volume", "audio");
	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "constant");
	xmlAttr.append("name", QString::null, QString::null, "gain");
	xmlAttr.append("max", QString::null, QString::null, "0");
	xmlAttr.append("min", QString::null, QString::null, "0");
	xmlAttr.append("default", QString::null, QString::null, "0");
	mute->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	effectList->append(mute);
    }

    if (filtersList.findIndex("obscure") != -1) {
        // Region obscure
	EffectDesc *obscure = new EffectDesc(i18n("Obscure"), "obscure", "video");

	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "complex");
	xmlAttr.append("name", QString::null, QString::null, "X;Y;Width;Height;Averaging");
	xmlAttr.append("min", QString::null, QString::null, "0;0;0;0;3");
	xmlAttr.append("max", QString::null, QString::null, QString::number(KdenliveSettings::defaultwidth()) + ";" + QString::number(KdenliveSettings::defaultheight()) + ";1000;1000;100");
	xmlAttr.append("default", QString::null, QString::null, QString::number((int) KdenliveSettings::defaultwidth() / 2) + ";" + QString::number((int) KdenliveSettings::defaultheight() / 2) + ";100;100;20");
	obscure->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	effectList->append(obscure);
    }

    if (filtersList.findIndex("mirror") != -1) {
        // Mirror
	EffectDesc *mirror = new EffectDesc(i18n("Mirror"), "mirror", "video");
	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "list");
	xmlAttr.append("paramlist", QString::null, QString::null, "horizontal,vertical,diagonal,xdiagonal,flip,flop");
	xmlAttr.append("default", QString::null, QString::null, "horizontal");
	xmlAttr.append("name", QString::null, QString::null, "mirror");
	xmlAttr.append("description", QString::null, QString::null,
	i18n("Mirroring direction"));
	mirror->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "bool");
	xmlAttr.append("name", QString::null, QString::null, "reverse");
	xmlAttr.append("description", QString::null, QString::null, i18n("Invert"));
	xmlAttr.append("default", QString::null, QString::null, "0");
	mirror->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	xmlAttr.clear();
	effectList->append(mirror);
    }

    // Slowmotion
    /* disable until MLT gets patched
    EffectDesc *slowmo = new EffectDesc(i18n("Speed"), "framebuffer", "video");
    xmlAttr.clear();
    xmlAttr.append("type", QString::null, QString::null, "constant");
    xmlAttr.append("name", QString::null, QString::null, "_speed");
    xmlAttr.append("description", QString::null, QString::null,
	i18n("Start Speed"));
    xmlAttr.append("max", QString::null, QString::null, "300");
    xmlAttr.append("min", QString::null, QString::null, "1");
    xmlAttr.append("default", QString::null, QString::null, "100");
    xmlAttr.append("factor", QString::null, QString::null, "100");
    slowmo->addParameter(effectDescParamFactory.createParameter(xmlAttr));*/
    /*xmlAttr.clear();  // MLT is not ready yet for variable speed
    xmlAttr.append("type", QString::null, QString::null, "constant");
    xmlAttr.append("name", QString::null, QString::null, "_speed_end");
    xmlAttr.append("description", QString::null, QString::null,
	i18n("End Speed"));
    xmlAttr.append("max", QString::null, QString::null, "300");
    xmlAttr.append("min", QString::null, QString::null, "1");
    xmlAttr.append("default", QString::null, QString::null, "100");
    xmlAttr.append("factor", QString::null, QString::null, "100");
    slowmo->addParameter(effectDescParamFactory.createParameter(xmlAttr));*/
    /*xmlAttr.clear();
    xmlAttr.append("type", QString::null, QString::null, "constant");
    xmlAttr.append("name", QString::null, QString::null, "strobe");
    xmlAttr.append("description", QString::null, QString::null,
	i18n("Stroboscope"));
    xmlAttr.append("max", QString::null, QString::null, "100");
    xmlAttr.append("min", QString::null, QString::null, "0");
    xmlAttr.append("default", QString::null, QString::null, "0");
    slowmo->addParameter(effectDescParamFactory.createParameter(xmlAttr));
    effectList->append(slowmo);*/


    if (filtersList.findIndex("gamma") != -1) {
	// Gamma
	EffectDesc *gamma = new EffectDesc(i18n("Gamma"), "gamma", "video");
	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "constant");
	xmlAttr.append("name", QString::null, QString::null, "gamma");
	xmlAttr.append("description", QString::null, QString::null, i18n("Gamma"));
	xmlAttr.append("max", QString::null, QString::null, "300");
	xmlAttr.append("min", QString::null, QString::null, "1");
	xmlAttr.append("default", QString::null, QString::null, "100");
	xmlAttr.append("factor", QString::null, QString::null, "100");
	gamma->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	effectList->append(gamma);
    }

    if (filtersList.findIndex("ladspa") != -1) {
	// Pitch shifter
	EffectDesc *pitch = new EffectDesc(i18n("Pitch Shift"), "ladspa1433", "audio");
	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "constant");
	xmlAttr.append("name", QString::null, QString::null, "pitch");
	xmlAttr.append("description", QString::null, QString::null, i18n("Shift"));
	xmlAttr.append("max", QString::null, QString::null, "400");
	xmlAttr.append("min", QString::null, QString::null, "25");
	xmlAttr.append("default", QString::null, QString::null, "100");
	xmlAttr.append("factor", QString::null, QString::null, "100");
	pitch->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	effectList->append(pitch);

	// Reverb
	EffectDesc *reverb = new EffectDesc(i18n("Room reverb"), "ladspa1216", "audio");
	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "constant");
	xmlAttr.append("name", QString::null, QString::null, "room");
	xmlAttr.append("description", QString::null, QString::null, i18n("Room size (m)"));
	xmlAttr.append("max", QString::null, QString::null, "300");
	xmlAttr.append("min", QString::null, QString::null, "1");
	xmlAttr.append("default", QString::null, QString::null, "75");
	reverb->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "constant");
	xmlAttr.append("name", QString::null, QString::null, "delay");
	xmlAttr.append("description", QString::null, QString::null, i18n("Delay (s/10)"));
	xmlAttr.append("max", QString::null, QString::null, "300");
	xmlAttr.append("min", QString::null, QString::null, "1");
	xmlAttr.append("default", QString::null, QString::null, "75");
	xmlAttr.append("factor", QString::null, QString::null, "10");
	reverb->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "constant");
	xmlAttr.append("name", QString::null, QString::null, "damp");
	xmlAttr.append("description", QString::null, QString::null, i18n("Damping"));
	xmlAttr.append("max", QString::null, QString::null, "100");
	xmlAttr.append("min", QString::null, QString::null, "1");
	xmlAttr.append("default", QString::null, QString::null, "50");
	xmlAttr.append("factor", QString::null, QString::null, "100");
	reverb->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	xmlAttr.clear();
	effectList->append(reverb);

	// Reverb 2
	EffectDesc *reverb2 = new EffectDesc(i18n("Reverb"), "ladspa1423", "audio");
	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "constant");
	xmlAttr.append("name", QString::null, QString::null, "time");
	xmlAttr.append("description", QString::null, QString::null, i18n("Reverb time"));
	xmlAttr.append("max", QString::null, QString::null, "85");
	xmlAttr.append("min", QString::null, QString::null, "1");
	xmlAttr.append("default", QString::null, QString::null, "42");
	xmlAttr.append("factor", QString::null, QString::null, "10");
	reverb2->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "constant");
	xmlAttr.append("name", QString::null, QString::null, "damp");
	xmlAttr.append("description", QString::null, QString::null, i18n("Damping"));
	xmlAttr.append("max", QString::null, QString::null, "100");
	xmlAttr.append("min", QString::null, QString::null, "0");
	xmlAttr.append("default", QString::null, QString::null, "25");
	xmlAttr.append("factor", QString::null, QString::null, "100");
	reverb2->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	xmlAttr.clear();
	effectList->append(reverb2);

	// Equalizer
	EffectDesc *equ = new EffectDesc(i18n("Equalizer"), "ladspa1901", "audio");
	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "constant");
	xmlAttr.append("name", QString::null, QString::null, "logain");
	xmlAttr.append("description", QString::null, QString::null, i18n("Lo gain"));
	xmlAttr.append("max", QString::null, QString::null, "6");
	xmlAttr.append("min", QString::null, QString::null, "-70");
	xmlAttr.append("default", QString::null, QString::null, "0");
	equ->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "constant");
	xmlAttr.append("name", QString::null, QString::null, "midgain");
	xmlAttr.append("description", QString::null, QString::null, i18n("Mid gain"));
	xmlAttr.append("max", QString::null, QString::null, "6");
	xmlAttr.append("min", QString::null, QString::null, "-70");
	xmlAttr.append("default", QString::null, QString::null, "0");
	equ->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	xmlAttr.clear();
	xmlAttr.append("type", QString::null, QString::null, "constant");
	xmlAttr.append("name", QString::null, QString::null, "higain");
	xmlAttr.append("description", QString::null, QString::null, i18n("Hi gain"));
	xmlAttr.append("max", QString::null, QString::null, "6");
	xmlAttr.append("min", QString::null, QString::null, "-70");
	xmlAttr.append("default", QString::null, QString::null, "0");
	equ->addParameter(effectDescParamFactory.createParameter(xmlAttr));
	xmlAttr.clear();
	effectList->append(equ);
    }
}

//static 
char* initEffects::ladspaEffectString(int ladspaId, QStringList params)
{
    if (ladspaId == 1433 ) //Pitch
	return ladspaPitchEffectString(params);
    else if (ladspaId == 1216 ) //Room Reverb
	return ladspaRoomReverbEffectString(params);
    else if (ladspaId == 1423 ) //Reverb
	return ladspaReverbEffectString(params);
    else if (ladspaId == 1901 ) //Reverb
	return ladspaEqualizerEffectString(params);
    else {
	kdDebug()<<"++++++++++  ASKING FOR UNKNOWN LADSPA EFFECT: "<<ladspaId<<endl;
	return("<jackrack></jackrack>");
    }
}

//static 
void initEffects::ladspaEffectFile(const QString & fname, int ladspaId, QStringList params)
{
    char *filterString;
    if (ladspaId == 1433 ) //Pitch
	filterString = ladspaPitchEffectString(params);
    else if (ladspaId == 1216 ) //Room Reverb
	filterString = ladspaRoomReverbEffectString(params);
    else if (ladspaId == 1423 ) //Reverb
	filterString = ladspaReverbEffectString(params);
    else if (ladspaId == 1901 ) //Reverb
	filterString = ladspaEqualizerEffectString(params);
    else {
	kdDebug()<<"++++++++++  ASKING FOR UNKNOWN LADSPA EFFECT: "<<ladspaId<<endl;
	return;
    }

	QFile f( QString( KdenliveSettings::currenttmpfolder() + fname).ascii() );
    	if ( f.open( IO_WriteOnly ) ) 
	{
        	QTextStream stream( &f );
		stream << filterString;
		f.close();
    	}
    	else kdDebug()<<"++++++++++  ERROR CANNOT WRITE TO: "<<KdenliveSettings::currenttmpfolder() +  fname<<endl;
}

char* initEffects::ladspaPitchEffectString(QStringList params)
{
	return KRender::decodedString( QString("<?xml version=\"1.0\"?><!DOCTYPE jackrack SYSTEM \"http://purge.bash.sh/~rah/jack_rack_1.2.dtd\"><jackrack><channels>2</channels><samplerate>48000</samplerate><plugin><id>1433</id><enabled>true</enabled><wet_dry_enabled>false</wet_dry_enabled><wet_dry_locked>true</wet_dry_locked><wet_dry_values><value>1.0</value><value>1.0</value></wet_dry_values><lockall>true</lockall><controlrow><lock>true</lock><value>%1</value><value>%2</value></controlrow><controlrow><lock>true</lock><value>4.000000</value><value>4.000000</value></controlrow></plugin></jackrack>").arg(params[0]).arg(params[0]));
}

char* initEffects::ladspaRoomReverbEffectString(QStringList params)
{
	return KRender::decodedString( QString("<?xml version=\"1.0\"?><!DOCTYPE jackrack SYSTEM \"http://purge.bash.sh/~rah/jack_rack_1.2.dtd\"><jackrack><channels>2</channels><samplerate>48000</samplerate>  <plugin><id>1216</id><enabled>true</enabled><wet_dry_enabled>false</wet_dry_enabled><wet_dry_locked>true</wet_dry_locked><wet_dry_values><value>1.000000</value><value>1.000000</value></wet_dry_values><lockall>true</lockall><controlrow><lock>true</lock><value>%1</value><value>%2</value></controlrow><controlrow><lock>true</lock><value>%3</value><value>%4</value></controlrow><controlrow><lock>true</lock><value>%5</value><value>%6</value></controlrow><controlrow><lock>true</lock><value>0.750000</value><value>0.750000</value></controlrow><controlrow><lock>true</lock><value>-70.000000</value><value>-70.000000</value></controlrow><controlrow><lock>true</lock><value>0.000000</value><value>0.000000</value></controlrow><controlrow><lock>true</lock><value>-17.500000</value><value>-17.500000</value></controlrow></plugin></jackrack>").arg(params[0]).arg(params[0]).arg(params[1]).arg(params[1]).arg(params[2]).arg(params[2]));
}

char* initEffects::ladspaReverbEffectString(QStringList params)
{
	return KRender::decodedString( QString("<?xml version=\"1.0\"?><!DOCTYPE jackrack SYSTEM \"http://purge.bash.sh/~rah/jack_rack_1.2.dtd\"><jackrack><channels>2</channels><samplerate>48000</samplerate><plugin><id>1423</id><enabled>true</enabled>  <wet_dry_enabled>false</wet_dry_enabled><wet_dry_locked>true</wet_dry_locked>    <wet_dry_values><value>1.000000</value><value>1.000000</value></wet_dry_values>    <lockall>true</lockall><controlrow><lock>true</lock><value>%1</value>      <value>%2</value></controlrow><controlrow><lock>true</lock><value>%3</value><value>%4</value></controlrow><controlrow><lock>true</lock><value>0.250000</value><value>0.250000</value></controlrow></plugin></jackrack>").arg(params[0]).arg(params[0]).arg(params[1]).arg(params[1]));
}

char* initEffects::ladspaEqualizerEffectString(QStringList params)
{
	return KRender::decodedString( QString("<?xml version=\"1.0\"?><!DOCTYPE jackrack SYSTEM \"http://purge.bash.sh/~rah/jack_rack_1.2.dtd\"><jackrack><channels>2</channels><samplerate>48000</samplerate><plugin><id>1901</id><enabled>true</enabled>    <wet_dry_enabled>false</wet_dry_enabled><wet_dry_locked>true</wet_dry_locked>    <wet_dry_values><value>1.000000</value><value>1.000000</value></wet_dry_values><controlrow><value>%1</value></controlrow><controlrow><value>%2</value></controlrow>    <controlrow><value>%3</value></controlrow></plugin></jackrack>").arg(params[0]).arg(params[1]).arg(params[2]));
}


