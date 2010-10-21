create or replace function test1(p1 int, p2 int)
returns table (
field1 varchar(20),
field2 varchar(20),
my_rank int
) as $$
begin
return query 
	select field1, field2, stage1.temp_rank as my_rank from 
	(
	 select field1, field2, rank()  over (order by field1, field2) as temp_rank from temp1
	) stage1 
	where rank >= 4 and rank <=8;
end 
$$ language plpgsql
