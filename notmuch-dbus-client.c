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
#include <assert.h>

#include <glib.h>
#include <gio/gio.h>

int
main (int argc, char *argv[])
{
    GOptionContext *context;
    GError *error = NULL;
    GDBusProxy *proxy;
    GVariant *result;
    guint32 result_uint32;

    g_type_init ();

    context = g_option_context_new (NULL);
    g_option_context_set_summary (context,
	    "The notmuch mail system - DBus daemon.");
    if (!g_option_context_parse (context, &argc, &argv, &error)) {
	g_error_free (error);
    }
    g_option_context_free (context);

    error = NULL;
    proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SESSION,
	    G_DBUS_PROXY_FLAGS_NONE,
	    NULL,
	    NOTMUCH_DBUS_NAME,
	    NOTMUCH_DBUS_OBJECT,
	    NOTMUCH_DBUS_INTERFACE,
	    NULL,
	    &error);
    if (error)
	g_print ("proxy connection failed\n");
    else
	g_print ("proxy connection succeeded\n");

    result = g_dbus_proxy_get_cached_property (proxy,
	    "database_version");
    if (result != NULL) {
	g_print ("got property \"database_version\" from proxy\n");
	g_variant_get (result, "(u)", &result_uint32);
	g_print ("result is: %i\n", result_uint32);
	g_print ("\n");
    }
    else {
	g_print ("failed to get property from proxy\n");
	g_print ("\n");
    }

    result = NULL;
    error = NULL;
    result = g_dbus_proxy_call_sync (proxy,
	    "database_get_version",
	    NULL,
	    G_DBUS_CALL_FLAGS_NONE,
	    -1,
	    NULL,
	    &error);


    if (error) {
	g_print ("call failed\n");
	g_print("error: %s\n", error->message);
	exit (EXIT_FAILURE);
    }
    else
	g_print ("call succeeded\n");

    g_print ("result is of type: %s\n", g_variant_get_type_string (result));

    g_variant_get (result, "(u)", &result_uint32);
    g_print ("result is: %i\n", result_uint32);

    g_object_unref (proxy);

    return EXIT_SUCCESS;
}
