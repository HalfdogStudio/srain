#include <gtk/gtk.h>

GtkStyleProvider *provider;

void theme_init(){
    provider = GTK_STYLE_PROVIDER(gtk_css_provider_new());
    gtk_css_provider_load_from_path(
            GTK_CSS_PROVIDER(provider),"./summer_rain.css", NULL);
}

#define _G_MAXUINT -1   // YCM can not found the defintion of G_MAXUINT, help him
void theme_apply(GtkWidget *widget){
    gtk_style_context_add_provider(
            gtk_widget_get_style_context(widget), provider, _G_MAXUINT);

    if(GTK_IS_CONTAINER(widget))
        gtk_container_forall(GTK_CONTAINER(widget), (GtkCallback)theme_apply, NULL);
}