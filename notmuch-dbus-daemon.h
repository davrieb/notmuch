
#ifndef __NOTMUCH_DBUS_DAEMON_H__
#define __NOTMUCH_DBUS_DAEMON_H__

#include <glib.h>

G_BEGIN_DECLS

gboolean
callback (gpointer data);

void
notmuch_dbus_acquired_handler (GDBusConnection *connection,
    const gchar *name,
    gpointer user_data);

void
notmuch_dbus_name_acquired_handler (GDBusConnection *connection,
    const gchar *name,
    gpointer user_data);

void
notmuch_dbus_name_lost_handler (GDBusConnection *connection,
    const gchar *name,
    gpointer user_data);

struct _NotmuchDBusDaemon;
typedef struct _NotmuchDBusDaemon NotmuchDBusDaemon;

#define NOTMUCH_DBUS_TYPE_DAEMON        (notmuch_dbus_daemon_get_type())
#define NOTMUCH_DBUS_DAEMON(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), NOTMUCH_DBUS_TYPE_DAEMON, NotmuchDBusDaemon))
#define NOTMUCH_DBUS_IS_DAEMON(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), NOTMUCH_DBUS_TYPE_DAEMON))

GType notmuch_dbus_daemon_get_type (void) G_GNUC_CONST;
NotmuchDBusDaemon *notmuch_dbus_daemon_new (GMainLoop *main_loop, GDBusConnection *connection);
GDBusConnection *notmuch_dbus_daemon_get_connection (NotmuchDBusDaemon *daemon);

G_END_DECLS

#endif /* __NOTMUCH_DBUS_DAEMON_H__ */
