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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "middle.h"
#include "version.h"
#include "speedc.h"

#define TEMP_FILE "/tmp/xbindkeysrc-tmp"

gboolean middle_get_key(GtkWidget *parent, void *data);
gboolean middle_run_action(GtkWidget *parent, void *data);

void select_row (GtkCList *clist, gint row, gint column,
                 GdkEventButton *event, gpointer user_data);
void unselect_row(GtkCList *clist, gint row, gint column,
		    GdkEventButton *event, gpointer user_data);

void clist_set_text  (GtkButton *button, gpointer user_data);
void append_file(char file_in[]);
void middle_delete_item(GtkWidget *parent, void *data);
void middle_append_item(GtkWidget *parent, void *data);
void middle_up_item(GtkWidget *parent, void *data);
void middle_down_item(GtkWidget *parent, void *data);
void middle_save_apply_exit_action(GtkWidget *parent, void *data);
void middle_copy_item(GtkWidget *parent, void *data);
void middle_apply_action(GtkWidget *parent, void *data);

void middle_clist_set_visible();
void restart_xbindkeys();

extern GtkWidget *button_undo_menu;

#define TITLE_NB_ITEMS 3
static char *titles[TITLE_NB_ITEMS]=
{
  "Name", "Key", "Action"
};
static int titles_width[TITLE_NB_ITEMS]=
{
  100, 220, 100
};

static int ROW=-1;   /* Active ROW   -1 if no seleced row */

gboolean allow_refresh_gtk_entry = TRUE;  /* For bugs */


GtkWidget *entry_rem;
GtkWidget *entry_key;
GtkWidget *entry_act;
GtkWidget *clist;
GtkWidget *button_get_key;
GtkWidget *button_run_act;

GtkWidget *button_del_sel;
GtkWidget *button_cp_item;
GtkWidget *button_up;
GtkWidget *button_down;

GtkWidget *Flag_NumLock;
GtkWidget *Flag_ScrollLock;
GtkWidget *Flag_CapsLock;

GtkWidget *button_undo;
gchar button_undo_rem[128];
gchar button_undo_key[128];
gchar button_undo_act[128];

GtkWidget *label_warning;


