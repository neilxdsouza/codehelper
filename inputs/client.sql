create table Country(
	Country_Code int: PRIMARY_KEY,
	Country_Short_Name varchar(5): UNIQUE,
	Country_Name varchar(250): UNIQUE);

CREATE TABLE ClientGroup(
	ClientGroup_Code int : PRIMARY_KEY,
	ClientGroup_Name varchar(250) : UNIQUE
);


CREATE TABLE Client(
	Client_Code int : PRIMARY_KEY,
	Client_Name varchar(250):  UNIQUE SEARCH_KEY,
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



CREATE TABLE ClientContactPerson(
	ClientContactPerson_Code int:  PRIMARY_KEY,
	ClientContactPerson_Name varchar(250): NOT NULL SEARCH_KEY,
	Client_Code int: REFERENCES Client(Client_Code),
	Designation varchar(150): NULL,
	Email1 varchar(250): not null,
	Email2 varchar(250): null,
	Phone varchar(50): NULL,
	Extention varchar(50): NULL,
	Mobile varchar(50): NULL
);

