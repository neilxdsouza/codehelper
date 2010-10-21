#include <postgresql/libpq-fe.h>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <cstdio>
#include <cstring>
#include <sstream>
class ConnCloser
{
public:
	void operator() (PGconn * conn)
	{
		PQfinish(conn);
	}
};
class MallocDeleter
{
public:
	void operator() (char * ptr)
	{
		free(ptr);
	}
};
static void
exit_nicely(PGconn *conn)
{
	PQfinish(conn);
	exit(1);
}

PGconn * GetPGConn()
{
	const char  *conninfo;
	PGconn      *conn;
	conninfo = "dbname=nxd port=5432 user=nxd password=nxd";
	conn = PQconnectdb(conninfo);
	if (PQstatus(conn) != CONNECTION_OK)
	{
		fprintf(stderr, "Connection to database failed: %s",
			PQerrorMessage(conn));
		exit_nicely(conn);
	}
	return conn;

}
/*
int InsertCountry(BizCountry & country)
{	boost::shared_ptr<PGconn> conn(GetPGConn(), ConnCloser());
	std::vector<boost::shared_ptr<char> > char_ptr_vec(3);
	const char * paramValues[3];
	std::stringstream ss_param_values[3];
	ss_param_values[0] << country.Get_Country_Short_Name();
	boost::shared_ptr<char> s_ptr0(strdup(ss_param_values[0].str().c_str()), MallocDeleter());
	char_ptr_vec.push_back(s_ptr0);
	paramValues[0]=s_ptr0.get();
	ss_param_values[1] << country.Get_Country_Name();
	boost::shared_ptr<char> s_ptr1(strdup(ss_param_values[1].str().c_str()), MallocDeleter());
	char_ptr_vec.push_back(s_ptr1);
	paramValues[1]=s_ptr1.get();
	PGresult *res=PQexecParams(conn.get(), 
		"select * from sp_Country_insert_Country("
			"$1::varchar,$2::varchar)", 2, NULL, paramValues, NULL, NULL,0);
	if (PQresultStatus(res) != PGRES_TUPLES_OK){
		int res_status = PQresultStatus(res);
		printf("res_status=%d, PGRES_COMMAND_OK = %d, PGRES_TUPLES_OK=%d\n",
			res_status, PGRES_COMMAND_OK, PGRES_TUPLES_OK);
		fprintf(stderr, "insert employee failed: %s", PQerrorMessage(conn.get()));
		PQclear(res);
		exit_nicely(conn.get());
	} else {
		int nTuples = PQntuples(res);
		int nFields = PQnfields(res);
		printf( "nTuples: %d, nFields=%d\n", nTuples, nFields);
		for(int i=0; i<nFields; ++i){
			char * fname=PQfname(res, i);
			printf("fname: %s\n", fname);
		}
		char * value=PQgetvalue(res, 0, 0);
		printf("value: %s\n", value);
	}

}
*/

void SelectCountry()
{
	boost::shared_ptr<PGconn> conn(GetPGConn(), ConnCloser());
	std::vector<boost::shared_ptr<char> > char_ptr_vec(2);
	const char * paramValues[2];
	std::stringstream ss_param_values[2];
	ss_param_values[0] << "1";
	boost::shared_ptr<char> s_ptr0(strdup(ss_param_values[0].str().c_str()), MallocDeleter());
	char_ptr_vec.push_back(s_ptr0);
	paramValues[0]=s_ptr0.get();

	ss_param_values[1] << "9";
	boost::shared_ptr<char> s_ptr1(strdup(ss_param_values[1].str().c_str()), MallocDeleter());
	char_ptr_vec.push_back(s_ptr1);
	paramValues[1]=s_ptr1.get();
	PGresult *res = PQexec(conn.get(), "BEGIN");
	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		fprintf(stderr, "BEGIN command failed: %s", PQerrorMessage(conn.get()));
		PQclear(res);
		exit(1);
	}
	PQclear(res);


	res=PQexecParams(conn.get(), 
		"declare myportal cursor for select * from food3("
			"$1::int,$2::int)", 2, NULL, paramValues, NULL, NULL,0);

	if (PQresultStatus(res) != PGRES_COMMAND_OK){
		int res_status = PQresultStatus(res);
		printf("res_status=%d, PGRES_COMMAND_OK = %d, PGRES_TUPLES_OK=%d\n",
			res_status, PGRES_COMMAND_OK, PGRES_TUPLES_OK);
		fprintf(stderr, "insert employee failed: %s", PQerrorMessage(conn.get()));
		PQclear(res);
		//exit_nicely(conn.get());
	} else {
		PQclear(res);
		res = PQexec(conn.get(), "FETCH ALL in myportal");
		if (PQresultStatus(res) != PGRES_TUPLES_OK) {
			fprintf(stderr, "FETCH ALL failed: %s", PQerrorMessage(conn.get()));
			PQclear(res);
			//exit_nicely(conn);
		}
		int nTuples = PQntuples(res);
		int nFields = PQnfields(res);
		printf( "nTuples: %d, nFields=%d\n", nTuples, nFields);
		for (int i=0; i<nFields; ++i) {
			char * fname=PQfname(res, i);
			printf("%-20s", fname);
		}
		printf("\n");
		for (int i=0; i<nTuples; ++i) {
			for (int j=0; j<nFields; ++j) {
				printf("%-20s", PQgetvalue(res, i, j) );
			}
			printf("\n");
		}
		char * value=PQgetvalue(res, 0, 0);
		printf("value: %s\n", value);
		PQclear(res);
	}
}

int main()
{
	SelectCountry();
}