/*****************************************************************************/
GtkWidget * xbindkeys_config_middle()
{
  GtkWidget *panedH;
  GtkWidget *src;
  GtkWidget *label;
  GtkWidget *button;

  GtkWidget *table;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *hbox2;
  GtkWidget *frame;


  panedH = gtk_hpaned_new();

  /*  LEFT paned */
  vbox = gtk_vbox_new (FALSE, 4);
  gtk_paned_pack1 ( GTK_PANED(panedH), vbox, TRUE, TRUE);

  src = xbindkeys_config_middle_list(FALSE);
  gtk_box_pack_start(GTK_BOX(vbox), src, TRUE, TRUE, 0);

  hbox = gtk_hbox_new (FALSE, 4);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

  button_del_sel = gtk_button_new_with_label("Delete Selected");
  gtk_box_pack_start(GTK_BOX(hbox), button_del_sel, TRUE, TRUE, 0);
  gtk_signal_connect(GTK_OBJECT(button_del_sel), "released",
                     (GtkSignalFunc)middle_delete_item, NULL);


  button = gtk_button_new_with_label("New");
  gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, TRUE, 0);
  gtk_signal_connect(GTK_OBJECT(button), "pressed",
		   (GtkSignalFunc)middle_append_item, NULL);

  button_cp_item = gtk_button_new_with_label("Copy Item");
  gtk_box_pack_start(GTK_BOX(hbox), button_cp_item, TRUE, TRUE, 0);
  gtk_signal_connect(GTK_OBJECT(button_cp_item), "pressed",
		     (GtkSignalFunc)middle_copy_item, NULL);

  hbox2 = gtk_hbox_new (TRUE, 4);
  gtk_box_pack_start(GTK_BOX(hbox), hbox2, TRUE, TRUE, 0);

  button_up = gtk_button_new_with_label("Up");
  gtk_box_pack_start(GTK_BOX(hbox2), button_up, TRUE, TRUE, 0);
  gtk_signal_connect(GTK_OBJECT(button_up), "pressed",
		   (GtkSignalFunc)middle_up_item, NULL);

  button_down = gtk_button_new_with_label("Down");
  gtk_box_pack_start(GTK_BOX(hbox2), button_down, TRUE, TRUE, 0);
  gtk_signal_connect(GTK_OBJECT(button_down), "pressed",
		     (GtkSignalFunc)middle_down_item, NULL);


  /* RIGHT paned */
  vbox = gtk_vbox_new (FALSE, 4);
  gtk_paned_pack2 ( GTK_PANED(panedH), vbox, TRUE, TRUE);


  frame =  gtk_frame_new ("Edit :");
  gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

  table = gtk_table_new (5, 2, FALSE);
  gtk_container_add(GTK_CONTAINER(frame), table);

    /* Remark */
  label =  gtk_label_new ("Name : ");
  gtk_label_set_justify (GTK_LABEL(label), GTK_JUSTIFY_LEFT);
  gtk_table_attach (GTK_TABLE(table), label,
		    0, 1, 0, 1,
		    GTK_FILL,
		    GTK_FILL,
		    0, 0);

  entry_rem =  gtk_entry_new();
  gtk_table_attach (GTK_TABLE(table), entry_rem,
		    1, 2, 0, 1,
		    GTK_FILL | GTK_EXPAND,
		    GTK_FILL | GTK_EXPAND,
		    0, 0);
  gtk_signal_connect(GTK_OBJECT(entry_rem), "changed",
		     (GtkSignalFunc)clist_set_text, NULL);

    /* key */
  label =  gtk_label_new ("Key : ");
  gtk_label_set_justify (GTK_LABEL(label), GTK_JUSTIFY_LEFT);

  gtk_table_attach (GTK_TABLE(table), label,
		    0, 1, 1, 2,
		    GTK_FILL,
		    GTK_FILL,
		    0, 0);

  entry_key =  gtk_entry_new();
  gtk_table_attach (GTK_TABLE(table), entry_key,
		    1, 2, 1, 2,
		    GTK_FILL | GTK_EXPAND,
		    GTK_FILL | GTK_EXPAND,
		    0, 0);
  gtk_signal_connect(GTK_OBJECT(entry_key), "changed",
		     (GtkSignalFunc)clist_set_text, NULL);

    /* Get Key */
  button_get_key = gtk_button_new_with_label("Get Key");
  gtk_label_set_justify (GTK_LABEL(label), GTK_JUSTIFY_LEFT);
  gtk_table_attach (GTK_TABLE(table), button_get_key,
		    1, 2, 2, 3,
		    GTK_FILL | GTK_EXPAND,
		    GTK_FILL | GTK_EXPAND,
		    0, 0);
   gtk_signal_connect(GTK_OBJECT(button_get_key), "pressed",
                     (GtkSignalFunc)middle_get_key, NULL);

    /* Action */
  label =  gtk_label_new ("Action : ");
  gtk_table_attach (GTK_TABLE(table), label,
		    0, 1, 3, 4,
		    GTK_FILL,
		    GTK_FILL,
		    0, 0);

  entry_act =  gtk_entry_new();
  gtk_table_attach (GTK_TABLE(table), entry_act,
		    1, 2, 3, 4,
		    GTK_FILL | GTK_EXPAND,
		    GTK_FILL | GTK_EXPAND,
		    0, 0);
  gtk_signal_connect(GTK_OBJECT(entry_act), "changed",
		     (GtkSignalFunc)clist_set_text, NULL);

    /* Action */
  button_run_act = gtk_button_new_with_label("Run Action");
  gtk_table_attach (GTK_TABLE(table), button_run_act,
		    1, 2, 4, 5,
		    GTK_FILL | GTK_EXPAND,
		    GTK_FILL | GTK_EXPAND,
		    0, 0);
   gtk_signal_connect(GTK_OBJECT(button_run_act), "pressed",
                     (GtkSignalFunc)middle_run_action, NULL);

	/* Undo */
   button_undo = gtk_button_new_with_label("Undo");
   gtk_table_attach (GTK_TABLE(table), button_undo,
		    1, 2, 5, 6,
		    GTK_FILL | GTK_EXPAND,
		    GTK_FILL | GTK_EXPAND,
		    0, 0);
   gtk_signal_connect(GTK_OBJECT(button_undo), "pressed",
                     (GtkSignalFunc)undo, NULL);

   /* Flags */
   frame =  gtk_frame_new ("Use state of:");
   gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

   /* vbox2 : warnning the name is hbox */
   hbox = gtk_vbox_new (FALSE, 4);
   gtk_container_add(GTK_CONTAINER(frame), hbox);

    
   /* NumLock */
   Flag_NumLock = gtk_check_button_new_with_label ("NumLock");
   gtk_box_pack_start(GTK_BOX(hbox), Flag_NumLock, FALSE, FALSE, 0);

   /* Flag_ScrollLock */
   Flag_ScrollLock = gtk_check_button_new_with_label ("ScrollLock");
   gtk_box_pack_start(GTK_BOX(hbox), Flag_ScrollLock, FALSE, FALSE, 0);

   /* Flag_CapsLock */
   Flag_CapsLock = gtk_check_button_new_with_label ("CapsLock");
   gtk_box_pack_start(GTK_BOX(hbox), Flag_CapsLock, FALSE, FALSE, 0);

   /*  Update */
   button = gtk_button_new_with_label("Apply");
   gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
   gtk_signal_connect(GTK_OBJECT(button), "pressed",
		      (GtkSignalFunc)middle_apply_action, NULL);


   /* Exit & Update */
   button = gtk_button_new_with_label("Save & Apply & Exit");
   gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 4);
   gtk_signal_connect(GTK_OBJECT(button), "pressed",
		      (GtkSignalFunc)middle_save_apply_exit_action, NULL);
   
   unselect_row((GtkCList*)NULL , 0, 0,
		(GdkEventButton *) NULL, (gpointer) NULL) ;

   /* Speed Config */
   frame =   xbindkeys_config_speed_config(0);
   gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);


   /* Warning */
   frame =  gtk_frame_new ("Warning :");
   gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);
   
   label_warning =  gtk_label_new ("\n \n"); 
   gtk_container_add(GTK_CONTAINER(frame), label_warning);
   

   /* End */
   return panedH;
}

