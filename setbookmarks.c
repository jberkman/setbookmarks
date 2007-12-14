#include "config.h"

#include <glib.h>

#include <errno.h>

#ifdef NEED_GBOOKMARK
#include "libslab-bookmarkfile.h"
#include "libslab-bookmarkfile.c"
#include "bookmark-agent-libslab.h"
#endif

int
main (int argc, char *argv[])
{
    int rv;
    GError *error = NULL;
    char *progname;
    char *file;
    char *dirname;
    char *url;
    GDir *dir;
    GBookmarkFile *bm;
    GKeyFile *kf;
    char *name;

    progname = g_strrstr (argv[0], G_DIR_SEPARATOR_S);
    progname = progname ? progname + 1 : argv[0];

    if (argc != 2) {
        g_printerr ("Usage: %s <directory>\n"
                    "<directory> should be a directory containing .desktop files.\n",
                    progname);
        return 1;
    }

    dirname = argv[1];

    dir = g_dir_open (dirname, 0, &error);
    if (!dir) {
        g_printerr ("%s: Could not open %s: %s.\n",
                    progname, dirname, error->message);
        g_error_free (error);
        return 1;
    }

    bm = g_bookmark_file_new ();
    kf = g_key_file_new ();

    for (file = (char *)g_dir_read_name (dir); file; file = (char *)g_dir_read_name (dir)) {
        if (!g_str_has_suffix (file, ".desktop")) {
            g_printerr ("%s: %s is not a .desktop file; skipping.\n", progname, file);
            continue;
        }

        name = NULL;
        url = NULL;
        error = NULL;

        file = g_build_filename (dirname, file, NULL);

        if (!g_key_file_load_from_file (kf, file, G_KEY_FILE_NONE, &error)) {
            g_printerr ("%s: Error parsing %s: %s; skipping.\n",
                        progname, file, error->message);
            goto next;
        }

        url = g_strconcat ("file://", file, NULL);

        g_bookmark_file_set_mime_type (bm, url, "application/x-desktop");

        name = g_key_file_get_locale_string (kf, "Desktop Entry", "Name", NULL, &error);
        if (name) {
            g_bookmark_file_set_title (bm, url, name);
        }

        g_bookmark_file_add_application (bm, url, PACKAGE_NAME, progname);

    next:
        g_free (name);
        g_free (url);
        g_free (file);
        if (error) {
            g_error_free (error);
        }
    }
    g_dir_close (dir);

    file = g_build_filename (g_get_user_data_dir (), "gnome-main-menu", NULL);
    if (g_mkdir_with_parents (file, 0700)) {
        g_printerr ("%s: Could not create bookmarks directory %s: %s.\n",
                    progname, file, g_strerror (errno));
        g_free (file);
        return 1;
    }
    g_free (file);

    file = g_build_filename (g_get_user_data_dir (), "gnome-main-menu", "applications.xbel", NULL);
    if (!g_bookmark_file_to_file (bm, file, &error)) {
        g_printerr ("%s: Could not save bookmarks to %s: %s.\n",
                    progname, file, error->message);
        g_free (file);
        g_error_free (error);
        return 1;
    }

    g_print ("%s: Wrote %d entries to %s.\n", progname, g_bookmark_file_get_size (bm), file);

    return 0;
}
