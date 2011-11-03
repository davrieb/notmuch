/* notmuch - Not much of an email program, (just index and search)
 *
 * Copyright © 2009 Carl Worth
 * Copyright © 2009 Keith Packard
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/ .
 *
 * Authors: Carl Worth <cworth@cworth.org>
 *	    Keith Packard <keithp@keithp.com>
 */

#include "notmuch-client.h"
#include "notmuch-dbus.h"

#include <stdlib.h>

#include <glib.h>
#include <gio/gio.h>


#define DAEMON_TIMEOUT 1200


static notmuch_database_t *notmuch;


static void notmuch_dbus_open_database ()
{
    notmuch_config_t *config;
    gchar *db_path;

    void *local = talloc_new (NULL);
    config = notmuch_config_open (
	    local, NULL, NULL);
    db_path = talloc_asprintf (local, "%s",
	    notmuch_config_get_database_path (config));
    notmuch_config_close (config);

    g_print("notmuch database path is: %s\n", db_path);

    notmuch = notmuch_database_open (db_path,
	    NOTMUCH_DATABASE_MODE_READ_WRITE);
    g_assert (notmuch);

    talloc_free (db_path);
}

static gboolean
notmuch_dbus_daemon_timeout_cb (gpointer user_data) {
    GMainLoop *main_loop = (GMainLoop *)user_data;

    g_main_loop_quit (main_loop);

    return TRUE;
}

static void notmuch_dbus_reset_daemon_quit_timeout (GMainLoop *main_loop)
{
    static GMainLoop *main_loop_internal;
    static guint daemon_timeout_source_id;

    if (main_loop != NULL)
	main_loop_internal = main_loop;

    if (daemon_timeout_source_id > 0) {
	g_source_remove (daemon_timeout_source_id);
    }

    daemon_timeout_source_id =
	g_timeout_add_seconds ( DAEMON_TIMEOUT,
		&notmuch_dbus_daemon_timeout_cb,
		main_loop_internal);

    g_print ("reset daemon_quit_timeout\n");
}

static void
notmuch_dbus_handle_method_call (GDBusConnection *connection,
	const gchar *sender,
	const gchar *object_path,
	const gchar *interface_name,
	const gchar *method_name,
	GVariant *unused (parameters),
	GDBusMethodInvocation *invocation,
	gpointer unused (user_data))
{
    GVariant *return_value;
    g_assert (connection);

    g_print ("Got method call.\n");
    g_print ("sender: %s\n", sender);
    g_print ("object_path: %s\n", object_path);
    g_print ("interface_name: %s\n", interface_name);
    g_print ("method_name: %s\n", method_name);

    return_value = g_variant_new ("(u)", 0);
    g_dbus_method_invocation_return_value (invocation, return_value);

    notmuch_dbus_reset_daemon_quit_timeout (NULL);
}

static GVariant *
notmuch_dbus_handle_get_property (GDBusConnection  *unused (connection),
	const gchar *unused (sender),
	const gchar *unused (object_path),
	const gchar *unused (interface_name),
	const gchar *unused (property_name),
	GError **unused (error),
	gpointer unused (user_data))
{
    g_print ("Got request for property.\n");
    g_print ("sender: %s\n", sender);
    g_print ("object_path: %s\n", object_path);
    g_print ("interface_name: %s\n", interface_name);
    g_print ("property_name: %s\n", property_name);

    notmuch_dbus_reset_daemon_quit_timeout (NULL);

    if (g_strcmp0 (property_name, "database_version") == 0) {
	guint32 database_version;

	database_version = notmuch_database_get_version (notmuch);
	return  g_variant_new ("(u)", database_version);
    }

    g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
	    "Property %s doesn't exist here", property_name);
    return NULL;
}

static gboolean
notmuch_dbus_handle_set_property (GDBusConnection  *unused (connection),
	const gchar *unused (sender),
	const gchar *unused (object_path),
	const gchar *unused (interface_name),
	const gchar *unused (property_name),
	GVariant *unused (value),
	GError **unused (error),
	gpointer unused (user_data))
{
    return FALSE;
}

static gchar*
notmuch_dbus_read_introspection_xml (const char* path)
{
    gchar *xml;
    gsize lenght;
    GError *error;

    g_assert (path);

    error = NULL;
    if (!g_file_get_contents (path, &xml, &lenght, &error)) {
	exit(EXIT_FAILURE);
    }

    return xml;
}

