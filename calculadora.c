#include <gtk/gtk.h>
#include <string.h>
#include <ctype.h>

static void al_presionar_boton(GtkWidget *widget, gpointer data) {
    GtkEntry *pantalla = GTK_ENTRY(data);
    const gchar *texto_boton = gtk_button_get_label(GTK_BUTTON(widget));
    const gchar *texto_actual = gtk_entry_get_text(pantalla);
    
    int longitud = strlen(texto_actual);
    char caracter_entrante = texto_boton[0];
    if (strcmp(texto_actual, "Syntax error") == 0) {
        texto_actual = ""; 
    }

    if (longitud > 0) {
        char ultimo_caracter = texto_actual[longitud - 1];
        if (strchr("+-*/,)", caracter_entrante) != NULL && strchr("+-*/,", ultimo_caracter) != NULL) {
            return;
        }
    } else {
        if (strchr("*/,)", caracter_entrante) != NULL) {
            return;
        }
    }

    gchar *nuevo_texto = g_strdup_printf("%s%s", texto_actual, texto_boton);
    gtk_entry_set_text(pantalla, nuevo_texto);
    g_free(nuevo_texto);
}

static const char *p_expr;
static int error_sintaxis;

static double evaluar_expresion();

static double evaluar_factor() {
    while (isspace(*p_expr)) p_expr++;
    
    if (*p_expr == '+' || *p_expr == '-') {
        int signo = (*p_expr++ == '+') ? 1 : -1;
        return signo * evaluar_factor();
    }
    
    if (*p_expr == '(') {
        p_expr++;
        double res = evaluar_expresion();
        if (*p_expr == ')') p_expr++;
        else error_sintaxis = 1;
        return res;
    }
    
    if (isdigit(*p_expr) || *p_expr == '.') {
        char *fin;
        double res = g_ascii_strtod(p_expr, &fin);
        if (p_expr == fin) error_sintaxis = 1;
        p_expr = fin;
        return res;
    }
    
    error_sintaxis = 1;
    return 0;
}

static double evaluar_termino() {
    double res = evaluar_factor();
    while (1) {
        while (isspace(*p_expr)) p_expr++;
        if (*p_expr == '*') {
            p_expr++;
            res *= evaluar_factor();
        } else if (*p_expr == '/') {
            p_expr++;
            double div = evaluar_factor();
            if (div == 0.0) error_sintaxis = 1; 
            else res /= div;
        } else {
            break;
        }
    }
    return res;
}

static double evaluar_expresion() {
    double res = evaluar_termino();
    while (1) {
        while (isspace(*p_expr)) p_expr++;
        if (*p_expr == '+') {
            p_expr++;
            res += evaluar_termino();
        } else if (*p_expr == '-') {
            p_expr++;
            res -= evaluar_termino();
        } else {
            break;
        }
    }
    return res;
}

static void al_presionar_borrar_todo(GtkWidget *widget, gpointer data) {
    GtkEntry *pantalla = GTK_ENTRY(data);
    gtk_entry_set_text(pantalla, "");
}

static void al_presionar_borrar(GtkWidget *widget, gpointer data) {
    GtkEntry *pantalla = GTK_ENTRY(data);
    const gchar *texto_actual = gtk_entry_get_text(pantalla);
    int longitud = strlen(texto_actual);

    if (longitud > 0) {
        gchar *nuevo_texto = g_strndup(texto_actual, longitud - 1);
        gtk_entry_set_text(pantalla, nuevo_texto);
        g_free(nuevo_texto);
    }
}

static void al_presionar_igual(GtkWidget *widget, gpointer data) {
    GtkEntry *pantalla = GTK_ENTRY(data);
    const gchar *texto_actual = gtk_entry_get_text(pantalla);
    
    if (strlen(texto_actual) == 0 || strcmp(texto_actual, "Syntax error") == 0) {
        return;
    }

    gchar *copia = g_strdup(texto_actual);
    for(int i = 0; copia[i]; i++) {
        if(copia[i] == ',') copia[i] = '.';
    }

    p_expr = copia;
    error_sintaxis = 0;
    
    double resultado = evaluar_expresion();
    while(isspace(*p_expr)) p_expr++;
    if (*p_expr != '\0') error_sintaxis = 1;

    if (error_sintaxis) {
        gtk_entry_set_text(pantalla, "Syntax error");
    } else {
        gchar *texto_resultado = g_strdup_printf("%g", resultado);
        // Volvemos a cambiar puntos por comas en el resultado visual
        for(int i = 0; texto_resultado[i]; i++) {
            if(texto_resultado[i] == '.') texto_resultado[i] = ',';
        }
        gtk_entry_set_text(pantalla, texto_resultado);
        g_free(texto_resultado);
    }
    g_free(copia);
}

