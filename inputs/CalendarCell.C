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

#include "CalendarCell.h"

#include <boost/lexical_cast.hpp>

#include <Wt/WDate>
#include <Wt/WText>

using namespace Wt;

CalendarCell::CalendarCell()
	: WContainerWidget()
{
	//resize(100, 120);
	resize(50, 60);
	setStyleClass("cell");
	setToolTip(tr("calendar.cell.tooltip"));
	clicked().connect(this, &CalendarCell::showEntryDialog);
}

void CalendarCell::update( const WDate& date)
{
	date_ = date;
	clear();
	WString day;
	day += boost::lexical_cast<std::string>(date.day());
	if (date.day() == 1)
	day += " " + WDate::longMonthName(date.month());
	WText* header = new WText(day);
	header->setStyleClass("cell-header");
	addWidget(header);
}

void CalendarCell::showEntryDialog()
{
	WString title =
	tr("calendar.entry.title").arg(date_.toString("ddd, d MMM yyyy"));
}

void CalendarCell::showAllEntriesDialog()
{
	WString title =
	tr("calendar.cell.all-entries.title")
	.arg(date_.toString("ddd, d MMM yyyy"));
}