/*****************************************************************************/
GtkWidget * xbindkeys_config_middle_list(gboolean show)
{
  GtkWidget *src;
  int i;

  src = gtk_scrolled_window_new ( NULL, NULL );

  gtk_widget_set_usize(src,550,400);
  
  clist = gtk_clist_new_with_titles (  TITLE_NB_ITEMS, titles );

  gtk_clist_set_selection_mode (GTK_CLIST(clist), GTK_SELECTION_BROWSE);
  gtk_clist_set_reorderable(GTK_CLIST(clist), TRUE);

  gtk_container_add (GTK_CONTAINER(src),  clist);
  
  if(!show){
    gtk_signal_connect(GTK_OBJECT(clist), "select-row",
		       (GtkSignalFunc)select_row, NULL);
    gtk_signal_connect(GTK_OBJECT(clist), "unselect-row",
		       (GtkSignalFunc)unselect_row, NULL);
  } else {
    /* For dont display a bug */
    Flag_NumLock = gtk_check_button_new_with_label ("NumLock");
    Flag_ScrollLock = gtk_check_button_new_with_label ("ScrollLock");
    Flag_CapsLock = gtk_check_button_new_with_label ("CapsLock");
  }
  
  for(i=0; i< TITLE_NB_ITEMS; i++){
   gtk_clist_column_title_passive (GTK_CLIST(clist), i);
  }
  
  for(i=0; i<TITLE_NB_ITEMS; i++)
    gtk_clist_set_column_width(GTK_CLIST(clist), i, titles_width[i]);
  
  return src;
}


