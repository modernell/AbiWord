/* AbiWord
 * Copyright (C) 2000 AbiSource, Inc.
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


#include "ut_string.h"
#include "ut_assert.h"
#include "ut_debugmsg.h"

#include "xap_App.h"
#include "xap_QNXApp.h"
#include "xap_QNXFrame.h"

#include "ap_Strings.h"
#include "ap_Dialog_Id.h"
#include "ap_Dialog_Lists.h"
#include "ap_QNXDialog_Lists.h"

#include "ut_qnxHelper.h"

/*****************************************************************/

XAP_Dialog * AP_QNXDialog_Lists::static_constructor(XAP_DialogFactory * pFactory,
													 XAP_Dialog_Id id)
{
	AP_QNXDialog_Lists * p = new AP_QNXDialog_Lists(pFactory,id);
	return p;
}

AP_QNXDialog_Lists::AP_QNXDialog_Lists(XAP_DialogFactory * pDlgFactory,
										 XAP_Dialog_Id id)
	: AP_Dialog_Lists(pDlgFactory,id)
{
}

AP_QNXDialog_Lists::~AP_QNXDialog_Lists(void)
{
}

/**********************************************************************/

static int s_startChanged (PtWidget_t *widget, void *data, PtCallbackInfo_t *info)
{
	AP_QNXDialog_Lists * dlg = (AP_QNXDialog_Lists *)data;
	dlg->startChanged ();
	return Pt_CONTINUE;
}

static int s_stopChanged (PtWidget_t *widget, void *data, PtCallbackInfo_t *info)
{
	AP_QNXDialog_Lists * dlg = (AP_QNXDialog_Lists *)data;
	dlg->stopChanged ();
	return Pt_CONTINUE;
}

static int s_startvChanged (PtWidget_t *widget, void *data, PtCallbackInfo_t *info)
{
	AP_QNXDialog_Lists * dlg = (AP_QNXDialog_Lists *)data;
	dlg->startvChanged ();
	return Pt_CONTINUE;
}

static int s_applyClicked (PtWidget_t *widget, void *data, PtCallbackInfo_t *info)
{
	AP_QNXDialog_Lists * dlg = (AP_QNXDialog_Lists *)data;
	dlg->applyClicked();
	return Pt_CONTINUE;
}

static int s_closeClicked (PtWidget_t *widget, void *data, PtCallbackInfo_t *info)
{
	AP_QNXDialog_Lists * dlg = (AP_QNXDialog_Lists *)data;
	dlg->destroy();
	return Pt_CONTINUE;
}

static int s_deleteClicked (PtWidget_t *widget, void *data, PtCallbackInfo_t *info)
{
	AP_QNXDialog_Lists * dlg = (AP_QNXDialog_Lists *)data;
	dlg->destroy();
	return Pt_CONTINUE;
}

static int s_update (void)
{
/*
	AP_QNXDialog_Lists * dlg = (AP_QNXDialog_Lists *)data;
	Current_Dialog->updateDialog();
*/
	return UT_TRUE;
}

/**********************************************************************/

void AP_QNXDialog_Lists::activate()
{
	ConstructWindowName();
	PtSetResource(m_mainWindow, Pt_ARG_WINDOW_TITLE, m_WindowName, NULL);
	updateDialog();
	//Raise the window ...
}

void AP_QNXDialog_Lists::destroy()
{
	if (!m_mainWindow) {
		return;
	}
	m_bDestroy_says_stopupdating = UT_TRUE;
	while (m_bAutoUpdate_happening_now == UT_TRUE) ;
	m_pAutoUpdateLists->stop();
	m_answer = AP_Dialog_Lists::a_CLOSE;	
	modeless_cleanup();
	
	PtWidget_t *tmp = m_mainWindow;
	m_mainWindow = NULL;
	PtDestroyWidget(tmp);
}

void AP_QNXDialog_Lists::notifyActiveFrame(XAP_Frame *pFrame)
{
	activate();
}

