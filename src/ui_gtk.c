#include <gtk/gtk.h>
#include <string.h>
#include "shell.h"
#include "log.h"

// Variables globales para componentes de la GUI
static GtkTextBuffer *BUF = NULL;     // Buffer del terminal de salida
static GtkWidget *ENTRY = NULL;       // Campo de entrada de comandos
static GtkWidget *STATUS = NULL;      // Barra de estado
static GtkWidget *SCROLLED_WIN = NULL; // Ventana con scroll para auto-scroll

/* ---------- Utilidades GUI ---------- */

// Función para enviar texto al terminal de la GUI
static void gui_sink(const char *text) {
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(BUF, &end);
    gtk_text_buffer_insert(BUF, &end, text, -1);

    // Auto-scroll hacia abajo cuando se agrega nuevo texto
    GtkTextMark *mark = gtk_text_buffer_get_insert(BUF);
    gtk_text_buffer_place_cursor(BUF, &end);
    GtkTextView *text_view = GTK_TEXT_VIEW(gtk_bin_get_child(GTK_BIN(SCROLLED_WIN)));
    gtk_text_view_scroll_mark_onscreen(text_view, mark);
}

// Actualiza el mensaje de la barra de estado
static void set_status(const char *msg) {
    gtk_label_set_text(GTK_LABEL(STATUS), msg);
}

// Ejecuta un comando y muestra su salida en el terminal
static void run_command_str(const char *cmd) {
    if (!cmd || !*cmd) return;

    // Mostrar el comando en el terminal con formato
    gui_sink("\n🔹 CinnamStrawbOS> ");
    gui_sink(cmd);
    gui_sink("\n");

    char line[1024];
    g_strlcpy(line, cmd, sizeof line);
    int quit = shell_handle_line(line);
    if (quit) gtk_main_quit();
}

// Callback para el botón ejecutar
static void on_execute_clicked(GtkWidget *btn, gpointer) {
    (void)btn;
    const char *cmd = gtk_entry_get_text(GTK_ENTRY(ENTRY));
    run_command_str(cmd);
    gtk_entry_set_text(GTK_ENTRY(ENTRY), ""); // Limpiar entrada
}

// Callback para Enter en el campo de entrada
static gboolean on_entry_activate(GtkEntry *e, gpointer) {
    (void)e;
    on_execute_clicked(NULL, NULL);
    return TRUE;
}

/* ---- Callbacks para botones de acceso rápido ---- */

// Botones de la barra de herramientas para comandos frecuentes
static void on_btn_help(GtkWidget*, gpointer)   { run_command_str("Ayuda"); set_status("📖 Mostrando ayuda"); }
static void on_btn_listp(GtkWidget*, gpointer)  { run_command_str("ListarProcesos"); set_status("⚡ Listando procesos"); }
static void on_btn_memmap(GtkWidget*, gpointer) { run_command_str("MostrarMapaMemoria"); set_status("💾 Mostrando mapa de memoria"); }
static void on_btn_listfs(GtkWidget*, gpointer) { run_command_str("ListarArchivos"); set_status("📂 Listando archivos"); }
static void on_btn_savefs(GtkWidget*, gpointer) { run_command_str("GuardarFS"); set_status("💾 VFS guardado"); }
static void on_btn_loadfs(GtkWidget*, gpointer) { run_command_str("CargarFS"); set_status("📁 VFS cargado"); }

// Callback para limpiar el terminal
static void on_btn_clear(GtkWidget*, gpointer) {
    gtk_text_buffer_set_text(BUF, "", -1);
    set_status("🧹 Terminal limpiado");
}