/*****************************************************************************/
gboolean middle_get_key(GtkWidget *parent, void *data)
{
  FILE *f;

  /* pl1 et pl2 is the line of key (Bad rem)*/
  char buf[500], buf2[100], *pl1, *pl2;
  char *buf3[50];
  int len, i;

  if(ROW == -1) return FALSE;

  system("killall -9 xbindkeys" );
  if(!(f = popen( XBINDKEYS_PATCH " -k", "r"))){
    perror("opening " XBINDKEYS_PATCH );
    return FALSE;
  }

  if(f){
    len= fread( buf, sizeof(char), 500, f );

    /* printf("%s\n", buf ); */


    /* find line beginning by " */
    pl1=buf; 
    while ( pl1[0]!='\"' ){
      pl1 = strchr (pl1, '\n')+1;
      if(!pl1) {
	printf("erreur in get line \"NoCommand\"");
	return FALSE; /* error in get the line "NoCommand" */
      }
    }; 

    /* pl1 is the next line */
    pl1 = strchr(pl1, '\n')+5;
    if(!pl1) {
      printf("erreur in get line \"Pl1\"");
	return FALSE; /* error in get the line 3 */
      }

    /* pl2 is the next line */
    pl2 = strchr(pl1, '\n')+5;
    if(!pl1) {
      printf("erreur in get line \"Pl2\"");
	return FALSE; /* error in get the line 4" */
      }


    /* change the \n to 0 */
    for(i=0; i<len; i++)
      if(buf[i]=='\n') buf[i]=0;

    /* update */
    sprintf( buf2, "%s | %s", pl2, pl1);

    for(i=0; gtk_clist_get_text (GTK_CLIST(clist), i, 1, buf3); i++)
      if(i!=ROW)
	if(!strcmp(buf3[0], buf2)){
	  sprintf( buf2, "Key Used");
	  gtk_label_set_text (GTK_LABEL(label_warning),
			      "The line with\n\"Key Used\"\nis not Saved");
	}

    gtk_entry_set_text (GTK_ENTRY(entry_key), buf2);

    /* end */
    fclose(f);
    restart_xbindkeys();
    return TRUE;
  }
  else{
    perror("middle_get_key:");
    return FALSE;
  }
}


/*****************************************************************************/
gboolean middle_run_action(GtkWidget *parent, void *data)
{
  char buf[1024];
  
  sprintf(buf, "%s &", gtk_entry_get_text(GTK_ENTRY(entry_act)));

  system(buf);
  return TRUE;
}

/*****************************************************************************/
void new_file()
{
  gtk_clist_clear (GTK_CLIST(clist));
  unselect_row(NULL, 0, 0, NULL, NULL);
}


/*****************************************************************************/
void read_file(char file_in[])
{
  gtk_clist_clear (GTK_CLIST(clist));

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(Flag_NumLock), FALSE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(Flag_ScrollLock), FALSE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(Flag_CapsLock), FALSE);

  append_file(file_in);
  gtk_clist_select_row (GTK_CLIST(clist), 0, 1);
}

/*****************************************************************************/
void append_file(char file_in[])
{
  char linep [1024], *pp;   /* line -1         */
  char line  [1024];        /* line current    */
  char line2 [1024], *p2;   /* next line       */
  char line3 [1024], *p3;   /* next next line  */

  char * pos1;
  char * pos2;
  char *text[TITLE_NB_ITEMS];
  int i;

  FILE *f;

  if(!(f = fopen(file_in, "r"))){
    perror("open file");
    return;
  }

  gtk_clist_freeze(GTK_CLIST(clist));

  while (fgets (line, sizeof(line), f)){

    /* test if is a keystate */
    pos1 = strchr (line, 'k');
    if (pos1){
      pos2 = strchr (line, '#');
      if (! pos2 || pos2 > pos1){
	pos1=strchr(line, '=');
	if(pos1++){
	  for(pos1++;  *pos1==' ' || *pos1=='\b'; pos1++);
	  for(pp=line; *pp==' ' || *pp=='\b'; pp++);
	  if(*pp=='k'){
	    if(strncmp (pp, "keystate_numlock",16))
	      if(strncmp (pos1-1, "enable",6))
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(Flag_NumLock), TRUE);
	    if(strncmp (pp, "keystate_capslock",17))
	      if(strncmp (pos1-1, "enable",6))
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(Flag_CapsLock), TRUE);
	    if(strncmp (pp, "keystate_scrolllock",19))
	      if(strncmp (pos1-1, "enable",6))
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(Flag_ScrollLock), TRUE); 
	  }
	}
      }
    }

    /* test is is a command */
    pos1 = strchr (line, '"');
    if (pos1){
      pos2 = strchr (line, '#');
      if (! pos2 || pos2 > pos1){
	pos2 = strrchr (line, '"');
	if (pos2 && pos1 < pos2){
	  
	  /* get line 1 */
	  *(pos2)=0; pos1++; 
	  
	  /* get line 2 */
	  fgets (line2, sizeof(line), f);
	  for(i=strlen(line2)-1; 
	      line2[i]==' ' || line2[i]=='\t' || line2[i]==10; i--) 
	    line2[i]=0;
	  for(i=0; line2[i]==' ' || line2[i]=='\t'; i++);
	  p2=line2+i;
	  
	  /* get line 3 */
	  fgets (line3, sizeof(line), f);
	  for(i=strlen(line3)-1; 
	      line3[i]==' ' || line3[i]=='\t' || line3[i]==10; i--) 
	    line3[i]=0;
	  for(i=0; line3[i]==' ' || line3[i]=='\t'; i++);
	  p3=line3+i;
	  
	  /* get line p */
	  for(i=strlen(linep)-1; 
	      linep[i]==' ' || linep[i]=='\t' || linep[i]==10; i--) 
	    linep[i]=0;
	  for(i=0; linep[i]==' ' || linep[i]=='\t'; i++);
	  pp=linep+i+1;  /* +1 to del '#' */
	  
	  text[0] = g_strdup_printf("%s", pp);
	  if(*p3 && *p2) text[1] = g_strdup_printf("%s | %s", p3, p2);
	  if(*p3 && !*p2) text[1] = g_strdup_printf("%s", p3);
	  if(!*p3 && *p2) text[1] = g_strdup_printf("%s", p2);
	  text[2] = g_strdup_printf("%s", pos1);
	  gtk_clist_append(GTK_CLIST(clist), text);
	  for(i=0; i<TITLE_NB_ITEMS; i++)
	    g_free(text[i]);
	  strcpy(line, "     ");
	}
      }
    }
    strcpy(linep, line);
  }
  gtk_clist_thaw(GTK_CLIST(clist));
}

