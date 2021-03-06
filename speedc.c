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
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>


#include "speedc.h"
#include "middle.h"
#include "version.h"

/* TODO change to dynamic list */
typedef struct list_speedc
{
  char Name[30];
  char File[1024];
} list_speedc;

list_speedc list[30];

int SPEEDC_ROW;

extern GtkWidget *label_warning;

static GtkWidget *Speed_config;

/*****************************************************************************/
void xbindkeys_config_speed_init()
{

  char *p, buf[1024];
  int i;

  DIR *dir;
  struct dirent *dp;


  /* clear the Speed_config */
  gtk_clist_clear (GTK_CLIST(Speed_config));


  /* create $HOME/.xbindkey_config is not exist */
  sprintf(buf,"%s/.xbindkeys_config", getenv ("HOME"));
  dir = opendir(buf);
  if( dir==NULL ) {
    if( errno==ENOENT ){
      i = mkdir( buf, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      printf("make file ~/.xbindkeys_config\n");
    }
  } else 
    closedir(dir);
  
  /* create $HOME/.xbindkey_config/speed_c is not exist */
  sprintf(buf,"%s/.xbindkeys_config/speed_c", getenv ("HOME"));
  dir = opendir(buf);
  if( dir==NULL ) {
    if( errno==ENOENT ){
      i = mkdir( buf, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      printf("make file ~/.xbindkeys_config/speed_c\n");
    }
  } else 
    closedir(dir);

  /* add the default file */
  p = g_strdup_printf("Default");
  gtk_clist_append(GTK_CLIST(Speed_config),  &p );
 
  sprintf(list[0].Name, "Default");
  sprintf(list[0].File, "%s/.xbindkeysrc", getenv ("HOME"));

  
  /* list all file in $HOME/.xbindkey_config/speed_c */
  if(!(dir = opendir(buf))) { 
    return;
  }
  
  dp = readdir(dir);
  while(dp!=NULL){
    if(dp->d_name[0]!='.'){
      p = g_strdup_printf(dp->d_name);
      gtk_clist_append(GTK_CLIST(Speed_config),  &p );
    }
    dp=readdir(dir);
  }

  closedir(dir);
}



/*****************************************************************************/
void speedc_load(GtkWidget *parent, void *data)
{
  char buf2[1024];
  char *buf[1024];

  if(SPEEDC_ROW==0){
    sprintf(buf2, "%s/.xbindkeysrc", getenv ("HOME"));
    read_file(buf2);
  }
  else {
    gtk_clist_get_text (GTK_CLIST(Speed_config), SPEEDC_ROW, 0, buf); 
    sprintf(buf2, "%s/.xbindkeys_config/speed_c/%s", getenv ("HOME"), buf[0]);
    read_file(buf2);
  }
  return;
}

/*****************************************************************************/
void speedc_apply (GtkWidget *parent, void *data)
{
  middle_apply_action(NULL, NULL);

}
/*****************************************************************************/
void speed_select_row (GtkCList *clist, gint row, gint column,
		       GdkEventButton *event, gpointer user_data)
{
  SPEEDC_ROW=row;
  return;
}

/*****************************************************************************/
void speed_select_row_show(GtkCList *clist, gint row, gint column,
			   GdkEventButton *event, gpointer user_data)
{
  SPEEDC_ROW=row;
  speedc_load(NULL, NULL);
}

/*****************************************************************************/
void speedc_save_ok(GtkWidget *parent, GtkWidget *label)
{
  char buf2[1024];

  sprintf(buf2, "%s/.xbindkeys_config/speed_c/%s", getenv ("HOME"),
	  gtk_entry_get_text(GTK_ENTRY(label)));
  save_file(buf2);
  xbindkeys_config_speed_init();
  return;
}

/*****************************************************************************/
void speedc_save_cancel(GtkWidget *parent, void *data)
{
  printf("cancel\n");
}


/*****************************************************************************/
void speedc_save(GtkWidget *parent, void *data)
{
  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *label;
  GtkWidget *button;


  window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
  gtk_window_set_title(GTK_WINDOW(window),
		       "Xbindkeys Config Version : " XBINDKEYS_CONFIG_VERSION );
  
  vbox = gtk_vbox_new (FALSE, 4);
  gtk_container_add(GTK_CONTAINER(window),vbox);
  
  /* The txt */
  hbox = gtk_hbox_new (FALSE, 4);
  gtk_container_add(GTK_CONTAINER(vbox),hbox);
  
  label = gtk_label_new("Save As :");
  gtk_container_add(GTK_CONTAINER(hbox),label);
  
  label =  gtk_entry_new_with_max_length(16);
  gtk_container_add(GTK_CONTAINER(hbox),label);
  
  /* Button */
  hbox = gtk_hbox_new (FALSE, 4);
  gtk_container_add(GTK_CONTAINER(vbox),hbox);
  
  button = gtk_button_new_with_label("Save");
  gtk_container_add(GTK_CONTAINER(hbox),button);
  gtk_signal_connect(GTK_OBJECT(button), "pressed",
		     (GtkSignalFunc)speedc_save_ok, label);
  gtk_signal_connect_object(GTK_OBJECT(button), "pressed",
			    (GtkSignalFunc)gtk_widget_destroy, GTK_OBJECT(window));
  
  
  button = gtk_button_new_with_label("Cancel");
  gtk_container_add(GTK_CONTAINER(hbox),button);
  gtk_signal_connect(GTK_OBJECT(button), "pressed",
		     (GtkSignalFunc)speedc_save_cancel, NULL);
  gtk_signal_connect_object(GTK_OBJECT(button), "pressed",
			    (GtkSignalFunc)gtk_widget_destroy, GTK_OBJECT(window));
  
  gtk_widget_show_all(window);
  
  return;
}


/*****************************************************************************/
void speedc_del(GtkWidget *parent, void *data)
{
  char buf2[1024];
  char *buf[1024];

  if(SPEEDC_ROW==0){
    gtk_label_set_text (GTK_LABEL(label_warning),
			"\nDefault cannot be deleted\n");
  }
  else {
    gtk_clist_get_text (GTK_CLIST(Speed_config), SPEEDC_ROW, 0, buf); 
    sprintf(buf2, "%s/.xbindkeys_config/speed_c/%s", getenv ("HOME"), buf[0]);
    unlink(buf2);
    xbindkeys_config_speed_init();
  }

  return;
}


/*****************************************************************************/
GtkWidget * xbindkeys_config_speed_config(int show)
{
  GtkWidget *frame;
  GtkWidget *src;
  GtkWidget *vbox;
  GtkWidget *vbox2;
  GtkWidget *hbox;
  GtkWidget *button;

  vbox = gtk_vbox_new (FALSE, 4);
  vbox2 = gtk_vbox_new (FALSE, 4);


  frame =  gtk_frame_new ("Speed Config:");
  gtk_container_add(GTK_CONTAINER(vbox), frame);
  
  src = gtk_scrolled_window_new ( NULL, NULL );
  gtk_container_add(GTK_CONTAINER(frame), vbox2);
  gtk_box_pack_start(GTK_BOX(vbox2), src, TRUE, TRUE, 0);
  
  Speed_config = gtk_clist_new (1);

  if(show)
    gtk_signal_connect(GTK_OBJECT(Speed_config), "select-row",
		       (GtkSignalFunc)speed_select_row_show, NULL);
  else 
    gtk_signal_connect(GTK_OBJECT(Speed_config), "select-row",
		       (GtkSignalFunc)speed_select_row, NULL);

  gtk_clist_set_selection_mode (GTK_CLIST(Speed_config), GTK_SELECTION_BROWSE);
  gtk_clist_set_reorderable(GTK_CLIST(Speed_config), TRUE);
  
  gtk_container_add (GTK_CONTAINER(src), Speed_config);
  

  hbox = gtk_hbox_new (FALSE, 4);
  
  if(show){
    button = gtk_button_new_with_label("Apply");
    gtk_container_add(GTK_CONTAINER(hbox), button);
    gtk_signal_connect(GTK_OBJECT(button), "pressed",
		       (GtkSignalFunc)speedc_apply, NULL);
  }
  else {

    button = gtk_button_new_with_label("Load");
    gtk_container_add(GTK_CONTAINER(hbox), button);
    gtk_signal_connect(GTK_OBJECT(button), "pressed",
		       (GtkSignalFunc)speedc_load, NULL);


    button = gtk_button_new_with_label("Save");
    gtk_container_add(GTK_CONTAINER(hbox), button);
    gtk_signal_connect(GTK_OBJECT(button), "pressed",
		       (GtkSignalFunc)speedc_save, NULL);
    
    
    button = gtk_button_new_with_label("Del");
    gtk_container_add(GTK_CONTAINER(hbox), button);
    gtk_signal_connect(GTK_OBJECT(button), "pressed",
		       (GtkSignalFunc)speedc_del, NULL);
  }

  gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);
  xbindkeys_config_speed_init();

  return vbox;
}

