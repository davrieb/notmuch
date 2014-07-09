
#ifndef __NOTMUCH_DBUS_CLIENT_H__
#define __NOTMUCH_DBUS_CLIENT_H__

#include <glib.h>
#include "notmuch-client.h"

G_BEGIN_DECLS

struct _NotmuchDBusClient;
typedef struct _NotmuchDBusClient NotmuchDBusClient;

struct _NotmuchDBusClientClass;
typedef struct _NotmuchDBusClientClass NotmuchDBusClientClass;

#define NOTMUCH_DBUS_TYPE_CLIENT (notmuch_dbus_client_get_type())
#define NOTMUCH_DBUS_CLIENT(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), NOTMUCH_DBUS_TYPE_CLIENT, NotmuchDBusClient))
#define NOTMUCH_DBUS_IS_CLIENT(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), NOTMUCH_DBUS_TYPE_CLIENT))
#define NOTMUCH_DBUS_DAEMON_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), NOTMUCH_DBUS_TYPE_CLIENT, NotmuchDBusClientClass))
#define NOTMUCH_DBUS_IS_CLIENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), NOTMUCH_DBUS_TYPE_CLIENT))
#define NOTMUCH_DBUS_CLIENT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), NOTMUCH_DBUS_TYPE_CLIENT, NotmuchDBusClientClass))

GType
notmuch_dbus_client_get_type (void) G_GNUC_CONST;

NotmuchDBusClient *
notmuch_dbus_client_new (notmuch_config_t *config);

G_END_DECLS

#endif /* __NOTMUCH_DBUS_CLIENT_H__ */
