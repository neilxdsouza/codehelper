create or replace function food3(p1 int, p2 int)
returns table (
food_category varchar(20),
food_name varchar(20),
my_rank bigint
) as $$
begin
return query 
	select stage1.* from 
	(
	 select food_category, food_name, rank()  over (order by food_category, food_name) from food
	) stage1;
	--where rank >= 4 and rank <=8;
end 
$$ language plpgsql;
