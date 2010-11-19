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

#ifndef TIMESHEET_CALENDAR_H_
#define TIMESHEET_CALENDAR_H_


#include <Wt/WCalendar>

class TimesheetCalendar : public Wt::WCalendar
{
public: 
  TimesheetCalendar(Wt::WContainerWidget* parent);
protected:
  virtual Wt::WWidget* renderCell(Wt::WWidget* widget, const Wt::WDate& date);
 
private:
};

#endif //TIMESHEET_CALENDAR_H_
