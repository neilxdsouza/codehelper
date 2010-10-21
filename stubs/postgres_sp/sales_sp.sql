CREATE or replace FUNCTION extended_sales1(p_itemno int) RETURNS TABLE(quantity int, total double precision) AS $$
BEGIN
    RETURN QUERY SELECT quantity, quantity * price FROM sales WHERE itemno = p_itemno;
END;
$$ LANGUAGE plpgsql;
