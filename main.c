#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>

void draw_handler(GtkDrawingArea* drawing_area, cairo_t* cr, int width, int height, gpointer user_data)
{
  cairo_rectangle(cr, 50, 50, 50, 50);
  cairo_fill(cr);
}


static void app_activate(GtkApplication *app, gpointer user_data)
{
  GtkBuilder *builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "../builder.ui", NULL);
  if(!builder){
    fprintf(stderr, "Failed to load builder.ui!\n");
    abort();
  }
  
  GObject *window = gtk_builder_get_object(builder, "window");
  if(!window){
    fprintf(stderr, "Failed to get window from builder!\n");
    abort();
  }

  GObject *draw_area = gtk_builder_get_object(builder, "drawarea");
  if(!draw_area){
    fprintf(stderr, "Failed to get draw area!\n");
    abort();
  }
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(draw_area), draw_handler, user_data, NULL);
  
  gtk_window_set_application(GTK_WINDOW(window), app);


  gtk_widget_set_visible(GTK_WIDGET(window), TRUE);

  g_object_unref(builder);
  
}


int main(int argc, char **argv)
{
  int status;
  GtkApplication *app = gtk_application_new("xyz.rewindz.Julia", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  
  return status;
}
