/*
 * Copyright (C) 2011 Hugo Cornelis hugo.cornelis@gmail.com
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <heccer_rtxi.h>

#include <stdio.h>

#include <qfiledialog.h>

#include <main_window.h>


extern "C" Plugin::Object *createRTXIPlugin(void) {
    return new Heccer();
}

static DefaultGUIModel::variable_t vars[] = {
    {
        "Vin",
        "The signal recorded from the tissue",
        DefaultGUIModel::INPUT,
    },
    {
        "Gout",
        "Computed channel conductance",
        DefaultGUIModel::OUTPUT,
    },
    {
        "Erev",
        "Computed reversal potential of the channel",
        DefaultGUIModel::OUTPUT,
    },
    {
        "Iout",
        "The scaled and offset copy of the current",
        DefaultGUIModel::OUTPUT,
    },

    {
        "Gain",
        "Scaling factor applied to the current it is injected",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },

    {
        "Global Offset",
        "Global offset current",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "tStep",
        "Sampling period of the real time thread, currently also the time step of the solver",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Channel enabled ?",
        "Enter a '1' to enable model current injection",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::UINTEGER,
    },
};

static size_t num_vars = sizeof(vars)/sizeof(DefaultGUIModel::variable_t);

Heccer::Heccer(void)
    : DefaultGUIModel("Heccer_RTXI",::vars,::num_vars) {

    /*
     * Initialize Parameters & Variables
     */

    dGainG = 1.0;

    dGlobalOffset = 1e-10;

    dERev = - 0.08;

    iEnabled0 = 0;

    load_dialog();

    if (pcHeccer == NULL)
    {
	pcHeccer = "/tmp/channel_model.heccer";
    }

    pfileHeccer = fopen(pcHeccer, "r");

    if (pfileHeccer)
    {
	pheccer = HeccerDeserialize(pfileHeccer);

	if (pheccer)
	{
	    //- set time

	    dTime = 0;

	    //- set sampling period, default 10kHz

	    dStep = pheccer->dStep;

	    /*
	     * Initialize the GUI
	     */

	    setParameter("Gain", dGain);

	    setParameter("Global Offset", dGlobalOffset);
	    setParameter("tStep", dStep);

	    setParameter("Channel enabled ?", iEnabled0);
	}
	else
	{
	    fprintf(stderr, "rtxi: error: /tmp/channel_model.heccer is not a heccer dump file\n");
	}
    }
    else
    {
	fprintf(stderr, "rtxi: error: /tmp/channel_model.heccer cannot be read from file\n");
    }

    refresh();
}

Heccer::~Heccer(void) {}


void Heccer::load(char *srcpath) 
{
     pcHeccer = canonicalize_file_name (srcpath);
}


void Heccer::load_dialog(void)
{
#define MODEL_SOURCE_SUFFIX    ".heccer"

     QString file_name = QFileDialog::getOpenFileName(QString::null,"Conductance file (*" MODEL_SOURCE_SUFFIX \
						      ");;All (*.*)",MainWindow::getInstance());

     load((char *)file_name.latin1());
}


void Heccer::execute(void)
{
    //- do the heccer update

    dTime += dStep;

    HeccerHeccs(pheccer, dTime);

    //- output conductance

    int iSerialGk = 1000;

    double *pdGk = HeccerAddressVariable(pheccer, iSerialGk, "G");

    output(0) = dGk;

    //- output current

    double dIk = HeccerAddressVariable(pheccer, iSerialGk, "I");

    output(1) = dIk;

    //t output reversal potential: heccer does not support this yet in its addressing module

//     fprintf(stdout, "at %i (index %i), value %g, result %g\n", dTime, iIndex, dGk, dGkCorrected);

    //- output current times gain factor

    output(3) = (dIk + dGlobalOffset) * dGain;
}


void Heccer::update(DefaultGUIModel::update_flags_t flag)
{
    if (flag == MODIFY)
    {
        dGain = getParameter("Gain").toDouble();
	dGlobalOffset = getParameter("Global Offset").toDouble();
        dStep = getParameter("tStep").toDouble();

	iEnabled0 = (int)getParameter("Channel enabled ?").toDouble();
    }
}