/*****************************************************************************/
void view_generated_file()
{
  FILE *f;
  GtkWidget *window;
  GtkWidget *text;
  GtkWidget *src;
  char line  [1024];

  unlink(TEMP_FILE); 
  save_file(TEMP_FILE);

  if((f = fopen(TEMP_FILE, "r"))){
    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_window_set_title(GTK_WINDOW(window), "Generated File");
    src = gtk_scrolled_window_new ( NULL, NULL );
    gtk_widget_set_usize(src,500,400);
    text = gtk_text_new (NULL, NULL);
    gtk_container_add (GTK_CONTAINER(src),  text);

    while (fgets (line, sizeof(line), f))
      gtk_text_insert (text, NULL, NULL, NULL,
		       line, strlen(line));

    gtk_container_add(GTK_CONTAINER(window),src);
    gtk_widget_show_all(window);
    fclose(f);
  }
}

/*****************************************************************************/
void  save_file(char file_out[])
{
  FILE *f;
  char *buf[200], buf2[200], *p2;
  int i;

  if(!(f = fopen(file_out, "w"))){
    perror("write file");
    return;
  }

  /*f = stdout;  for debuging */
  
  fprintf(f, "###########################\n");
  fprintf(f, "# xbindkeys configuration #\n");
  fprintf(f, "###########################\n#\n");
  fprintf(f, "# Version: " XBINDKEYS_CONFIG_VERSION "\n#\n");
  fprintf(f, "# If you edit this, do not forget to uncomment any lines that you change.\n");
  fprintf(f, "# The pound(#) symbol may be used anywhere for comments.\n#\n");
  fprintf(f, "# A list of keys is in /usr/include/X11/keysym.h and in\n");
  fprintf(f, "# /usr/include/X11/keysymdef.h \n");
  fprintf(f, "# The XK_ is not needed. \n#\n");
  fprintf(f, "# List of modifier (on my keyboard): \n");
  fprintf(f, "#   Control, Shift, Mod1 (Alt), Mod2 (NumLock), \n");
  fprintf(f, "#   Mod3 (CapsLock), Mod4, Mod5 (Scroll). \n#\n");
  fprintf(f, "# Another way to specifie a key is to use 'xev' and set the \n");
  fprintf(f, "# keycode with c:nnn or the modifier with m:nnn where nnn is \n");
  fprintf(f, "# the keycode or the state returned by xev \n#\n");
  fprintf(f, "# This file is created by xbindkey_config \n");
  fprintf(f, "# The structure is : \n");
  fprintf(f, "# # Remark \n");
  fprintf(f, "# \"command\" \n");
  fprintf(f, "# m:xxx + c:xxx \n");
  fprintf(f, "# Shift+... \n\n\n\n\n");

  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Flag_NumLock)))
    fprintf(f,"keystate_numlock = enable\n");
  else 
    fprintf(f,"#keystate_numlock = enable\n");

  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Flag_ScrollLock)))
    fprintf(f,"keystate_scrolllock = enable\n");
  else 
    fprintf(f,"#keystate_scrolllock = enable\n");

  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Flag_CapsLock)))
    fprintf(f,"keystate_capslock = enable\n");
  else 
    fprintf(f,"#keystate_capslock = enable\n");

   fprintf(f,"\n\n\n");

  for(i=0; gtk_clist_get_text (GTK_CLIST(clist), i, 1, buf); i++)
    if(strcmp("Key | Key", buf[0]) && strcmp("Key Used", buf[0])){

      gtk_clist_get_text (GTK_CLIST(clist), i, 0, buf);
      if(buf[0][0]) fprintf(f, "#%s\n", buf[0]);
      
      gtk_clist_get_text (GTK_CLIST(clist), i, 2, buf);	   
      fprintf(f, "\"%s\"\n", buf[0]);
      
      gtk_clist_get_text (GTK_CLIST(clist), i, 1, buf);
      strcpy(buf2, buf[0]);
      
      p2 = strchr (buf2, '|');
      
      if(p2){
	*p2=0; p2++;
	fprintf(f,"   %s\n    %s\n\n", p2, buf2);
      }
      else{
	fprintf(f,"   %s\n\n", buf2);
      }
    }
  fprintf(f, "#\n");
  fprintf(f, "# End of xbindkeys configuration\n");
  fclose(f);
}


