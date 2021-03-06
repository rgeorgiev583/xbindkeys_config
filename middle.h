/***************************************************************************
          xbindkeys_config : config programme for xbindkey.
                           -------------------
    begin                : Sat jan 5 14:11:34 CEST 2002
    copyright            : (C) 2002 by Laurent VUIBERT
    email                : lapinator@gmx.fr
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <gtk/gtk.h>

GtkWidget * xbindkeys_config_middle();
GtkWidget * xbindkeys_config_middle_list(gboolean show);

void new_file();
void read_file(char file[]);
void save_file(char file_out[]);
void append_file(char file_in[]);
void view_generated_file();
void undo();
void set_default();
void selectable_clist(gboolean state);
void main_exit(); 

void middle_apply_action(GtkWidget *parent, void *data);
