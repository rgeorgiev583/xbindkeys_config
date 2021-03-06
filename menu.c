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
#include "middle.h"
#include "menu.h"
#include "version.h"



void menu_open_file (GtkMenuItem *menuitem, gpointer user_data);
void menu_open_default_file (GtkMenuItem *menuitem, gpointer user_data);
gboolean menu_open_file_ok(GtkFileSelection *Sel);
void menu_save_default_file (GtkMenuItem *menuitem, gpointer user_data);
void menu_save_file (GtkMenuItem *menuitem, gpointer user_data);
gboolean menu_save_file_ok(GtkFileSelection *Sel);
void menu_append_file (GtkMenuItem *menuitem, gpointer user_data);
gboolean menu_append_file_ok(GtkFileSelection *Sel);

void menu_about (GtkMenuItem *menuitem, gpointer user_data);
void menu_manual (GtkMenuItem *menuitem, gpointer user_data);
void lockfilewindow();
void unlockfilewindow();

GtkWidget *OpenAs;
GtkWidget *AppendAs;
GtkWidget *SaveAs;
GtkWidget *About;
GtkWidget *Manual;

GtkWidget *button_undo_menu;

GtkWidget * xbindkeys_config_menu(GtkAccelGroup *accel_group)
{
  GtkWidget *Handle;
  GtkWidget *MenuBar;
  GtkItemFactory *item_factory;
  
  static GtkItemFactoryEntry menu_items[] = {
    {"/_File", NULL, 0, 0, "<Branch>"},
    {"/File/tearoff", NULL, 0, 0, "<Tearoff>"},
    {"/File/New file", "<control>N", new_file, 0},
    {"/File/Open default file", "<control>O", menu_open_default_file, 0},
    {"/File/Save to default file", "<control>S", menu_save_default_file, 0},
    {"/File/Nothing", NULL, 0, 0,"<Separator>"},
    {"/File/Open as ...", "<control><shift>O", menu_open_file, 0},
    {"/File/Append file ...", "<control><shift>A", menu_append_file, 0},
    {"/File/Save as ...", "<control><shift>S", menu_save_file, 0},
    {"/File/Nothing", NULL, 0, 0,"<Separator>"},
    {"/File/Exit (no Saving)", "<control>Q", main_exit, 0},

    {"/_Edit", NULL, 0, 0, "<Branch>"},
    {"/Edit/tearoff", NULL, 0, 0, "<Tearoff>"},
    {"/Edit/Undo", "<control>Z", undo, 0},
    {"/Edit/Nothing", NULL, 0, 0,"<Separator>"},
    {"/Edit/View generated rc file", NULL, view_generated_file, 0},
    {"/Edit/Set to default rc file", NULL, set_default, 0},

/*
    {"/_SpeedConfig", NULL, 0, 0, "<Branch>"},
    {"/SpeedConfig/tearoff", NULL, 0, 0, "<Tearoff>"},
    {"/SpeedConfig/Quik Save", NULL, undo, 0},
    {"/Edit/Nothing", NULL, 0, 0,"<Separator>"},
    {"/Edit/View generated rc file", NULL, view_generated_file, 0},
    {"/Edit/Set to default rc file", NULL, set_default, 0}, */


    {"/_Help", NULL, 0, 0, "<LastBranch>"},
    {"/Help/tearoff", NULL, 0, 0, "<Tearoff>"},
    {"/Help/About ...", "<control>A", menu_about, 0, "<Item>"},
    {"/Help/Manual ...", "<control>M", menu_manual, 0}
  };

  Handle   = gtk_handle_box_new();
  item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR,
				       "<Main>",
				       accel_group);

  gtk_item_factory_create_items ( item_factory, 21, menu_items, NULL);
  MenuBar = gtk_item_factory_get_widget( item_factory, "<Main>");
  
  button_undo_menu = gtk_item_factory_get_widget( item_factory, "/Edit/Undo");

  OpenAs = gtk_item_factory_get_widget( item_factory, "/File/Open as ...");
  AppendAs = gtk_item_factory_get_widget( item_factory, "/File/Append file ...");
  SaveAs = gtk_item_factory_get_widget( item_factory, "/File/Save as ...");

  About = gtk_item_factory_get_widget( item_factory, "/Help/About ...");
  Manual = gtk_item_factory_get_widget( item_factory, "/Help/Manual ...");

  gtk_container_add(GTK_CONTAINER(Handle), MenuBar );

  return Handle;
}


