
select field1, field2, rank as my_rank from 
	(
	 select field1, field2, rank()  over (order by field1, field2) from temp1
	) stage1 
where rank >= 4 and rank <=8;


select field1, field2, my_rank from 
	(
	 select field1, field2, rank()  over (order by field1, field2)as  my_rank from temp1
	) stage1 
where my_rank >= 4 and my_rank <=8;


create function test1(p1 int, p2 int)
returns table (
field1 varchar(20),
field2 varchar(20),
) as $$
return query 
	select field1, field2, rank as my_rank from 
	(
	 select field1, field2, rank()  over (order by field1, field2) from temp1
	) stage1 
	where rank >= 4 and rank <=8;
end 
$$ language plpgsql;
