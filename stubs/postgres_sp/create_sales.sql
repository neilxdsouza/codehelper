create table sales
(sales_code serial unique,
 quantity int,
 price	float,
 itemno int);

insert into sales (quantity, price, itemno) values (1, 12, 1);
insert into sales (quantity, price, itemno) values (2, 11, 2);
insert into sales (quantity, price, itemno) values (3, 21, 3);