/* -------------------- MAIN -------------------- */

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

    /* CSS moderno para una interfaz atractiva y minimalista */
    const char *css =
        "/* Tipografía global */\n"
        "* { font-family: 'SF Pro Display', 'Segoe UI', 'Roboto', sans-serif; }\n"
        "\n"
        "/* Ventana principal con gradiente suave */\n"
        "window {\n"
        "    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);\n"
        "    color: #ffffff;\n"
        "}\n"
        "\n"
        "/* Header bar elegante */\n"
        "#header {\n"
        "    background: rgba(255, 255, 255, 0.1);\n"
        "    backdrop-filter: blur(10px);\n"
        "    border-bottom: 1px solid rgba(255, 255, 255, 0.2);\n"
        "    color: #ffffff;\n"
        "    font-weight: 600;\n"
        "}\n"
        "\n"
        "/* Toolbar con efecto glass */\n"
        "#toolbar {\n"
        "    background: rgba(255, 255, 255, 0.08);\n"
        "    backdrop-filter: blur(8px);\n"
        "    border-bottom: 1px solid rgba(255, 255, 255, 0.1);\n"
        "    padding: 12px;\n"
        "}\n"
        "\n"
        "/* Botones modernos con efectos */\n"
        "button {\n"
        "    background: rgba(255, 255, 255, 0.15);\n"
        "    color: #ffffff;\n"
        "    border: 1px solid rgba(255, 255, 255, 0.2);\n"
        "    border-radius: 12px;\n"
        "    padding: 8px 16px;\n"
        "    margin: 0 4px;\n"
        "    font-weight: 500;\n"
        "    transition: all 0.3s ease;\n"
        "}\n"
        "\n"
        "button:hover {\n"
        "    background: rgba(255, 255, 255, 0.25);\n"
        "    border-color: rgba(255, 255, 255, 0.4);\n"
        "    transform: translateY(-1px);\n"
        "    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.2);\n"
        "}\n"
        "\n"
        "button:active {\n"
        "    transform: translateY(0px);\n"
        "    background: rgba(255, 255, 255, 0.2);\n"
        "}\n"
        "\n"
        "/* Terminal de salida */\n"
        "#terminal {\n"
        "    background: rgba(0, 0, 0, 0.4);\n"
        "    color: #00ff88;\n"
        "    font-family: 'Consolas', 'Fira Code', monospace;\n"
        "    font-size: 13px;\n"
        "    border-radius: 8px;\n"
        "    margin: 8px;\n"
        "    border: 1px solid rgba(255, 255, 255, 0.1);\n"
        "}\n"
        "\n"
        "/* Campo de entrada elegante */\n"
        "entry {\n"
        "    background: rgba(255, 255, 255, 0.1);\n"
        "    color: #ffffff;\n"
        "    border: 1px solid rgba(255, 255, 255, 0.2);\n"
        "    border-radius: 8px;\n"
        "    padding: 10px 15px;\n"
        "    font-size: 14px;\n"
        "}\n"
        "\n"
        "entry:focus {\n"
        "    border-color: rgba(255, 255, 255, 0.4);\n"
        "    background: rgba(255, 255, 255, 0.15);\n"
        "    box-shadow: 0 0 0 2px rgba(255, 255, 255, 0.1);\n"
        "}\n"
        "\n"
        "/* Barra de estado */\n"
        "#status {\n"
        "    color: rgba(255, 255, 255, 0.8);\n"
        "    padding: 8px 15px;\n"
        "    font-size: 12px;\n"
        "    font-weight: 500;\n"
        "}\n"
        "\n"
        "/* Scrollbars personalizadas */\n"
        "scrollbar {\n"
        "    background: rgba(255, 255, 255, 0.1);\n"
        "    border-radius: 6px;\n"
        "}\n"
        "\n"
        "scrollbar slider {\n"
        "    background: rgba(255, 255, 255, 0.3);\n"
        "    border-radius: 6px;\n"
        "}\n"
        "\n"
        "scrollbar slider:hover {\n"
        "    background: rgba(255, 255, 255, 0.5);\n"
        "}\n";
    GtkCssProvider *prov = gtk_css_provider_new();
    gtk_css_provider_load_from_data(prov, css, -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(prov), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(prov);

    /* Ventana principal con diseño moderno */
    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), "CinnamStrawbOS");
    gtk_window_set_default_size(GTK_WINDOW(win), 1200, 800);
    gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(win), root);

    /* Header bar moderno */
    GtkWidget *header = gtk_header_bar_new();
    gtk_widget_set_name(header, "header");
    gtk_header_bar_set_title(GTK_HEADER_BAR(header), "🍓 CinnamStrawbOS");
    gtk_header_bar_set_subtitle(GTK_HEADER_BAR(header), "Sistema Operativo Educativo");
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
    gtk_box_pack_start(GTK_BOX(root), header, FALSE, FALSE, 0);

    /* Toolbar con botones organizados en grupos */
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_name(toolbar, "toolbar");
    gtk_widget_set_halign(toolbar, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(root), toolbar, FALSE, FALSE, 0);

    // Grupo Sistema
    GtkWidget *b_help   = gtk_button_new_with_label("📖 Ayuda");
    GtkWidget *b_clear  = gtk_button_new_with_label("🧹 Limpiar");

    // Grupo Procesos
    GtkWidget *b_listp  = gtk_button_new_with_label("⚡ Procesos");
    GtkWidget *b_memmap = gtk_button_new_with_label("💾 Memoria");

    // Grupo Archivos
    GtkWidget *b_listfs = gtk_button_new_with_label("📂 Archivos");
    GtkWidget *b_savefs = gtk_button_new_with_label("💾 Guardar");
    GtkWidget *b_loadfs = gtk_button_new_with_label("📁 Cargar");

    // Conectar callbacks
    g_signal_connect(b_help,   "clicked", G_CALLBACK(on_btn_help),   NULL);
    g_signal_connect(b_clear,  "clicked", G_CALLBACK(on_btn_clear),  NULL);
    g_signal_connect(b_listp,  "clicked", G_CALLBACK(on_btn_listp),  NULL);
    g_signal_connect(b_memmap, "clicked", G_CALLBACK(on_btn_memmap), NULL);
    g_signal_connect(b_listfs, "clicked", G_CALLBACK(on_btn_listfs), NULL);
    g_signal_connect(b_savefs, "clicked", G_CALLBACK(on_btn_savefs), NULL);
    g_signal_connect(b_loadfs, "clicked", G_CALLBACK(on_btn_loadfs), NULL);

    // Organizar botones en grupos visuales
    gtk_box_pack_start(GTK_BOX(toolbar), b_help,   FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), b_clear,  FALSE, FALSE, 0);

    // Separador visual
    GtkWidget *sep1 = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
    gtk_box_pack_start(GTK_BOX(toolbar), sep1, FALSE, FALSE, 8);

    gtk_box_pack_start(GTK_BOX(toolbar), b_listp,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), b_memmap, FALSE, FALSE, 0);

    // Separador visual
    GtkWidget *sep2 = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
    gtk_box_pack_start(GTK_BOX(toolbar), sep2, FALSE, FALSE, 8);

    gtk_box_pack_start(GTK_BOX(toolbar), b_listfs, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), b_savefs, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), b_loadfs, FALSE, FALSE, 0);

    /* Terminal de salida con diseño moderno */
    SCROLLED_WIN = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_name(SCROLLED_WIN, "terminal");
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(SCROLLED_WIN),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(SCROLLED_WIN), GTK_SHADOW_NONE);

    GtkWidget *tv = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(tv), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(tv), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tv), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(tv), 15);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(tv), 15);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(tv), 15);
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(tv), 15);

    gtk_container_add(GTK_CONTAINER(SCROLLED_WIN), tv);
    gtk_box_pack_start(GTK_BOX(root), SCROLLED_WIN, TRUE, TRUE, 0);
    BUF = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));

    /* Barra inferior moderna con entrada de comandos */
    GtkWidget *bottom_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(root), bottom_container, FALSE, FALSE, 0);

    // Barra de estado superior
    STATUS = gtk_label_new("🚀 Sistema listo - Escribe un comando o usa los botones");
    gtk_widget_set_name(STATUS, "status");
    gtk_widget_set_halign(STATUS, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(bottom_container), STATUS, FALSE, FALSE, 0);

    // Contenedor para entrada de comandos
    GtkWidget *input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(input_box), 12);
    gtk_box_pack_start(GTK_BOX(bottom_container), input_box, FALSE, FALSE, 0);

    // Etiqueta prompt
    GtkWidget *prompt_label = gtk_label_new("🔹");
    gtk_widget_set_valign(prompt_label, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(input_box), prompt_label, FALSE, FALSE, 0);

    // Campo de entrada mejorado
    ENTRY = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ENTRY), "Escribe un comando (Enter para ejecutar)");
    gtk_widget_set_size_request(ENTRY, -1, 40);
    g_signal_connect(ENTRY, "activate", G_CALLBACK(on_entry_activate), NULL);

    // Botón ejecutar con icono
    GtkWidget *btn = gtk_button_new_with_label("▶ Ejecutar");
    gtk_widget_set_size_request(btn, 100, 40);
    g_signal_connect(btn, "clicked", G_CALLBACK(on_execute_clicked), NULL);

    gtk_box_pack_start(GTK_BOX(input_box), ENTRY, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(input_box), btn, FALSE, FALSE, 0);

    /* Configuración inicial del sistema */
    set_output(gui_sink);
    set_output_mode(LOG_MODE_GUI);

    // Mostrar mensaje de bienvenida en el terminal
    shell_init();
    gui_sink("\n🌟 ¡Bienvenido a CinnamStrawbOS!\n");
    gui_sink("✨ Usa los botones de arriba o escribe comandos abajo\n");
    gui_sink("📖 Escribe 'Ayuda' para ver todos los comandos disponibles\n\n");

    // Enfocar el campo de entrada al iniciar
    gtk_widget_grab_focus(ENTRY);

    gtk_widget_show_all(win);
    gtk_main();
    return 0;
}

