CREATE TABLE Employee(
      Employee_Code int: primary_key,
      EmployeeStatus composite:	references many EmployeeStatus(Employee_Code),
      Gender bit : not null,
      Title varchar(150) : not null,
      ForeName varchar(250) : not null,
      MiddleName varchar(250) : not null,
      LastName varchar(250) : not null,
      Prefered_Name varchar(150) : not null,
      Date_of_Birth datetime : not null,
      Previous_Employee_Code int : not null,
      HireDate datetime : not null
);
 
CREATE TABLE EmployeeStatus(
      EmployeeStatus_Code int : primary_key,
      Employee_Code int : not null references Employee(Employee_Code),
      Position_Code int : not null,
      Date_appointed_to_Position datetime : not null,
      Division_Code int : not null,
      Is_head_of_Division bit : not null,
      Comments varchar(200) : not null,
      isCurrent bit : not null
);


CREATE TABLE Division(
      Division_Code int : primary_key,
      Division_Name varchar(150) : not  null,
      Division_Head int : not null,
      Part_of_Division int : not null,
      Lft int : not null invisible,
      Rgt int : not null invisible,
      Deleted bit: null invisible
);

CREATE TABLE Designation (
	Designation_Code int : primary_key,
	Designation_Title varchar(150): not null,
	DesignationGroup_Code int: not null references DesignationGroup(DesignationGroup_Code)
	);

CREATE TABLE DesignationGroup (
	DesignationGroup_Code int : primary_key,
	DesignationGroup_Title varchar(150): not null
	);

CREATE TABLE TimeCost (
	TimeCost_Code int: primary_key,
	DesignationGroup_Code int: references DesignationGroup(DesignationGroup_Code),
	Hourly_Rate double: not null
	);

CREATE TABLE Activity (
	Activity_Code int: primary_key,
	Activity_Name varchar(150): not null,
	Activity_Description varchar(2000): not null
	);

CREATE TABLE DesignationActivity (
	DesignationActivity_Code int: primary_key,
	Activity_Code int: references Activity(Activity_Code),
	DesignationGroup_Code int: references DesignationGroup(DesignationGroup_Code)
	);


