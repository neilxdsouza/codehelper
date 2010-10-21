create table food(
	food_code serial unique,
	food_category varchar(20),
	food_name varchar(20)
	);

insert into food (food_category, food_name) values ('fruit', 'tomato');
insert into food (food_category, food_name) values ('fruit', 'banana');
insert into food (food_category, food_name) values ('fruit', 'apple');

insert into food (food_category, food_name) values ('vegetable', 'cabbage');
insert into food (food_category, food_name) values ('vegetable', 'cauliflower');
insert into food (food_category, food_name) values ('vegetable', 'okra');

insert into food (food_category, food_name) values ('nuts', 'almonds');
insert into food (food_category, food_name) values ('nuts', 'hazelnuts');
insert into food (food_category, food_name) values ('nuts', 'pine-seeds');

select food_category, food_name, rank as my_rank from 
	(
	 select food_category, food_name, rank()  over (order by food_category, food_name) from food
	) stage1 
where rank >= 4 and rank <=8;

select food_category, food_name, my_rank from 
	(
	 select food_category, food_name, rank()  over (order by food_category, food_name)as  my_rank from food
	) stage1 
where my_rank >= 4 and my_rank <=8;
