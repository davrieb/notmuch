#include <glib.h>
#include <gio/gio.h>

#include "notmuch-dbus-daemon.h"

typedef struct _NotmuchDBusDaemonClass NotmuchDBusDaemonClass;

struct _NotmuchDBusDaemon
{
  GObject parent_instance;
  GDBusConnection *connection;
  GMainLoop *main_loop;
};

struct _NotmuchDBusDaemonClass
{
  GObjectClass parent_class;
};

enum
{
  PROP_0,
  PROP_CONNECTION,
  PROP_MAIN_LOOP,
  N_PROPERTIES
};

G_DEFINE_TYPE (NotmuchDBusDaemon, notmuch_dbus_daemon, G_TYPE_OBJECT);

struct _NotmuchDBusDaemon *
notmuch_dbus_daemon_new (GMainLoop *main_loop,
    GDBusConnection *connection)
{
  g_return_val_if_fail (main_loop, NULL);
  g_return_val_if_fail (G_IS_DBUS_CONNECTION (connection), NULL);
  return NOTMUCH_DBUS_DAEMON (g_object_new (NOTMUCH_DBUS_TYPE_DAEMON,
        "main_loop", main_loop,
        "conection", connection,
        NULL));
}

static GObject *
notmuch_dbus_daemon_constructor (GType gtype,
    guint n_properties,
    GObjectConstructParam *properties)
{
  GObject *object;

  object = G_OBJECT_CLASS (notmuch_dbus_daemon_parent_class)->constructor (gtype, n_properties, properties);

  return object;
}

static void
notmuch_dbus_daemon_class_init (NotmuchDBusDaemonClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->constructor = notmuch_dbus_daemon_constructor;

  g_object_class_install_property (gobject_class,
	  PROP_CONNECTION,
	  g_param_spec_pointer ("main-loop",
	      "Main loop",
	      "The GMainLoop this daemon uses",
	      G_PARAM_READWRITE |
	      G_PARAM_CONSTRUCT_ONLY |
	      G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (gobject_class,
	  PROP_CONNECTION,
	  g_param_spec_object ("connection",
	      "Connection",
	      "The D-Bus connection this daemon uses",
	      G_TYPE_DBUS_CONNECTION,
	      G_PARAM_READWRITE |
	      G_PARAM_CONSTRUCT_ONLY |
	      G_PARAM_STATIC_STRINGS));
}

static void
notmuch_dbus_daemon_init (G_GNUC_UNUSED NotmuchDBusDaemon *self)
{
}

gboolean
callback (gpointer data)
{
  GMainLoop* main_loop;

  g_print("idle callback called, exiting.\n");

  main_loop = (GMainLoop*) data;
  g_main_loop_quit(main_loop);
  return G_SOURCE_REMOVE;
}

void
notmuch_dbus_acquired_handler (GDBusConnection *connection,
    const gchar *name,
    G_GNUC_UNUSED gpointer user_data)
{
  g_assert (connection);
  g_assert (name);

  g_print ("Acquired D-Bbus connection, requesting name %s\n", name);
}

void
notmuch_dbus_name_acquired_handler (GDBusConnection *connection,
    const gchar *name,
    gpointer user_data)
{
  GMainLoop *main_loop;
  NotmuchDBusDaemon *daemon;

  g_assert (connection);
  g_assert (name);

  main_loop = (GMainLoop*) user_data;

  g_print ("Acquired name %s on D-Bus\n", name);

  daemon = notmuch_dbus_daemon_new (main_loop, connection);

  g_idle_add (callback, main_loop);
}

void
notmuch_dbus_name_lost_handler (GDBusConnection *connection,
    const gchar *name,
    G_GNUC_UNUSED gpointer user_data)
{
  g_assert (name);

  if (connection == NULL) {
    g_print ("Failed to connect to D-Bus.\n");
    return;
  }

  g_print ("Name %s can't be acquired on D-Bus\n", name);
}

int
main ()
{
  GMainLoop* main_loop;
  guint name_owner_id;

  main_loop = g_main_loop_new (NULL, FALSE);

  name_owner_id = g_bus_own_name (G_BUS_TYPE_SESSION,
      "org.notmuchmail.NotMuch0_1",
      G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT | G_BUS_NAME_OWNER_FLAGS_REPLACE,
      notmuch_dbus_acquired_handler,
      notmuch_dbus_name_acquired_handler,
      notmuch_dbus_name_lost_handler,
      main_loop,
      NULL);

  g_main_loop_run (main_loop);

  g_main_loop_unref (main_loop);
  if (name_owner_id != 0)
    g_bus_unown_name (name_owner_id);

  return 0;
}
