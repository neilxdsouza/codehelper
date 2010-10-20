select field1, field2, rank as my_rank from 
	(
	 select field1, field2, rank()  over (order by field1, field2) from temp1
	) stage1 
where rank >= 4 and rank <=8;