void menu_open_file (GtkMenuItem *menuitem, gpointer user_data)
{
  GtkFileSelection *FichierDgl;
  FichierDgl=(GtkFileSelection*) gtk_file_selection_new("Open xbindkeysrc file");

  gtk_signal_connect_object(GTK_OBJECT(FichierDgl->ok_button), "clicked",
                    (GtkSignalFunc)menu_open_file_ok, GTK_OBJECT(FichierDgl));

  gtk_signal_connect_object(GTK_OBJECT(FichierDgl->ok_button), "clicked",
			    (GtkSignalFunc)unlockfilewindow, NULL);
  gtk_signal_connect_object(GTK_OBJECT(FichierDgl->ok_button), "clicked",
			    (GtkSignalFunc)gtk_widget_destroy, GTK_OBJECT(FichierDgl));
  
  gtk_signal_connect_object(GTK_OBJECT(FichierDgl->cancel_button), "clicked",
  			    (GtkSignalFunc)unlockfilewindow, NULL);
  gtk_signal_connect_object(GTK_OBJECT(FichierDgl->cancel_button), "clicked",
			    (GtkSignalFunc)gtk_widget_destroy, GTK_OBJECT(FichierDgl));

  gtk_signal_connect(GTK_OBJECT(FichierDgl), "delete_event",
		     (GtkSignalFunc)gtk_widget_destroy, NULL);
  gtk_signal_connect_object(GTK_OBJECT(FichierDgl), "delete_event",
			    (GtkSignalFunc)unlockfilewindow, NULL);

  gtk_signal_connect(GTK_OBJECT(FichierDgl), "destroy",
		     (GtkSignalFunc)gtk_widget_destroy, NULL);
  gtk_signal_connect_object(GTK_OBJECT(FichierDgl), "destroy",
			    (GtkSignalFunc)unlockfilewindow, NULL);

  gtk_widget_show(GTK_WIDGET(FichierDgl));

  lockfilewindow();
}

gboolean menu_open_file_ok(GtkFileSelection *Sel)
{
  gchar *nom;
  nom=gtk_file_selection_get_filename(Sel);
  read_file(nom);
  return TRUE;
}

void menu_open_default_file (GtkMenuItem *menuitem, gpointer user_data)
{
  char buf[1024];
  sprintf(buf, "%s/.xbindkeysrc", getenv ("HOME"));
  read_file(buf);
}

void menu_save_default_file (GtkMenuItem *menuitem, gpointer user_data)
{
  char buf[1024];
  sprintf(buf, "%s/.xbindkeysrc", getenv ("HOME"));
  save_file(buf);
}

void menu_save_file (GtkMenuItem *menuitem, gpointer user_data)
{
  GtkFileSelection *FichierDgl;
  FichierDgl=(GtkFileSelection*) gtk_file_selection_new("Save xbindkeysrc file");

  gtk_signal_connect_object(GTK_OBJECT(FichierDgl->ok_button), "clicked",
                    (GtkSignalFunc)menu_save_file_ok, GTK_OBJECT(FichierDgl));
  gtk_signal_connect_object(GTK_OBJECT(FichierDgl->ok_button), "clicked",
                    (GtkSignalFunc)gtk_widget_destroy, GTK_OBJECT(FichierDgl));

  gtk_signal_connect_object(GTK_OBJECT(FichierDgl->cancel_button), "clicked",
                    (GtkSignalFunc)unlockfilewindow, NULL);
  gtk_signal_connect_object(GTK_OBJECT(FichierDgl->cancel_button), "clicked",
                    (GtkSignalFunc)gtk_widget_destroy, GTK_OBJECT(FichierDgl));

  gtk_signal_connect(GTK_OBJECT(FichierDgl), "delete_event",
		     (GtkSignalFunc)gtk_widget_destroy, NULL);
  gtk_signal_connect_object(GTK_OBJECT(FichierDgl), "delete_event",
			    (GtkSignalFunc)unlockfilewindow, NULL);
  gtk_signal_connect(GTK_OBJECT(FichierDgl), "destroy",
		     (GtkSignalFunc)gtk_widget_destroy, NULL);
  gtk_signal_connect_object(GTK_OBJECT(FichierDgl), "destroy",
			    (GtkSignalFunc)unlockfilewindow, NULL);

  gtk_widget_show(GTK_WIDGET(FichierDgl));
  lockfilewindow();
}

gboolean menu_save_file_ok(GtkFileSelection *Sel)
{
  gchar *nom;
  nom=gtk_file_selection_get_filename(Sel);
  save_file(nom);
  return TRUE;
}

void menu_append_file (GtkMenuItem *menuitem, gpointer user_data)
{
  GtkFileSelection *FichierDgl;
  FichierDgl=(GtkFileSelection*) gtk_file_selection_new("Append xbindkeysrc file");

  gtk_signal_connect_object(GTK_OBJECT(FichierDgl->ok_button), "clicked",
                    (GtkSignalFunc)menu_append_file_ok, GTK_OBJECT(FichierDgl));
  gtk_signal_connect_object(GTK_OBJECT(FichierDgl->ok_button), "clicked",
                    (GtkSignalFunc)gtk_widget_destroy, GTK_OBJECT(FichierDgl));

  gtk_signal_connect_object(GTK_OBJECT(FichierDgl->cancel_button), "clicked",
                    (GtkSignalFunc)unlockfilewindow, NULL);
  gtk_signal_connect_object(GTK_OBJECT(FichierDgl->cancel_button), "clicked",
                    (GtkSignalFunc)gtk_widget_destroy, GTK_OBJECT(FichierDgl));

  gtk_signal_connect(GTK_OBJECT(FichierDgl), "delete_event",
		     (GtkSignalFunc)gtk_widget_destroy, NULL);
  gtk_signal_connect_object(GTK_OBJECT(FichierDgl), "delete_event",
			    (GtkSignalFunc)unlockfilewindow, GTK_OBJECT(FichierDgl));
  
  gtk_signal_connect(GTK_OBJECT(FichierDgl), "destroy",
		     (GtkSignalFunc)gtk_widget_destroy, NULL);
  gtk_signal_connect_object(GTK_OBJECT(FichierDgl), "destroy",
			    (GtkSignalFunc)unlockfilewindow, GTK_OBJECT(FichierDgl));
 

  gtk_widget_show(GTK_WIDGET(FichierDgl));
  lockfilewindow();
}

