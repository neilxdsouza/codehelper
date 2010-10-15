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
      EmpStatusID int : not null,
      Employee_Code int : not null,
      Position_Code int : not null,
      Date_appointed_to_Position datetime : not null,
      Division_Code int : not null,
      Is_head_of_Division bit : not null,
      Comments varchar(200) : not null,
      isCurrent bit : not null
);


CREATE TABLE Division(
      Division_Code int : not null,
      Division_Name varchar(150) : not  null,
      Division_Head int : not null,
      Part_of_Division int : not null,
      Lft int : not null invisible,
      Rgt int : not null invisible,
      Deleted bit: not null invisible
);


