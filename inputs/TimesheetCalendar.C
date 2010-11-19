/*
 * Copyright (C) 2010 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

/*
 * Modifications made by Neil Xavier D'Souza and Copyrighted - 
 * 	Year 2010
 * License GNU GPL V2
 */ 

#include "TimesheetCalendar.h"
#include "CalendarCell.h"

using namespace Wt;

TimesheetCalendar::TimesheetCalendar(WContainerWidget* parent)
	: WCalendar(parent)
{
	setStyleClass(styleClass() + " calendar");
	setSelectionMode(NoSelection);
}

WWidget* TimesheetCalendar::renderCell(WWidget* widget, const WDate& date)
{
	if (!widget) 
		widget = new CalendarCell();
		
	CalendarCell* cc = static_cast<CalendarCell*> (widget);
	cc->update(date);
		
	return cc;
}
