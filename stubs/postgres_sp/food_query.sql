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
