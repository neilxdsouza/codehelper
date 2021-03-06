create function food1(p1 int, p2 int)
returns table (
food_category varchar(20),
food_name varchar(20),
my_rank int
) as $$
begin
return query 
	select food_category, food_name, my_rank from 
	(
	 select food_category, food_name, rank()  over (order by food_category, food_name) as my_rank from food
	) stage1 
	where rank >= 4 and rank <=8;
end 
$$ language plpgsql;
