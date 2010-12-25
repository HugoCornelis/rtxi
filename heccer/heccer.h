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

#include <default_gui_model.h>

#include <heccer/heccer.h>


class Heccer : public DefaultGUIModel
{

public:

    Heccer(void);
    virtual ~Heccer(void);

    void execute(void);

protected:

    void update(DefaultGUIModel::update_flags_t);

    void load(char *srcpath);
    void load_dialog(void);

private:

    double dGainG;

    double dGlobalOffset;

    double dTime;

    double dStep;

    double dERev;

    int iEnabled0;

    char *pcHeccer;

    FILE *pfileHeccer;
};