/*****************************************************************************/
void select_row (GtkCList *clist, gint row, gint column,
                 GdkEventButton *event, gpointer user_data)
{
  char *buf[1024];

  ROW=row;
  allow_refresh_gtk_entry = FALSE;

  if(gtk_clist_get_text (GTK_CLIST(clist), row, 0, buf)){
    gtk_entry_set_text (GTK_ENTRY(entry_rem), buf[0]);
    strcpy(button_undo_rem, buf[0]);
    gtk_entry_set_editable (GTK_ENTRY(entry_rem), TRUE);
    gtk_widget_set_sensitive (GTK_WIDGET(entry_rem), TRUE);
    gtk_widget_grab_focus (GTK_WIDGET(entry_rem));
    
    gtk_clist_get_text (GTK_CLIST(clist), row, 1, buf);
    gtk_entry_set_text (GTK_ENTRY(entry_key), buf[0]);
    strcpy(button_undo_key, buf[0]); 
    gtk_entry_set_editable (GTK_ENTRY(entry_key), TRUE);
    gtk_widget_set_sensitive (GTK_WIDGET(entry_key), TRUE);
    
    gtk_clist_get_text (GTK_CLIST(clist), row, 2, buf);
    gtk_entry_set_text (GTK_ENTRY(entry_act), buf[0]);
    strcpy(button_undo_act, buf[0]);
    allow_refresh_gtk_entry = TRUE;
    gtk_entry_set_editable (GTK_ENTRY(entry_act), TRUE);
    gtk_widget_set_sensitive (GTK_WIDGET(entry_act), TRUE);
    
    gtk_widget_set_sensitive (GTK_WIDGET(button_del_sel), TRUE);
    gtk_widget_set_sensitive (GTK_WIDGET(button_cp_item), TRUE);
    gtk_widget_set_sensitive (GTK_WIDGET(button_up), TRUE);
    gtk_widget_set_sensitive (GTK_WIDGET(button_down), TRUE);
    
    gtk_widget_set_sensitive (GTK_WIDGET(button_get_key), TRUE);
    gtk_widget_set_sensitive (GTK_WIDGET(button_run_act), TRUE);
  }
}

/*****************************************************************************/
void unselect_row(GtkCList *clist, gint row, gint column,
		    GdkEventButton *event, gpointer user_data)
{

  ROW=-1;
  gtk_entry_set_text (GTK_ENTRY(entry_rem), "");
  gtk_entry_set_editable (GTK_ENTRY(entry_rem), FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET(entry_rem), FALSE);

  gtk_entry_set_text (GTK_ENTRY(entry_key), "");
  gtk_entry_set_editable (GTK_ENTRY(entry_key), FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET(entry_key), FALSE);

  gtk_entry_set_text (GTK_ENTRY(entry_act), "");
  gtk_entry_set_editable (GTK_ENTRY(entry_act), FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET(entry_act), FALSE);

  gtk_widget_set_sensitive (GTK_WIDGET(button_del_sel), FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET(button_cp_item), FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET(button_up), FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET(button_down), FALSE);

  gtk_widget_set_sensitive (GTK_WIDGET(button_get_key), FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET(button_run_act), FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET(button_undo), FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET(button_undo_menu), FALSE);
}