static void
notmuch_dbus_ping_cb (GDBusConnection * unused(connection),
	const gchar *sender_name,
	const gchar * unused(object_path),
	const gchar *interface_name,
	const gchar *signal_name,
	GVariant * unused(parameters),
	gpointer unused (user_data))
{
    g_print ("received signal \"%s.%s\" from %s\n", interface_name, signal_name, sender_name);
};

static void
notmuch_dbus_name_lost_cb (GDBusConnection * connection,
	const gchar *name,
	gpointer unused (user_data))
{
    g_assert (connection);

    g_print ("Lost the name %s on the session bus.\n", name);
}

static void
notmuch_dbus_name_acquired_cb (GDBusConnection * connection,
	const gchar *name,
	gpointer unused (user_data))
{
    g_assert (connection);

    g_print ("Acquired the name %s on the session bus.\n", name);
}

static void
notmuch_dbus_bus_acquired_cb (GDBusConnection *connection,
	const gchar *name,
	gpointer user_data)
{
    gchar *xml;
    GError *error;
    GDBusNodeInfo *node_info;
    GDBusInterfaceVTable interface_vtable;
    GDBusInterfaceInfo *interface_info;

    g_assert (connection);
    g_assert (name);

    g_print ("Acquired the bus\n");

    error = NULL;
    xml = notmuch_dbus_read_introspection_xml (NOTMUCH_DBUS_INTROSPECTION_XML);
    node_info = g_dbus_node_info_new_for_xml (xml, &error);
    g_print ("Read node info with path: %s\n", node_info->path);
    if (error) {
	g_print ("Reading instrospection data failed\n");
        exit (EXIT_FAILURE);
    } else
	g_print ("Reading instrospection data succeeded\n");

    interface_vtable.method_call = notmuch_dbus_handle_method_call;
    interface_vtable.set_property = notmuch_dbus_handle_set_property;
    interface_vtable.get_property = notmuch_dbus_handle_get_property;

    interface_info = g_dbus_node_info_lookup_interface (node_info,
	    NOTMUCH_DBUS_INTERFACE);

    error = NULL;
    g_dbus_connection_register_object (connection,
	    NOTMUCH_DBUS_OBJECT,
	    interface_info,
	    &interface_vtable,
	    NULL, NULL, NULL);
    if (error) {
	g_print ("Registering objects failed\n");
        exit (EXIT_FAILURE);
    } else
	g_print ("Registering objects succeeded\n");

    g_free (xml);
    g_dbus_node_info_unref (node_info);

    g_dbus_connection_signal_subscribe (connection,
	    NULL,
	    NOTMUCH_DBUS_INTERFACE,
	    "ping",
	    NOTMUCH_DBUS_OBJECT,
	    NULL,
	    G_DBUS_SIGNAL_FLAGS_NONE,
	    &notmuch_dbus_ping_cb, user_data, NULL);

    notmuch_dbus_open_database ();
}

int
main (int argc, char *argv[])
{
    GOptionContext *context;
    GError *error = NULL;
    GMainLoop *main_loop;
    guint owner_id;

    g_type_init ();

    context = g_option_context_new (NULL);
    g_option_context_set_summary (context,
	    "The notmuch mail system - DBus daemon.");
    if (!g_option_context_parse(context, &argc, &argv, &error)) {
	g_error_free (error);
    }
    g_option_context_free (context);

    /* Create a new event loop to run in */
    main_loop = g_main_loop_new (NULL, FALSE);

    owner_id = g_bus_own_name (G_BUS_TYPE_SESSION, NOTMUCH_DBUS_NAME,
	     G_BUS_NAME_OWNER_FLAGS_NONE,
	     &notmuch_dbus_bus_acquired_cb,
	     &notmuch_dbus_name_acquired_cb,
	     &notmuch_dbus_name_lost_cb,
	     NULL,
	     NULL);

    notmuch_dbus_reset_daemon_quit_timeout (main_loop);

    g_main_loop_run (main_loop);

    notmuch_database_close (notmuch);
    g_bus_unown_name (owner_id);

    return EXIT_SUCCESS;
}
