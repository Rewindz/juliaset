#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <gtk/gtk.h>

#include "colours.h"

typedef struct{
  double x;
  double y;
}Vec2;

Vec2 _c = {-0.7, 0.27015};
double _R = 2.0;
double _adjustment = 0.1;

int is_in_julia_set(Vec2 *coords, Vec2 *size, Vec2 *c, double *R, int max_iterations, double *smooth_t)
{
  double zx = (2.0 * *R * coords->x / size->x) - *R;
  double zy = (2.0 * *R * coords->y / size->y) - *R;
  int i = 0;
  for(; i < max_iterations; ++i){
    double xtemp = zx * zx - zy * zy;
    zy = 2 * zx * zy + c->y;
    zx = xtemp + c->x;
    if(zx*zx + zy*zy < (*R)*(*R))
      break;
  }
  double mag = sqrt(zx * zx + zy * zy);
  *smooth_t = i + 1 - log(log(mag)) / log(2.0);
  return i;
}


void draw_handler(GtkDrawingArea* drawing_area, cairo_t* cr, int width, int height, gpointer user_data)
{
  Vec2 size = {width, height};
  
  int max_iterations = 1000;

  GdkRGBA color = {0};
  color.alpha = 1.0;
  
  for(int lx = 0; lx < width; ++lx){
    for(int ly = 0; ly < height; ++ly){
      double smooth_t = 0;
      Vec2 pos = {lx, ly};
      int result = is_in_julia_set(&pos, &size, &_c, &_R, max_iterations, &smooth_t);

      double t = smooth_t / max_iterations; 
      
      if(result == max_iterations){
	color.red = 1.0;
	color.green = 1.0;
	color.blue = 1.0;
      }else{
        simple_rgb(t, &color.red, &color.green, &color.blue);
      }
      gdk_cairo_set_source_rgba(cr, &color);
      cairo_rectangle(cr, lx, ly, 1, 1);
      cairo_fill(cr);  
    }
  }
}

void julia_set_title(GtkWindow *window)
{
  char title[100];
  //char *title;
  //title = malloc(sizeof(char)*100);
  sprintf(title, "Julia Set |R: %g| |C.X: %g| |C.Y: %g| |Adjustment: %g|", _R, _c.x, _c.y, _adjustment);
  gtk_window_set_title(window, title);
  //free(title);
}

gboolean window_key_pressed (GtkEventControllerKey* event, guint keyval, guint keycode, GdkModifierType state, gpointer user_data)
{
  switch(keyval){
  case GDK_KEY_1:
    _c.x -= _adjustment;
    break;
  case GDK_KEY_2:
    _c.x += _adjustment;
    break;
  case GDK_KEY_3:
    _c.y -= _adjustment;
    break;
  case GDK_KEY_4:
    _c.y += _adjustment;
    break;
  case GDK_KEY_5:
    _R -= _adjustment;
    break;
  case GDK_KEY_6:
    _R += _adjustment;
    break;
  case GDK_KEY_8:
    _adjustment -= 0.001;
    break;
  case GDK_KEY_9:
    _adjustment += 0.001;
    break;
  case GDK_KEY_0:
    _R = 2.0;
    _c.x = -0.7;
    _c.y = 0.27015;
    break;
  default:
    return FALSE;
  }

  GtkWindow *window = GTK_WINDOW(gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(event)));

  julia_set_title(window);
  
  gtk_widget_queue_draw(gtk_window_get_child(window));
    
  return TRUE;
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
  gtk_widget_grab_focus(GTK_WIDGET(draw_area));

  GtkEventControllerKey *key_event_ctrl = gtk_event_controller_key_new();
  g_signal_connect(key_event_ctrl, "key-pressed", G_CALLBACK(window_key_pressed), NULL);
  gtk_widget_add_controller(GTK_WIDGET(window), GTK_EVENT_CONTROLLER(key_event_ctrl));

  julia_set_title(GTK_WINDOW(window));

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