void AP_QNXDialog_Lists::runModeless(XAP_Frame * pFrame)
{
	UT_ASSERT(pFrame);

	m_mainWindow = _constructWindow ();
	UT_ASSERT (m_mainWindow);

	// Save dialog the ID number and pointer to the widget
	UT_sint32 sid = (UT_sint32) getDialogId ();
	m_pApp->rememberModelessId(sid, (XAP_Dialog_Modeless *) m_pDialog);
 
	// This magic command displays the frame that characters will be
	// inserted into.
    // This variation runs the additional static function shown afterwards.
    // Only use this if you need to to update the dialog upon focussing.
	//connectFocusModelessOther (m_mainWindow, m_pApp, s_update);
	connectFocusModeless(m_mainWindow, m_pApp);

	// Populate the dialog
	updateDialog();

	// Now Display the dialog
	PtRealizeWidget(m_mainWindow);

	// Next construct a timer for auto-updating the dialog
	m_pAutoUpdateLists = UT_Timer::static_constructor(autoupdateLists,this);
	m_bDestroy_says_stopupdating = UT_FALSE;

	// OK fire up the auto-updater for 0.5 secs
	m_pAutoUpdateLists->set(500);
}

void    AP_QNXDialog_Lists::autoupdateLists(UT_Timer * pTimer)
{
	UT_ASSERT(pTimer);
	// this is a static callback method and does not have a 'this' pointer.
	AP_QNXDialog_Lists * pDialog =  (AP_QNXDialog_Lists *) pTimer->getInstanceData();
	// Handshaking code

	if( pDialog->m_bDestroy_says_stopupdating != UT_TRUE)
	{
		pDialog->m_bAutoUpdate_happening_now = UT_TRUE;
		pDialog->updateDialog();
		pDialog->m_bAutoUpdate_happening_now = UT_FALSE;
	}
}


void  AP_QNXDialog_Lists::applyClicked(void)
{
#if 0
    char * szStartValue;
	PtWidget_t * wlisttype;

	//
	// Failsafe code to make sure the start, stop and change flags are set
        // as shown on the GUI.
	//

       if (GTK_TOGGLE_BUTTON (m_wCheckstartlist)->active)
       {
	       wlisttype=gtk_menu_get_active(GTK_MENU(m_wOption_types_menu));
	       m_iListType = GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(wlisttype)));
	       szStartValue =gtk_entry_get_text( GTK_ENTRY (m_wNew_startingvaluev) );
	       m_bStartList = UT_TRUE;
	       if(m_iListType == 0)
	       {
		      m_newStartValue = atoi(szStartValue);
		      strcpy((gchar *) m_newListType, "%*%d.");
	       }
	       else if (m_iListType == 1)
	       {
		      m_newStartValue = atoi(szStartValue);
		      strcpy((gchar *) m_newListType,"%*%a.");
	       }
	       else if (m_iListType == 2)
	       {
		      m_newStartValue = atoi(szStartValue);
		      strcpy((gchar *) m_newListType,"%*%A.");
	       }
	       else if (m_iListType == 3)
	       {
		 //	      gchar c = *szStartValue;
		      m_newStartValue = 1;
		      strcpy((gchar *) m_newListType, "%b");
	       }
       }
       else
	       m_bStartList = UT_FALSE;

       if (GTK_TOGGLE_BUTTON (m_wCheckstoplist)->active)
	       m_bStopList = UT_TRUE;
       else
	       m_bStopList = UT_FALSE;

       if (GTK_TOGGLE_BUTTON (m_wCur_changestart_button)->active)
       {
	       m_bChangeStartValue = UT_TRUE;
	       wlisttype=gtk_menu_get_active(GTK_MENU(m_wCur_Option_types_menu));
	       m_iListType = GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(wlisttype)));
	       szStartValue =gtk_entry_get_text( GTK_ENTRY (m_wCur_startingvaluev) );
	       m_bStartList = UT_TRUE;
	       if(m_iListType == 0)
	       {
		      m_curStartValue = atoi(szStartValue);
		      strcpy((gchar *) m_newListType, "%*%d.");
	       }
	       else if (m_iListType == 1)
	       {
		      m_curStartValue = atoi(szStartValue);
		      strcpy((gchar *) m_newListType,"%*%a.");
	       }
	       else if (m_iListType == 2)
	       {
		      m_curStartValue = atoi(szStartValue);
		      strcpy((gchar *) m_newListType,"%*%A.");
	       }
	       else if (m_iListType == 3)
	       {
		 //	      gchar c = *szStartValue;
		      m_curStartValue = 1;
		      strcpy((gchar *) m_newListType, "%b");
	       }
       }
       else
	       m_bChangeStartValue = UT_FALSE;

	Apply();

	// Make all checked buttons inactive 
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wCheckstoplist),FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wCur_changestart_button),FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wCheckstartlist),FALSE);
	setAllSensitivity();
