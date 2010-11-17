
CREATE TABLE Project : UI_GROUP (Project_Admin) (
	Project_Code int: primary_key UI_VIEW UI_SELECT,
	Project_Number varchar(20): unique UI_VIEW UI_SELECT,
	Project_Name varchar(250): unique UI_VIEW UI_SELECT,
	Project_Short_Description varchar(2000): not null,
	Project_Entry_Date datetime: not null,
	Project_Start_Date datetime: null,
	Project_End_Date datetime: null,
	Project_Commission_Date datetime: not null UI_VIEW UI_SELECT,
	Project_Lost_Date datetime: not null,
	Project_Description varchar(5000): null,
	Revenue_Start_Date datetime: not null,
	Project_Status_Code int: references Project_Status(Project_Status_Code),
	Employee_Code int: references Employee(Employee_Code),
	ClientContactPerson_Code int: references ClientContactPerson(ClientContactPerson_Code),
	Exchange_Master_Code int: references Exchange_Master(Exchange_Master_Code),
	Project_Type_Code int: references Project_Type(Project_Type_Code)
	);

CREATE TABLE ClientContactPerson: UI_GROUP (Client) (
	ClientContactPerson_Code int:  PRIMARY_KEY UI_VIEW UI_SELECT,
	ClientContactPerson_Name varchar(250): NOT NULL SEARCH_KEY UI_VIEW UI_SELECT,
	Client_Code int: REFERENCES Client(Client_Code),
	Designation varchar(150): NULL,
	Email1 varchar(250): not null,
	Email2 varchar(250): null,
	Phone varchar(50): NULL,
	Extention varchar(50): NULL,
	Mobile varchar(50): NULL
);

CREATE TABLE Client: UI_GROUP (Client) (
	Client_Code int : PRIMARY_KEY UI_VIEW UI_SELECT,
	Client_Name varchar(250):  UNIQUE SEARCH_KEY UI_VIEW UI_SELECT,
	ClientGroup_Code int: REFERENCES ClientGroup(ClientGroup_Code),
	Address1 varchar(500) :NULL,
	Address2 varchar(500) :NULL,
	City varchar(250) : NOT NULL,
	Pin varchar(150) : NULL,
	POBOX varchar(50) : NULL,
	Country_Code int:  references Country(Country_Code),
	Phone1 varchar(50):  NULL,
	Phone2 varchar(50):  NULL,
	Fax varchar(50): NULL,
	Website varchar(250): NULL,
	isInternal bit: NULL
);

create table Country :UI_GROUP (Client) (
	Country_Code int: PRIMARY_KEY UI_VIEW UI_SELECT,
	Country_Short_Name varchar(5): UNIQUE UI_VIEW UI_SELECT,
	Country_Name varchar(250): UNIQUE);

CREATE TABLE ClientGroup: UI_GROUP(Client) (
	ClientGroup_Code int : PRIMARY_KEY UI_VIEW UI_SELECT,
	ClientGroup_Name varchar(250) : UNIQUE  UI_VIEW UI_SELECT
);





CREATE TABLE Employee: UI_GROUP (Employee) (
      Employee_Code int: primary_key UI_VIEW UI_SELECT,
      ForeName varchar(250) : not null UI_VIEW UI_SELECT,
      MiddleName varchar(250) : not null UI_VIEW UI_SELECT,
      LastName varchar(250) : not null UI_VIEW UI_SELECT,
      EmployeeStatus composite:	references many EmployeeStatus(Employee_Code),
      Gender bit : not null,
      Title varchar(150) : not null,
      Prefered_Name varchar(150) : not null,
      Date_of_Birth datetime : not null,
      Previous_Employee_Code int : not null,
      HireDate datetime : not null
);
 
CREATE TABLE EmployeeStatus: UI_GROUP(Employee) (
      EmployeeStatus_Code int : primary_key UI_VIEW UI_SELECT,
      Employee_Code int : not null references Employee(Employee_Code) UI_VIEW UI_SELECT,
      Position_Code int : not null,
      Date_appointed_to_Position datetime : not null,
      Division_Code int : not null,
      Is_head_of_Division bit : not null,
      Comments varchar(200) : not null,
      isCurrent bit : not null
);


CREATE TABLE Division: UI_GROUP(Employee) (
      Division_Code int : primary_key UI_VIEW UI_SELECT,
      Division_Name varchar(150) : not  null UI_VIEW UI_SELECT,
      Division_Head int : not null,
      Part_of_Division int : not null,
      Lft int : not null invisible,
      Rgt int : not null invisible,
      Deleted bit: null invisible
);

CREATE TABLE Designation : UI_GROUP (Employee) (
	Designation_Code int : primary_key UI_VIEW UI_SELECT,
	Designation_Title varchar(150): not null UI_VIEW UI_SELECT,
	DesignationGroup_Code int: not null references DesignationGroup(DesignationGroup_Code)
	);

CREATE TABLE DesignationGroup : UI_GROUP (Employee) (
	DesignationGroup_Code int : primary_key UI_VIEW UI_SELECT,
	DesignationGroup_Title varchar(150): not null UI_VIEW UI_SELECT
	);

CREATE TABLE TimeCost : UI_GROUP (Timesheet_Admin) (
	TimeCost_Code int: primary_key,
	DesignationGroup_Code int: references DesignationGroup(DesignationGroup_Code),
	Hourly_Rate double: not null
	);

CREATE TABLE Activity : UI_GROUP (Timesheet_Admin) (
	Activity_Code int: primary_key UI_VIEW UI_SELECT,
	Activity_Name varchar(150): not null UI_VIEW UI_SELECT,
	Activity_Description varchar(2000): not null
	);

CREATE TABLE DesignationActivity : UI_GROUP (Timesheet_Admin) (
	DesignationActivity_Code int: primary_key UI_VIEW UI_SELECT,
	Activity_Code int: references Activity(Activity_Code) UI_VIEW UI_SELECT,
	DesignationGroup_Code int: references DesignationGroup(DesignationGroup_Code)
	);

create Table Currency : UI_GROUP (Exchange_Rates) (
	Currency_Code int: primary_key,
	Currency_Short_Name varchar(5): not null UI_VIEW UI_SELECT,
	Currency_Name varchar(200): null);

create Table Exchange_Master: UI_GROUP (Exchange_Rates) (
	Exchange_Master_Code int: primary_key UI_VIEW UI_SELECT,
	Currency_Code int: references Currency(Currency_Code) UI_VIEW UI_SELECT,
	Exchange_Details composite: references many Exchange_Details(Exchange_Details_Code),
	Exchange_Master_Date datetime: not null UI_VIEW UI_SELECT,
	Exchange_Master_Comments varchar(250): not null
	);

create table Exchange_Details: UI_GROUP (Exchange_Rates) (
	Exchange_Details_Code int: primary_key   UI_VIEW UI_SELECT,
	Exchange_Master_Code int: references Exchange_Master(Exchange_Master_Code),
	Exchange_Rate double: not null UI_VIEW UI_SELECT,
	Exchange_Details_Date datetime: not null UI_VIEW UI_SELECT,
	Comments varchar(250): not null);

create Table Project_Type : UI_GROUP (Project_Admin) (
	Project_Type_Code int: primary_key UI_VIEW UI_SELECT,
	Project_Type_Short_Description varchar(100): not null UI_VIEW UI_SELECT,
	Project_Type_Description varchar(1000): not null);

create Table Project_Status: UI_GROUP (Project_Admin) (
	Project_Status_Code int: primary_key UI_VIEW UI_SELECT,
	Project_Status_Description varchar(250): not null UI_VIEW UI_SELECT
	);
