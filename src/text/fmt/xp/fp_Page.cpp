 
/*
** The contents of this file are subject to the AbiSource Public
** License Version 1.0 (the "License"); you may not use this file
** except in compliance with the License. You may obtain a copy
** of the License at http://www.abisource.com/LICENSE/ 
** 
** Software distributed under the License is distributed on an
** "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
** implied. See the License for the specific language governing
** rights and limitations under the License. 
** 
** The Original Code is AbiWord.
** 
** The Initial Developer of the Original Code is AbiSource, Inc.
** Portions created by AbiSource, Inc. are Copyright (C) 1998 AbiSource, Inc. 
** All Rights Reserved. 
** 
** Contributor(s):
**  
*/


#include <stdlib.h>
#include <string.h>

#include "ut_types.h"
#include "ut_misc.h"

#include "fp_Page.h"
#include "fl_DocLayout.h"
#include "fp_SectionSlice.h"
#include "dg_DrawArgs.h"
#include "dg_LayoutView.h"

#include "ut_debugmsg.h"
#include "ut_assert.h"
#include "ut_units.h"

FP_Page::FP_Page(FL_DocLayout* pLayout, DG_LayoutView* pLayoutView,
				 UT_uint32 iWidth, UT_uint32 iHeight,
				 UT_uint32 iLeft,
				 UT_uint32 iTop, 
				 UT_uint32 iRight,
				 UT_uint32 iBottom)
{
	UT_ASSERT(pLayout);

	m_pLayout = pLayout;
	m_pLayoutView = pLayoutView;
	
	DG_Graphics * pG = pLayout->getGraphics();
	UT_ASSERT(pG);

	m_iWidth = UT_docUnitsFromPaperUnits(pG,iWidth);
	m_iHeight = UT_docUnitsFromPaperUnits(pG,iHeight);

	m_iLeft = UT_docUnitsFromPaperUnits(pG,iLeft);
	m_iTop = UT_docUnitsFromPaperUnits(pG,iTop);
	m_iRight = UT_docUnitsFromPaperUnits(pG,iRight);
	m_iBottom = UT_docUnitsFromPaperUnits(pG,iBottom);

	m_pNext = NULL;
}

fp_SectionSliceInfo::fp_SectionSliceInfo(FP_SectionSlice* p, UT_uint32 x, UT_uint32 y)
{
	pSlice = p;
	xoff = x;
	yoff = y;
}

FP_Page::~FP_Page()
{
	UT_VECTOR_PURGEALL(fp_SectionSliceInfo, m_vecSliceInfos);
}


int FP_Page::getWidth()
{
	return m_iWidth;
}

int FP_Page::getHeight()
{
	return m_iHeight;
}

void FP_Page::getOffsets(FP_SectionSlice* pSS, void* pData, UT_sint32& xoff, UT_sint32& yoff)
{
	fp_SectionSliceInfo* pSSI = (fp_SectionSliceInfo*) pData;
	UT_ASSERT(pSS == pSSI->pSlice);

	xoff = pSSI->xoff;
	yoff = pSSI->yoff;
}

void FP_Page::getScreenOffsets(FP_SectionSlice* pSS, void* pData, UT_sint32& xoff, UT_sint32& yoff, UT_sint32& width, UT_sint32& height)
{
	fp_SectionSliceInfo* pSSI = (fp_SectionSliceInfo*) pData;
	UT_ASSERT(pSS == pSSI->pSlice);

	UT_ASSERT(m_pLayoutView);
	
	m_pLayoutView->getPageScreenOffsets(this, xoff, yoff, width, height);

	xoff += pSSI->xoff;
	yoff += pSSI->yoff;
}

FP_Page* FP_Page::getNext()
{
	return m_pNext;
}

void FP_Page::setNext(FP_Page* p)
{
	m_pNext = p;
}

FL_DocLayout* FP_Page::getLayout()
{
	return m_pLayout;
}