/*****************************************************************************/
void clist_set_text  (GtkButton *button, gpointer user_data)
{
  if(allow_refresh_gtk_entry){
    gtk_clist_set_text (GTK_CLIST(clist), ROW, 0,
			gtk_entry_get_text (GTK_ENTRY(entry_rem)));

    gtk_clist_set_text (GTK_CLIST(clist), ROW, 1, 
			gtk_entry_get_text (GTK_ENTRY(entry_key)));

    gtk_clist_set_text (GTK_CLIST(clist), ROW, 2,
			gtk_entry_get_text (GTK_ENTRY(entry_act)));
    
    if((!strcmp(gtk_entry_get_text (GTK_ENTRY(entry_rem)), button_undo_rem) &
       !strcmp(gtk_entry_get_text (GTK_ENTRY(entry_key)), button_undo_key)) &
       !strcmp(gtk_entry_get_text (GTK_ENTRY(entry_act)), button_undo_act)){
      gtk_widget_set_sensitive (GTK_WIDGET(button_undo), FALSE);
      gtk_widget_set_sensitive (GTK_WIDGET(button_undo_menu), FALSE);
    }
    else{
      gtk_widget_set_sensitive (GTK_WIDGET(button_undo), TRUE);
      gtk_widget_set_sensitive (GTK_WIDGET(button_undo_menu), TRUE);
      }  
  }
  
}

/*****************************************************************************/
void middle_delete_item(GtkWidget *parent, void *data)
{
  char *buf[1024];

  if(gtk_clist_get_text (GTK_CLIST(clist), ROW+1, 0, buf)){
      gtk_clist_remove (GTK_CLIST(clist), ROW);
      gtk_clist_select_row (GTK_CLIST(clist), ROW, 1);
  }
  else
    if(gtk_clist_get_text (GTK_CLIST(clist), ROW-1, 0, buf)){
      gtk_clist_select_row (GTK_CLIST(clist), ROW-1, 1);
      gtk_clist_remove (GTK_CLIST(clist), ROW+1);
    }
    else{
      unselect_row(NULL, 0, 0, NULL, NULL);
      gtk_clist_remove (GTK_CLIST(clist), 0);
      unselect_row(NULL, 0, 0, NULL, NULL);
    }

  middle_clist_set_visible();  
}

/*****************************************************************************/
void middle_append_item(GtkWidget *parent, void *data)
{
  char *text[TITLE_NB_ITEMS];
  int i;
  char *buf[1024];

  text[0] = g_strdup_printf("Remark");
  text[1] = g_strdup_printf("Key | Key");
  text[2] = g_strdup_printf("Command");

  gtk_label_set_text (GTK_LABEL(label_warning),
		      "The line with\n\"Key | Key\"\nis not Saved");


  gtk_clist_append(GTK_CLIST(clist), text);

  for(i=0; gtk_clist_get_text (GTK_CLIST(clist), i, 0, buf); i++)
    ROW=i;
  gtk_clist_select_row (GTK_CLIST(clist), ROW, 1);
  middle_clist_set_visible();

  for(i=0; i<TITLE_NB_ITEMS; i++)
    g_free(text[i]);

}

/*****************************************************************************/
void middle_copy_item(GtkWidget *parent, void *data)
{
  char *text[TITLE_NB_ITEMS];
  int i;
  char *buf[50];

  gtk_clist_get_text (GTK_CLIST(clist), ROW, 0, buf);
  text[0] = g_strdup_printf("%s-Copy",buf[0]);

  text[1] = g_strdup_printf("Key | Key");
  gtk_label_set_text (GTK_LABEL(label_warning),
		      "The line with\n\"Key | Key\"\nis not Saved");

  gtk_clist_get_text (GTK_CLIST(clist), ROW, 2, buf);
  text[2] = g_strdup_printf("%s",buf[0]);

  gtk_clist_insert (GTK_CLIST(clist), ROW+1, text);

  for(i=0; i<TITLE_NB_ITEMS; i++)
    g_free(text[i]);

  gtk_clist_select_row (GTK_CLIST(clist), ++ROW, 1);
  middle_clist_set_visible();

}

