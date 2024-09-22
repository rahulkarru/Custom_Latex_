#include "latex_editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILE_SIZE 10000
void show_message(GtkWidget *parent, const char *message);


char processed_output[MAX_FILE_SIZE];

void on_open_file_clicked(GtkWidget *widget, gpointer text_view) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Open File",
                                                    GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Open", GTK_RESPONSE_ACCEPT, NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        FILE *file = fopen(filename, "r");

        if (file != NULL) {
            char buffer[MAX_FILE_SIZE];
            size_t n = fread(buffer, 1, sizeof(buffer), file);
            buffer[n] = '\0';  
            fclose(file);

            GtkTextBuffer *buffer_view = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
            gtk_text_buffer_set_text(buffer_view, buffer, -1);
            show_message(GTK_WIDGET(gtk_widget_get_toplevel(widget)), "File loaded successfully.");
        } else {
            show_message(GTK_WIDGET(gtk_widget_get_toplevel(widget)), "Failed to open file.");
        }
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

void on_save_file_clicked(GtkWidget *widget, gpointer text_view) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Save File",
                                                    GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                                                    GTK_FILE_CHOOSER_ACTION_SAVE,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Save", GTK_RESPONSE_ACCEPT, NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        GtkTextBuffer *buffer_view = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer_view, &start, &end);
        char *latex_code = gtk_text_buffer_get_text(buffer_view, &start, &end, FALSE);

        
        FILE *file = fopen(filename, "w");
        if (file != NULL) {
            fputs(latex_code, file);
            fclose(file);
            show_message(GTK_WIDGET(gtk_widget_get_toplevel(widget)), "File saved successfully.");
        } else {
            show_message(GTK_WIDGET(gtk_widget_get_toplevel(widget)), "Failed to save file.");
        }
        g_free(filename);
        g_free(latex_code);
    }
    gtk_widget_destroy(dialog);
}

void on_process_and_save_latex_clicked(GtkWidget *widget, gpointer text_view) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Save Processed LaTeX",
                                                    GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                                                    GTK_FILE_CHOOSER_ACTION_SAVE,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Save", GTK_RESPONSE_ACCEPT, NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

       
        GtkTextBuffer *buffer_view = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer_view, &start, &end);
        char *latex_code = gtk_text_buffer_get_text(buffer_view, &start, &end, FALSE);

       
        memset(processed_output, 0, sizeof(processed_output));

      
        process_latex_to_output(latex_code, processed_output, MAX_FILE_SIZE);

       
        FILE *file = fopen(filename, "w");
        if (file != NULL) {
            fputs(processed_output, file);
            fclose(file);
            show_message(GTK_WIDGET(gtk_widget_get_toplevel(widget)), "Processed LaTeX saved successfully.");
        } else {
            show_message(GTK_WIDGET(gtk_widget_get_toplevel(widget)), "Failed to save processed file.");
        }
        g_free(filename);
        g_free(latex_code);
    }
    gtk_widget_destroy(dialog);
}

void show_message(GtkWidget *parent, const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
void on_check_errors_clicked(GtkWidget *widget, gpointer text_view) {
    GtkTextBuffer *buffer_view = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer_view, &start, &end);
    char *latex_code = gtk_text_buffer_get_text(buffer_view, &start, &end, FALSE);

    int open_braces = 0, close_braces = 0;
    for (int i = 0; latex_code[i] != '\0'; i++) {
        if (latex_code[i] == '{') open_braces++;
        if (latex_code[i] == '}') close_braces++;
    }

    if (open_braces != close_braces) {
        show_message(GTK_WIDGET(gtk_widget_get_toplevel(widget)), "Error: Unmatched braces {}.");
    } else {
        show_message(GTK_WIDGET(gtk_widget_get_toplevel(widget)), "No LaTeX syntax errors found.");
    }
    g_free(latex_code);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

 
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "LaTeX Editor with Custom Parser");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);


    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);


    GtkWidget *scroll_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(vbox), scroll_window, TRUE, TRUE, 0);

    GtkWidget *text_view = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(scroll_window), text_view);


    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);


    GtkWidget *open_button = gtk_button_new_with_label("Open File");
    gtk_box_pack_start(GTK_BOX(hbox), open_button, TRUE, TRUE, 0);
    g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_file_clicked), text_view);

    GtkWidget *save_button = gtk_button_new_with_label("Save File");
    gtk_box_pack_start(GTK_BOX(hbox), save_button, TRUE, TRUE, 0);
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_file_clicked), text_view);

    GtkWidget *error_button = gtk_button_new_with_label("Check Errors");
    gtk_box_pack_start(GTK_BOX(hbox), error_button, TRUE, TRUE, 0);
    g_signal_connect(error_button, "clicked", G_CALLBACK(on_check_errors_clicked), text_view);

    GtkWidget *process_button = gtk_button_new_with_label("Process LaTeX");
    gtk_box_pack_start(GTK_BOX(hbox), process_button, TRUE, TRUE, 0);
    g_signal_connect(process_button, "clicked", G_CALLBACK(on_process_and_save_latex_clicked), text_view);

    gtk_widget_show_all(window);

    gtk_main();
    return 0;
}
