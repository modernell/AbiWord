/* AbiWord
 * Copyright (C) 2011-2019 Hubert Figuiere
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

#pragma once
#include <gtk/gtk.h>

#if GTK_CHECK_VERSION(3,96,0)
#define XAP_HAS_NATIVE_WINDOW(w) \
  (gtk_widget_get_surface(w) != nullptr)
#else
#define XAP_HAS_NATIVE_WINDOW(w) \
  (gtk_widget_get_window(w) != nullptr)
#endif

/// Convenience to raise the widget window.
void XAP_gtk_window_raise(GtkWidget*);

/// Convenience to set the same margin on all side.
void XAP_gtk_widget_set_margin(GtkWidget* w, gint margin);

/// Convenience to get the entry text.
inline
const gchar* XAP_gtk_entry_get_text(GtkEntry* entry)
{
    return gtk_entry_buffer_get_text(gtk_entry_get_buffer(entry));
}

/// Convenience to set the entry text.
inline
void XAP_gtk_entry_set_text(GtkEntry* entry, const gchar* text)
{
  auto buffer = gtk_entry_get_buffer(entry);
  gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(buffer), text, g_utf8_strlen(text, -1));
}

void XAP_gtk_keyboard_ungrab(GtkWidget *widget);

#if GTK_CHECK_VERSION(3,96,0)
// It's a no-op on Gtk4
inline
void gtk_widget_show_all(GtkWidget*)
{
}
#endif