#endif
}

#if 0
void mark_changes(PtWidget_t *widget, UT_Bool *update) {

	if (TOGGLED(widget)) {
		m_bStartList = UT_FALSE;
	    m_bStopList = UT_FALSE;
	    m_bChangeStartValue = UT_FALSE;
		*update = UT_TRUE;
		
        m_wCur_changestart_button),FALSE);
        m_wCheckresumelist),FALSE);
        m_wCheckstoplist),FALSE);
	}
	else {
		*update = UT_FALSE;
	}
}
#endif


void  AP_QNXDialog_Lists::startChanged(void)
{
#if 0
	if (GTK_TOGGLE_BUTTON (m_wCheckstartlist)->active)
	{
	       m_bStartList = UT_TRUE;
	       m_bStopList = UT_FALSE;
	       m_bChangeStartValue = UT_FALSE;
           gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wCur_changestart_button),FALSE);
           gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wCheckresumelist),FALSE);
           gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wCheckstoplist),FALSE);
    }
    else
    {
		m_bStartList = UT_FALSE;
	}
	setAllSensitivity();
#endif
}


void  AP_QNXDialog_Lists::stopChanged(void)
{
#if 0
	if (GTK_TOGGLE_BUTTON (m_wCheckstoplist)->active)
	{
		m_bStopList = UT_TRUE;
		m_bChangeStartValue = UT_FALSE;
		m_bStartList = UT_FALSE;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wCur_changestart_button),FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wCheckresumelist),FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wCheckstartlist),FALSE);
	}
	else
	{
		m_bStopList = UT_FALSE;
	}
	setAllSensitivity();
#endif
}


void  AP_QNXDialog_Lists::startvChanged(void)
{
#if 0
	if (GTK_TOGGLE_BUTTON (m_wCur_changestart_button)->active)
	{
		m_bChangeStartValue = UT_TRUE;
		m_bStartList = UT_FALSE;
		m_bStopList = UT_FALSE;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wCheckresumelist),FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wCheckstoplist),FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wCheckstartlist),FALSE);
	}
	else
	{
		m_bChangeStartValue = UT_FALSE;
	}
	setAllSensitivity();
#endif
}


void AP_QNXDialog_Lists::updateDialog(void)
{
  // Update the dialog
  //
	_populateWindowData();
	setAllSensitivity();
}


