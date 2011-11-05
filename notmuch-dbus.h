/* notmuch - Not much of an email program, (just index and search)
 *
 * Copyright © 2009 Carl Worth
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
 * Author: Carl Worth <cworth@cworth.org>
 */

#ifndef NOTMUCH_DBUS_H
#define NOTMUCH_DBUS_H


#ifndef NOTMUCH_DBUS_NAME
#    define NOTMUCH_DBUS_NAME "org.notmuchmail.NotMuch"
#endif

#ifndef NOTMUCH_DBUS_SERVER_NAME
#    define NOTMUCH_DBUS_SERVER_NAME "org.notmuchmail.NotMuch.Server"
#endif

#ifndef NOTMUCH_DBUS_CLI_NAME
#    define NOTMUCH_DBUS_CLI_NAME "org.notmuchmail.NotMuch.CLI"
#endif

#ifndef NOTMUCH_DBUS_OBJECT
#    define NOTMUCH_DBUS_OBJECT "/org/notmuchmail/NotMuch/Server"
#endif

#ifndef NOTMUCH_DBUS_INTERFACE
#    define NOTMUCH_DBUS_INTERFACE "org.notmuchmail.NotMuch.TestInterface"
#endif

#ifndef NOTMUCH_DBUS_INTROSPECTION_XML
#    define NOTMUCH_DBUS_INTROSPECTION_XML "notmuch-dbus-interface.xml"
#endif

#endif
