/* -*- mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* AbiWord
 * Copyright (C) 2019 Hubert Figuière
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


#include "xap_UnixDialog.h"
#include "xap_UnixDialogHelper.h"

#if GTK_CHECK_VERSION(3,96,0)
static void s_delete_clicked(GtkWidget * widget, gpointer)
#else
static void s_delete_clicked(GtkWidget * widget, gpointer, gpointer)
#endif
{
  abiDestroyWidget(widget);
}

void XAP_UnixDialog::connectBasicSignals()
{
#if GTK_CHECK_VERSION(3,96,0)
	g_signal_connect(G_OBJECT(m_windowMain), "close-request",
					   G_CALLBACK(s_delete_clicked), nullptr);
#else
    g_signal_connect(G_OBJECT(m_windowMain),
                     "delete_event",
                     G_CALLBACK(s_delete_clicked),
                     static_cast<gpointer>(this));
#endif
}
