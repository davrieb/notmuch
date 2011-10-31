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

#include <stdlib.h>

#include <glib.h>
#include <gio/gio.h>


static void
notmuch_dbus_name_acquired_cb (GDBusConnection * connection,
	const gchar *name,
	gpointer user_data)
{
    GMainLoop *main_loop = (GMainLoop*)user_data;

    g_assert (connection);

    g_print ("Acquired the name %s on the session bus.\n", name);

    g_main_loop_quit (main_loop);
}

int
main (void)
{
    GMainLoop *main_loop;
    guint owner_id;

    g_type_init ();

    /* Create a new event loop to run in */
    main_loop = g_main_loop_new (NULL, FALSE);

    owner_id = g_bus_own_name (G_BUS_TYPE_SESSION, "org.notmuchmail",
	     G_BUS_NAME_OWNER_FLAGS_NONE,
	     NULL,
	     &notmuch_dbus_name_acquired_cb,
	     NULL,
	     main_loop,
	     NULL);

    g_main_loop_run (main_loop);

    g_bus_unown_name (owner_id);

    return EXIT_SUCCESS;
}