/*****************************************************************************/
void middle_up_item(GtkWidget *parent, void *data)
{
  if(ROW>0){
    gtk_clist_swap_rows (GTK_CLIST(clist), ROW, ROW -1);
    ROW--;
    middle_clist_set_visible();
  }
}

/*****************************************************************************/
void middle_down_item(GtkWidget *parent, void *data)
{
  char *buf[1024];

  if( gtk_clist_get_text (GTK_CLIST(clist), ROW+1, 0, buf) && ROW!=-1 ){
    gtk_clist_swap_rows (GTK_CLIST(clist), ROW+1, ROW);
    ROW++;
    middle_clist_set_visible();
  }
}

/*****************************************************************************/
void main_exit()
{
  g_print("Exit\n");
  exit(0);

  unlink(TEMP_FILE); 
}

/*****************************************************************************/
void middle_save_apply_exit_action(GtkWidget *parent, void *data)
{
  char buf[1024];
  sprintf(buf, "%s/.xbindkeysrc", getenv ("HOME"));

  save_file(buf);
  
  system("killall -HUP xbindkeys");
  //usleep(500);
  //system("xbindkeys&");

  main_exit();
}

/*****************************************************************************/
void middle_apply_action(GtkWidget *parent, void *data)
{

  unlink(TEMP_FILE); 
  save_file(TEMP_FILE);
  system("killall -9 xbindkeys");
  usleep(500);
  /* printf("****\n\noutput = %d\n\n****",system("xbindkeys -f " TEMP_FILE )); */
  system("xbindkeys -f " TEMP_FILE );
}

/*****************************************************************************/
void selectable_clist(gboolean state)
{
  char *buf[1024];
  int i;
  for(i=0; gtk_clist_get_text (GTK_CLIST(clist), i, 0, buf); i++)
    gtk_clist_set_selectable(GTK_CLIST(clist), i, state);
}

/*****************************************************************************/
void middle_clist_set_visible()
{
/*  if(!(gtk_clist_row_is_visible(GTK_CLIST(clist), ROW)==GTK_VISIBILITY_NONE)) */
    gtk_clist_moveto(GTK_CLIST(clist), ROW, 0, 0.5, 0.5);
		   
}

/*****************************************************************************/
void undo()
{
  gtk_entry_set_text (GTK_ENTRY(entry_rem), button_undo_rem);
  gtk_entry_set_text (GTK_ENTRY(entry_key), button_undo_key);
  gtk_entry_set_text (GTK_ENTRY(entry_act), button_undo_act);
}

/*****************************************************************************/
void set_default()
{
  char *text[TITLE_NB_ITEMS];
  int i;

  gtk_clist_clear (GTK_CLIST(clist));

  text[0] = g_strdup_printf("key win");
  text[1] = g_strdup_printf("Mod2 + Menu |  m:0x10 + c:117");
  text[2] = g_strdup_printf("xbindkeys_show");
  gtk_clist_append(GTK_CLIST(clist), text);
  for(i=0; i<TITLE_NB_ITEMS; i++)
    g_free(text[i]);

  text[0] = g_strdup_printf("Alt-Echap");
  text[1] = g_strdup_printf("Alt+Mod2 + Escape | m:0x18 + c:9");
  text[2] = g_strdup_printf("xbindkeys_config");
  gtk_clist_append(GTK_CLIST(clist), text);
  for(i=0; i<TITLE_NB_ITEMS; i++)
    g_free(text[i]);

  text[0] = g_strdup_printf("Ctrl-Shift-x");
  text[1] = g_strdup_printf("Control+Shift+Mod2 + x | m:0x15 + c:53");
  text[2] = g_strdup_printf("xterm");
  gtk_clist_append(GTK_CLIST(clist), text);
  for(i=0; i<TITLE_NB_ITEMS; i++)
  g_free(text[i]);

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(Flag_NumLock), FALSE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(Flag_CapsLock), FALSE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(Flag_ScrollLock), FALSE);
  
}


/*****************************************************************************/
void restart_xbindkeys()
{
  system( XBINDKEYS_PATCH );
}