gboolean menu_append_file_ok(GtkFileSelection *Sel)
{
  gchar *nom;
  nom=gtk_file_selection_get_filename(Sel);
  append_file(nom);
  return TRUE;
}

void unlockabout()
{
  gtk_widget_set_sensitive (GTK_WIDGET(About), TRUE);
}

void unlockmanual()
{
  gtk_widget_set_sensitive (GTK_WIDGET(Manual), TRUE);
}


void menu_about (GtkMenuItem *menuitem, gpointer user_data)
{
  GtkWidget *window;
  GtkWidget *text;

  window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
  gtk_window_set_title(GTK_WINDOW(window),
		       "About XBindKeys Config");

  text = gtk_label_new("\n    XBindKeys_Config    \n\n"\
		       "ver : " XBINDKEYS_CONFIG_VERSION "\n\n"\
		       "by Laurent VUIBERT\n"\
		       " lapinator@gmx.fr\n\n");

  gtk_signal_connect(GTK_OBJECT(window), "delete_event",
		     (GtkSignalFunc)unlockabout, NULL);
  gtk_signal_connect_object(GTK_OBJECT(window), "delete_event",
			    (GtkSignalFunc)gtk_widget_destroy, GTK_OBJECT(window));
  gtk_signal_connect(GTK_OBJECT(window), "destroy",
		     (GtkSignalFunc)unlockabout, NULL);
  gtk_signal_connect_object(GTK_OBJECT(window), "delete_event",
			    (GtkSignalFunc)gtk_widget_destroy, GTK_OBJECT(window));

  gtk_container_add(GTK_CONTAINER(window),text);
  gtk_widget_set_sensitive (GTK_WIDGET(About), FALSE);
  gtk_widget_show_all(window);
}

#define MANUAL_TEXT "To edit action, first append item and select them.\n\nTo get key use button \"Get Key\", don't write \nthe key (only for expert).\n\n\n                        FIX ME  (HELP ME)\n\n\n"

void menu_manual (GtkMenuItem *menuitem, gpointer user_data)
{
  GtkWidget *window;
  GtkWidget *text;
  GtkWidget *vbox;
  
  window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
  gtk_window_set_title(GTK_WINDOW(window),
		       "Manual of xbindkeys Config");
  
  vbox = gtk_vbox_new (FALSE,0);
  
  text = gtk_label_new("\nManual\n");
  gtk_box_pack_start(GTK_BOX(vbox), text, FALSE, FALSE, 0);
  
  text = gtk_text_new(NULL,NULL);
  
  
  gtk_text_insert (text, NULL, NULL, NULL,
		   MANUAL_TEXT, sizeof(MANUAL_TEXT)-1);
  gtk_box_pack_start(GTK_BOX(vbox), text, TRUE, TRUE, 0);
  gtk_widget_set_usize(text,300,250);
  
   gtk_signal_connect(GTK_OBJECT(window), "delete_event",
		     (GtkSignalFunc)unlockmanual, NULL);
  gtk_signal_connect_object(GTK_OBJECT(window), "delete_event",
			    (GtkSignalFunc)gtk_widget_destroy, GTK_OBJECT(window));
  gtk_signal_connect(GTK_OBJECT(window), "destroy",
		     (GtkSignalFunc)unlockmanual, NULL);
  gtk_signal_connect_object(GTK_OBJECT(window), "delete_event",
			    (GtkSignalFunc)gtk_widget_destroy, GTK_OBJECT(window));

 
  /*gtk_label_set_justify (GTK_LABEL(text), GTK_JUSTIFY_LEFT);*/

  gtk_container_add(GTK_CONTAINER(window),vbox);
  gtk_widget_set_sensitive (GTK_WIDGET(Manual), FALSE);
  gtk_widget_show_all(window);
}

void lockfilewindow()
{
  gtk_widget_set_sensitive (GTK_WIDGET(OpenAs), FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET(AppendAs), FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET(SaveAs), FALSE);
}

void unlockfilewindow()
{
  gtk_widget_set_sensitive (GTK_WIDGET(OpenAs), TRUE);
  gtk_widget_set_sensitive (GTK_WIDGET(AppendAs), TRUE);
  gtk_widget_set_sensitive (GTK_WIDGET(SaveAs), TRUE);
}
