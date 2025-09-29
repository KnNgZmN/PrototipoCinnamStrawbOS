#include <gtk/gtk.h>
#include <string.h>
#include "shell.h"
#include "log.h"

static GtkTextBuffer *BUF = NULL;
static GtkWidget *ENTRY = NULL;
static GtkWidget *STATUS = NULL;

/* ---------- Utilidades GUI ---------- */

static void gui_sink(const char *text) {
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(BUF, &end);
    gtk_text_buffer_insert(BUF, &end, text, -1);
    // autoscroll
    GtkTextMark *mark = gtk_text_buffer_create_mark(BUF, NULL, &end, FALSE);
    GtkWidget *tv = gtk_widget_get_parent(gtk_widget_get_parent(gtk_widget_get_toplevel(GTK_WIDGET(ENTRY))));
    (void)tv; // (no necesario, solo evita warning)
}

static void set_status(const char *msg) {
    gtk_label_set_text(GTK_LABEL(STATUS), msg);
}

static void run_command_str(const char *cmd) {
    if (!cmd || !*cmd) return;
    gui_sink("CinnamStrawbOS> "); gui_sink(cmd); gui_sink("\n");

    char line[1024];
    g_strlcpy(line, cmd, sizeof line);
    int quit = shell_handle_line(line);
    if (quit) gtk_main_quit();
}

static void on_execute_clicked(GtkWidget *btn, gpointer) {
    (void)btn;
    const char *cmd = gtk_entry_get_text(GTK_ENTRY(ENTRY));
    run_command_str(cmd);
    gtk_entry_set_text(GTK_ENTRY(ENTRY), "");
}

static gboolean on_entry_activate(GtkEntry *e, gpointer) {
    (void)e;
    on_execute_clicked(NULL, NULL);
    return TRUE;
}

/* ---- Botones de barra de herramientas (comandos rápidos) ---- */

static void on_btn_help(GtkWidget*, gpointer){ run_command_str("Ayuda"); set_status("Ayuda"); }
static void on_btn_listp(GtkWidget*, gpointer){ run_command_str("ListarProcesos"); set_status("Procesos activos"); }
static void on_btn_memmap(GtkWidget*, gpointer){ run_command_str("MostrarMapaMemoria"); set_status("Mapa de memoria"); }
static void on_btn_savefs(GtkWidget*, gpointer){ run_command_str("GuardarFS"); set_status("VFS guardado"); }
static void on_btn_loadfs(GtkWidget*, gpointer){ run_command_str("CargarFS"); set_status("VFS cargado"); }

/* -------------------- MAIN -------------------- */

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

    /* CSS simple para colores y estilos */
    const char *css =
        "* { font-family: 'Consolas', 'Cascadia Mono', monospace; }\n"
        "window { background: #0f1226; }\n"
        "#header { background: #1b2040; color: #e5e7ff; }\n"
        "#toolbar { background: #141833; }\n"
        "#status { color: #a7b0ff; padding: 6px 10px; }\n"
        "textview { color: #dfe3ff; }\n"
        "entry { color: #e8eaff; background: #10142c; }\n"
        "button { background: #20264d; color: #e8eaff; border-radius: 8px; padding: 6px 10px; }\n"
        "button:hover { background: #2a3268; }\n";
    GtkCssProvider *prov = gtk_css_provider_new();
    gtk_css_provider_load_from_data(prov, css, -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(prov), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(prov);

    /* Ventana principal con HeaderBar */
    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), "CinnamStrawbOS (GUI)");
    gtk_window_set_default_size(GTK_WINDOW(win), 980, 640);
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(win), root);

    GtkWidget *header = gtk_header_bar_new();
    gtk_widget_set_name(header, "header");
    gtk_header_bar_set_title(GTK_HEADER_BAR(header), "CinnamStrawbOS");
    gtk_header_bar_set_subtitle(GTK_HEADER_BAR(header), "Prototipo de SO con GUI");
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
    gtk_box_pack_start(GTK_BOX(root), header, FALSE, FALSE, 0);

    /* Toolbar de acciones */
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_set_name(toolbar, "toolbar");
    gtk_container_set_border_width(GTK_CONTAINER(toolbar), 6);
    gtk_box_pack_start(GTK_BOX(root), toolbar, FALSE, FALSE, 0);

    GtkWidget *b_help   = gtk_button_new_with_label("Ayuda");
    GtkWidget *b_listp  = gtk_button_new_with_label("Procesos");
    GtkWidget *b_memmap = gtk_button_new_with_label("Mapa Memoria");
    GtkWidget *b_savefs = gtk_button_new_with_label("Guardar FS");
    GtkWidget *b_loadfs = gtk_button_new_with_label("Cargar FS");

    g_signal_connect(b_help,   "clicked", G_CALLBACK(on_btn_help),   NULL);
    g_signal_connect(b_listp,  "clicked", G_CALLBACK(on_btn_listp),  NULL);
    g_signal_connect(b_memmap, "clicked", G_CALLBACK(on_btn_memmap), NULL);
    g_signal_connect(b_savefs, "clicked", G_CALLBACK(on_btn_savefs), NULL);
    g_signal_connect(b_loadfs, "clicked", G_CALLBACK(on_btn_loadfs), NULL);

    gtk_box_pack_start(GTK_BOX(toolbar), b_help,   FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), b_listp,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), b_memmap, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), b_savefs, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), b_loadfs, FALSE, FALSE, 0);

    /* Área central: TextView con scroll (salida) */
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    GtkWidget *tv = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(tv), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(tv), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tv), GTK_WRAP_WORD_CHAR);
    gtk_container_add(GTK_CONTAINER(scroll), tv);
    gtk_box_pack_start(GTK_BOX(root), scroll, TRUE, TRUE, 0);
    BUF = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));

    /* Barra inferior: Entry + Ejecutar + Estado */
    GtkWidget *bottom = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_container_set_border_width(GTK_CONTAINER(bottom), 6);
    gtk_box_pack_start(GTK_BOX(root), bottom, FALSE, FALSE, 0);

    ENTRY = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ENTRY), "Escribe un comando (Enter para ejecutar)...");
    GtkWidget *btn = gtk_button_new_with_label("Ejecutar");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_execute_clicked), NULL);
    g_signal_connect(ENTRY, "activate", G_CALLBACK(on_entry_activate), NULL);

    gtk_box_pack_start(GTK_BOX(bottom), ENTRY, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(bottom), btn,   FALSE, FALSE, 0);

    STATUS = gtk_label_new("Listo");
    gtk_widget_set_name(STATUS, "status");
    gtk_box_pack_end(GTK_BOX(bottom), STATUS, FALSE, FALSE, 0);

    /* Atajos */
    gtk_widget_add_accelerator(btn, "clicked",
        gtk_accel_group_new(), GDK_KEY_Return, 0, GTK_ACCEL_VISIBLE);

    /* Redirige salida y arranca shell */
    set_output(gui_sink);
    set_output_mode(LOG_MODE_GUI);
    shell_init();

    gtk_widget_show_all(win);
    gtk_main();
    return 0;
}