void FP_Page::draw(dg_DrawArgs* pDA)
{
	/*
		draw each slice on the page
	*/
	int count = m_vecSliceInfos.getItemCount();
	FP_SectionSlice* p;

	for (int i=0; i<count; i++)
	{
		fp_SectionSliceInfo* pci = (fp_SectionSliceInfo*) m_vecSliceInfos.getNthItem(i);
		p = pci->pSlice;

		dg_DrawArgs da = *pDA;
		da.xoff += pci->xoff;
		da.yoff += pci->yoff;
		p->draw(&da);
	}
}

void FP_Page::dump()
{
	int count = m_vecSliceInfos.getItemCount();
	FP_SectionSlice* p;

	for (int i=0; i<count; i++)
	{
		fp_SectionSliceInfo* pci = (fp_SectionSliceInfo*) m_vecSliceInfos.getNthItem(i);
		p = pci->pSlice;

		UT_DEBUGMSG(("FP_Page::dump(0x%x) - FP_SectionSlice 0x%x\n", this, p));
		p->dump();
	}
}

UT_Bool FP_Page::requestSpace(FL_SectionLayout*, FP_SectionSlice** ppsi)
{
	UT_sint32 iHeight = 0;
	int count = m_vecSliceInfos.getItemCount();
	for (int i=0; i<count; i++)
	{
		fp_SectionSliceInfo* pSSI = (fp_SectionSliceInfo*) m_vecSliceInfos.getNthItem(i);
		iHeight += pSSI->pSlice->getHeight();
	}

	UT_sint32 yBottom = m_iTop + iHeight;
	UT_sint32 iAvailable = m_iHeight - m_iBottom - yBottom;

	if (iAvailable > 0)
	{
		FP_SectionSlice* pSS = new FP_SectionSlice(m_iWidth - (m_iRight + m_iLeft), iAvailable);
		fp_SectionSliceInfo* pSSI = new fp_SectionSliceInfo(pSS, m_iLeft, yBottom);
		pSS->setPage(this, pSSI);
		m_vecSliceInfos.addItem(pSSI);
		*ppsi = pSS;

		return UT_TRUE;
	}
	else
	{
		return UT_FALSE;
	}
}

void FP_Page::mapXYToPosition(UT_sint32 x, UT_sint32 y, PT_DocPosition& pos, UT_Bool& bRight)
{
	int count = m_vecSliceInfos.getItemCount();
	FP_SectionSlice* p;
	UT_uint32 iMinDist = 0xffffffff;
	fp_SectionSliceInfo* pMinDist = NULL;
	
	for (int i=0; i<count; i++)
	{
		fp_SectionSliceInfo* pci = (fp_SectionSliceInfo*) m_vecSliceInfos.getNthItem(i);
		p = pci->pSlice;

		// when hit testing for SectionSlices on a page, we ignore X coords
//		if (((x - pci->xoff) > 0) && ((x - pci->xoff) < p->getWidth()))
		{
			if (((y - (UT_sint32)pci->yoff) > 0) && ((y - (UT_sint32)pci->yoff) < (UT_sint32)p->getHeight()))
			{
				p->mapXYToPosition(x - pci->xoff, y - pci->yoff, pos, bRight);
				return;
			}
		}

		UT_uint32 iDistTop;
		UT_uint32 iDistBottom;
		UT_uint32 iDist;

		iDistTop = UT_ABS((y - (UT_sint32) pci->yoff));
		iDistBottom = UT_ABS((y - (UT_sint32) (pci->yoff + p->getHeight())));
			
		iDist = UT_MIN(iDistTop, iDistBottom);
		if (iDist < iMinDist)
		{
			pMinDist = pci;
			iMinDist = iDist;
		}
	}

	UT_ASSERT(pMinDist);
	pMinDist->pSlice->mapXYToPosition(x - pMinDist->xoff, y - pMinDist->yoff, pos, bRight);
}

void FP_Page::setLayoutView(DG_LayoutView* pLayoutView)
{
	m_pLayoutView = pLayoutView;
}