static void activate(GtkApplication* app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *caja_vertical;
    GtkWidget *pantalla;
    GtkWidget *grid;        
    GtkWidget *boton_1, *boton_2, *boton_3, *boton_mas, *boton_4, *boton_5, *boton_6, *boton_menos, *boton_7, *boton_8, *boton_9, *boton_multiplicacion, *boton_0, *boton_division, *boton_coma, *boton_igual, *boton_abre_parentesis, *boton_cierra_parentesis, *boton_borrar_todo, *boton_borrar;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Mi Calculadora");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 500);

    caja_vertical = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), caja_vertical);

    pantalla = gtk_entry_new();
    gtk_editable_set_editable(GTK_EDITABLE(pantalla), FALSE);
    gtk_box_pack_start(GTK_BOX(caja_vertical), pantalla, FALSE, FALSE, 10);
    
    grid = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(caja_vertical), grid, TRUE, TRUE, 10);

    boton_1 = gtk_button_new_with_label("1");
    boton_2 = gtk_button_new_with_label("2");
    boton_3 = gtk_button_new_with_label("3");
    boton_mas = gtk_button_new_with_label("+");
    boton_4 = gtk_button_new_with_label("4");
    boton_5 = gtk_button_new_with_label("5");
    boton_6 = gtk_button_new_with_label("6");
    boton_menos = gtk_button_new_with_label("-");
    boton_7 = gtk_button_new_with_label("7");
    boton_8 = gtk_button_new_with_label("8");
    boton_9 = gtk_button_new_with_label("9");
    boton_multiplicacion = gtk_button_new_with_label("*");
    boton_0 = gtk_button_new_with_label("0");
    boton_division = gtk_button_new_with_label("/");
    boton_coma = gtk_button_new_with_label(",");
    boton_igual = gtk_button_new_with_label("=");
    boton_abre_parentesis = gtk_button_new_with_label("(");
    boton_cierra_parentesis = gtk_button_new_with_label(")");
    boton_borrar_todo = gtk_button_new_with_label("C");
    boton_borrar = gtk_button_new_with_label("⌫");

    gtk_grid_attach(GTK_GRID(grid), boton_1, 0, 0, 1, 1); 
    gtk_grid_attach(GTK_GRID(grid), boton_2, 1, 0, 1, 1); 
    gtk_grid_attach(GTK_GRID(grid), boton_3, 2, 0, 1, 1); 
    gtk_grid_attach(GTK_GRID(grid), boton_mas, 3, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), boton_4, 0, 1, 1, 1); 
    gtk_grid_attach(GTK_GRID(grid), boton_5, 1, 1, 1, 1); 
    gtk_grid_attach(GTK_GRID(grid), boton_6, 2, 1, 1, 1); 
    gtk_grid_attach(GTK_GRID(grid), boton_menos, 3, 1, 1, 1); 
    gtk_grid_attach(GTK_GRID(grid), boton_7, 0, 2, 1, 1); 
    gtk_grid_attach(GTK_GRID(grid), boton_8, 1, 2, 1, 1); 
    gtk_grid_attach(GTK_GRID(grid), boton_9, 2, 2, 1, 1); 
    gtk_grid_attach(GTK_GRID(grid), boton_multiplicacion, 3, 2, 1, 1); 
    gtk_grid_attach(GTK_GRID(grid), boton_0, 0, 3, 1, 1); 
    gtk_grid_attach(GTK_GRID(grid), boton_division, 1, 3, 1, 1); 
    gtk_grid_attach(GTK_GRID(grid), boton_coma, 2, 3, 1, 1); 
    gtk_grid_attach(GTK_GRID(grid), boton_igual, 3, 3, 1, 1); 
    gtk_grid_attach(GTK_GRID(grid), boton_abre_parentesis, 0, 4, 1, 1); 
    gtk_grid_attach(GTK_GRID(grid), boton_cierra_parentesis, 1, 4, 1, 1); 
    gtk_grid_attach(GTK_GRID(grid), boton_borrar_todo, 2, 4, 1, 1); 
    gtk_grid_attach(GTK_GRID(grid), boton_borrar, 3, 4, 1, 1); 

    g_signal_connect(boton_0, "clicked", G_CALLBACK(al_presionar_boton), pantalla);
    g_signal_connect(boton_1, "clicked", G_CALLBACK(al_presionar_boton), pantalla);
    g_signal_connect(boton_2, "clicked", G_CALLBACK(al_presionar_boton), pantalla);
    g_signal_connect(boton_3, "clicked", G_CALLBACK(al_presionar_boton), pantalla);
    g_signal_connect(boton_4, "clicked", G_CALLBACK(al_presionar_boton), pantalla);
    g_signal_connect(boton_5, "clicked", G_CALLBACK(al_presionar_boton), pantalla);
    g_signal_connect(boton_6, "clicked", G_CALLBACK(al_presionar_boton), pantalla);
    g_signal_connect(boton_7, "clicked", G_CALLBACK(al_presionar_boton), pantalla);
    g_signal_connect(boton_8, "clicked", G_CALLBACK(al_presionar_boton), pantalla);
    g_signal_connect(boton_9, "clicked", G_CALLBACK(al_presionar_boton), pantalla);
    g_signal_connect(boton_mas, "clicked", G_CALLBACK(al_presionar_boton), pantalla);
    g_signal_connect(boton_menos, "clicked", G_CALLBACK(al_presionar_boton), pantalla);
    g_signal_connect(boton_multiplicacion, "clicked", G_CALLBACK(al_presionar_boton), pantalla);
    g_signal_connect(boton_division, "clicked", G_CALLBACK(al_presionar_boton), pantalla);
    g_signal_connect(boton_coma, "clicked", G_CALLBACK(al_presionar_boton), pantalla);
    g_signal_connect(boton_abre_parentesis, "clicked", G_CALLBACK(al_presionar_boton), pantalla);
    g_signal_connect(boton_cierra_parentesis, "clicked", G_CALLBACK(al_presionar_boton), pantalla);
    g_signal_connect(boton_borrar_todo, "clicked", G_CALLBACK(al_presionar_borrar_todo), pantalla);
    g_signal_connect(boton_borrar, "clicked", G_CALLBACK(al_presionar_borrar), pantalla);
    g_signal_connect(boton_igual, "clicked", G_CALLBACK(al_presionar_igual), pantalla);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;
    
    app = gtk_application_new("org.gtk.calculadora", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}