void AP_QNXDialog_Lists::setAllSensitivity(void)
{ 
#if 0
       gtk_widget_set_sensitive( m_wCheckstartlist,TRUE);
       PopulateDialogData();
       if(m_isListAtPoint == UT_TRUE)
       {
	       gtk_widget_set_sensitive( m_wCheckstoplist,TRUE);
	       gtk_widget_set_sensitive( m_wCur_listtype,TRUE);
	       gtk_widget_set_sensitive( m_wCur_listtypev,TRUE);
	       gtk_widget_set_sensitive( m_wCur_listlabel,TRUE);
	       gtk_widget_set_sensitive( m_wCur_listlabelv,TRUE);
	       gtk_widget_set_sensitive( m_wCheckresumelist,FALSE);
               gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wCheckresumelist),FALSE);
       }
       else
       {
	       gtk_widget_set_sensitive( m_wCheckstoplist,FALSE);
               gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wCheckstoplist),FALSE);
	       m_bStopList = UT_FALSE;
	       gtk_widget_set_sensitive( m_wCur_listtype,FALSE);
	       gtk_widget_set_sensitive( m_wCur_listtypev,FALSE);
	       gtk_widget_set_sensitive( m_wCur_listlabel,FALSE);
	       gtk_widget_set_sensitive( m_wCur_listlabelv,FALSE);
	       gtk_widget_set_sensitive( m_wCur_changestart_button,FALSE);
               gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wCur_changestart_button),FALSE);
	       m_bChangeStartValue = UT_FALSE;
	       gtk_widget_set_sensitive( m_wCur_Option_types,FALSE);
	       gtk_widget_set_sensitive( m_wCur_startingvaluel,FALSE);
	       gtk_widget_set_sensitive( m_wCur_startingvaluev,FALSE);
	       if(m_previousListExistsAtPoint == UT_TRUE)
	       {
	               gtk_widget_set_sensitive( m_wCheckresumelist,TRUE);
	       }
	       else
	       {
	               gtk_widget_set_sensitive( m_wCheckresumelist,FALSE);
		       gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wCheckresumelist),FALSE);
	       }

       }
       if(!GTK_TOGGLE_BUTTON( m_wCur_changestart_button)->active)
       {
	       gtk_widget_set_sensitive( m_wCur_Option_types,FALSE);
	       gtk_widget_set_sensitive( m_wCur_startingvaluel,FALSE);
	       gtk_widget_set_sensitive( m_wCur_startingvaluev,FALSE);
       }

       if(GTK_TOGGLE_BUTTON( m_wCheckstoplist)->active)
       {
	       gtk_widget_set_sensitive( m_wCur_changestart_button,FALSE);
	       gtk_widget_set_sensitive( m_wCheckresumelist,FALSE);
	       gtk_widget_set_sensitive( m_wCur_Option_types,FALSE);
	       gtk_widget_set_sensitive( m_wCur_startingvaluel,FALSE);
	       gtk_widget_set_sensitive( m_wCur_startingvaluev,FALSE);
               gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wCheckresumelist),FALSE);
       }
       else if(m_isListAtPoint == UT_TRUE)
       {
	       gtk_widget_set_sensitive( m_wCur_changestart_button,TRUE);
	       if(GTK_TOGGLE_BUTTON( m_wCur_changestart_button)->active)
	       {
	                gtk_widget_set_sensitive( m_wCur_Option_types,TRUE);
	                gtk_widget_set_sensitive( m_wCur_startingvaluel,TRUE);
	                gtk_widget_set_sensitive( m_wCur_startingvaluev,TRUE);
	       }
       }
       if(GTK_TOGGLE_BUTTON( m_wCheckstartlist)->active)
       {
	       gtk_widget_set_sensitive( m_wNewlisttypel,TRUE);
	       gtk_widget_set_sensitive( m_wOption_types,TRUE);
	       gtk_widget_set_sensitive( m_wOption_types_menu,TRUE);
	       gtk_widget_set_sensitive( m_wNew_startingvaluel,TRUE);
	       gtk_widget_set_sensitive( m_wNew_startingvaluev,TRUE);
	       gtk_widget_set_sensitive( m_wCheckresumelist,FALSE);
               gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wCur_changestart_button),FALSE);
               gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_wCheckresumelist),FALSE);
       }
       else
       {
	       gtk_widget_set_sensitive( m_wNewlisttypel,FALSE);
	       gtk_widget_set_sensitive( m_wOption_types,FALSE);
	       gtk_widget_set_sensitive( m_wOption_types_menu,FALSE);
	       gtk_widget_set_sensitive( m_wNew_startingvaluel,FALSE);
	       gtk_widget_set_sensitive( m_wNew_startingvaluev,FALSE);
       }
#endif
}


PtWidget_t * AP_QNXDialog_Lists::_constructWindow (void)
{
  // Code to construct the dialog window
  //
}


void AP_QNXDialog_Lists::_populateWindowData (void) 
{
  // Code to fill in the labels from the current list context in the current 
  // view
}


