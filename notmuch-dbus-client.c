#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

#include "notmuch-dbus-client.h"
#include "notmuch-dbus-generated.h"

#include "notmuch-client.h"

typedef struct _NotmuchDBusClientPrivate NotmuchDBusClientPrivate;

struct _NotmuchDBusClient
{
  GObject parent_instance;
  NotmuchDBusClientPrivate *priv;
};

struct _NotmuchDBusClientClass
{
  GObjectClass parent_class;
};

struct _NotmuchDBusClientPrivate
{
  notmuch_config_t *config;
};

enum
{
  PROP_0,
  PROP_CONFIG,
  N_PROPERTIES
};

G_DEFINE_TYPE_WITH_PRIVATE (NotmuchDBusClient, notmuch_dbus_client, G_TYPE_OBJECT);

NotmuchDBusClient *
notmuch_dbus_client_new (notmuch_config_t *config)
{
  g_return_val_if_fail (config, NULL);

  return NOTMUCH_DBUS_CLIENT (g_object_new (NOTMUCH_DBUS_TYPE_CLIENT,
        "config", config,
        NULL));
}

static void
notmuch_dbus_client_class_init (NotmuchDBusClientClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_object_class_install_property (gobject_class,
      PROP_CONFIG,
      g_param_spec_pointer ("config",
        "config",
        "Notmuch configuration",
        G_PARAM_READWRITE |
        G_PARAM_CONSTRUCT_ONLY |
        G_PARAM_STATIC_STRINGS));
}

static void
notmuch_dbus_client_init (NotmuchDBusClient *self)
{
  self->priv = notmuch_dbus_client_get_instance_private (self);
}
