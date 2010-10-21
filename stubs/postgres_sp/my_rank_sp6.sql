create or replace function food6(p1 int, p2 int)
returns table (
p_food_code int,
p_food_category varchar(20),
p_food_name varchar(20),
my_rank bigint
) as $$
begin
return query 
	select stage1.* from 
	(
	 select food_code, food_category, food_name, rank()  over (order by food_code) from food
	) stage1;
	--where rank >= 4 and rank <=8;
end 
$$ language plpgsql;
