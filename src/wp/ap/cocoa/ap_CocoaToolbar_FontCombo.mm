/* AbiWord
 * Copyright (C) 1998 AbiSource, Inc.
 * Copyright (C) 2001 Hubert Figuiere
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  
 * 02111-1307, USA.
 */

#import <Cocoa/Cocoa.h>

#include "ut_assert.h"
#include "ut_vector.h"
#include "ut_hash.h"
#include "ut_debugmsg.h"

#include "xap_CocoaFont.h"
#include "xap_CocoaFontManager.h"

#include "ap_CocoaToolbar_FontCombo.h"
#include "ap_Toolbar_Id.h"
#include "xap_CocoaApp.h"
#include "xap_Frame.h"

#include "ev_CocoaToolbar.h"
#include "ev_Toolbar.h"


EV_Toolbar_Control * AP_CocoaToolbar_FontCombo::static_constructor(EV_Toolbar * pToolbar,
														  XAP_Toolbar_Id tlbrid)
{
	AP_CocoaToolbar_FontCombo * p = new AP_CocoaToolbar_FontCombo(pToolbar,tlbrid);
	return p;
}

AP_CocoaToolbar_FontCombo::AP_CocoaToolbar_FontCombo(EV_Toolbar * pToolbar,
												   XAP_Toolbar_Id tlbrid)
	: EV_Toolbar_Control(pToolbar/*,id*/)
{
	UT_ASSERT(tlbrid == AP_TOOLBAR_ID_FMT_FONT);
	m_nPixels = 150;
	m_nLimit = 32;
}

AP_CocoaToolbar_FontCombo::~AP_CocoaToolbar_FontCombo(void)
{
	// nothing to purge.  contents are static strings
}

bool AP_CocoaToolbar_FontCombo::populate(void)
{
	UT_ASSERT(m_pToolbar);
	
	// Things are relatively easy with the font manager.  Just
	// request all fonts and ask them their names.
	EV_CocoaToolbar * toolbar = static_cast<EV_CocoaToolbar *>(m_pToolbar);
	
	UT_Vector * list = toolbar->getApp()->getFontManager()->getAllFonts();
	UT_ASSERT(list);

	UT_uint32 count = list->size();

	m_vecContents.clear();

	for (UT_uint32 i = 0; i < count; i++)
	{
		// sort-out duplicates
		XAP_CocoaFont * pFont = (XAP_CocoaFont *)list->getNthItem(i);
		const char * fName = pFont->getName();

		int foundAt = -1;

		for (int j = 0; j < m_vecContents.size(); j++)
		{
			// sort out dups
			char * str = (char *)m_vecContents.getNthItem(j);
			if (str && !UT_strcmp (str, fName))
			{
				foundAt = j;
				break;
			}
		}

		if (foundAt == -1)
			m_vecContents.addItem((void *)(fName));
	}
	DELETEP(list);

	return true;
}
