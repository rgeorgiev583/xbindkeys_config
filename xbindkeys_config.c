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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h"
#include "middle.h"
#include "version.h"
#include "speedc.h"

int main (int argc, char *argv[])
{
   GtkWidget *window;
   GtkWidget *menu;
   GtkWidget *middle;
   GtkWidget *vbox;
   GtkWidget *hbox;
   GtkAccelGroup *accel_group;
   char buf[1024];
   int i;
   gboolean show=FALSE;

   sprintf(buf, "%s/.xbindkeysrc", getenv ("HOME"));

   for(i=1; i<argc; i++){
     if(!strcmp(argv[i],"-h") | !strcmp(argv[i],"--help")){
       printf("XBKC: XBindKeys_Config " XBINDKEYS_CONFIG_VERSION " by Laurent Vuibert/n" \
	      "usage: [--version|-v][--file|-f file][--help|-h][--show|-s]\n" \
	      "\n" \
	      "  -h, --help     This Help\n" \
	      "  -f, --file     Use an alternative rc file\n" \
	      "  -s, --show     show only the rc file\n\n");
       exit(0);
     }
     if(!strcmp(argv[i],"-s") | !strcmp(argv[i],"--show"))
       show=TRUE;

     if(!strcmp(argv[i],"-f") | !strcmp(argv[i],"--file")){
       if(i+1<argc) strcpy(buf,argv[++i]);
       else printf("error: use xbindkey_config %s \"FILE\"\n", argv[i]);
     }
   }
   
   gtk_init( &argc, &argv );   

   if(show){
     window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
     gtk_window_set_title(GTK_WINDOW(window),
			  "Xbindkeys Config  *** Show ***  Version : " XBINDKEYS_CONFIG_VERSION );
     gtk_signal_connect(GTK_OBJECT(window), "delete_event",
			(GtkSignalFunc)main_exit, NULL);
     gtk_signal_connect(GTK_OBJECT(window), "destroy",
			(GtkSignalFunc)main_exit, NULL);
     
     hbox    = gtk_hbox_new(FALSE, 0);
     gtk_container_add(GTK_CONTAINER(window),hbox);

     middle = xbindkeys_config_middle_list(show);
     gtk_container_add(GTK_CONTAINER(hbox),middle);

     middle = xbindkeys_config_speed_config(1);
     gtk_container_add(GTK_CONTAINER(hbox),middle);

     read_file(buf);
     selectable_clist(FALSE);
   }
   else{
     window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
     gtk_window_set_title(GTK_WINDOW(window),
			  "Xbindkeys Config Version : " XBINDKEYS_CONFIG_VERSION );
     gtk_signal_connect(GTK_OBJECT(window), "delete_event",
			(GtkSignalFunc)main_exit, NULL);
     gtk_signal_connect(GTK_OBJECT(window), "destroy",
			(GtkSignalFunc)main_exit, NULL);
   
     vbox    = gtk_vbox_new(FALSE, 0);
     gtk_container_add(GTK_CONTAINER(window),vbox);
     
     accel_group = gtk_accel_group_new();
     menu=xbindkeys_config_menu(accel_group);
     gtk_box_pack_start(GTK_BOX(vbox),menu,FALSE,FALSE,0);
     gtk_window_add_accel_group(GTK_OBJECT(window), accel_group);
     
     middle= xbindkeys_config_middle();
     gtk_box_pack_start(GTK_BOX(vbox),middle,TRUE,TRUE,0);
     read_file(buf);   
   }
   
   gtk_widget_show_all(window);
   gtk_main();
   return 0;
}
