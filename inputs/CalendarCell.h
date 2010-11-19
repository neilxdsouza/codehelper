// This may look like C code, but it's really -*- C++ -*-
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

#ifndef CALENDAR_CELL_H_
#define CALENDAR_CELL_H_


#include <Wt/WContainerWidget>
#include <Wt/WDate>

class CalendarCell : public Wt::WContainerWidget
{
	public:
	CalendarCell();

	void update(const Wt::WDate& date);

	Wt::WDate date() {return date_; }

	private:
	Wt::WDate date_;

	void showEntryDialog();
	void showAllEntriesDialog();
};

#endif //CALENDAR_CELL_H_
