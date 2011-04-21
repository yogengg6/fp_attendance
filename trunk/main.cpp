#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/statement.h>
#include <ctime>

using namespace std;
using namespace sql;

void RunConnectMySQL()   
{  
	mysql::MySQL_Driver *driver;
	Connection *con;
	Statement *state;
	ResultSet *result;
	driver = sql::mysql::get_driver_instance();
	con = driver->connect("tcp://172.16.81.156:3306", "moodle", "moodle");  
	state = con->createStatement();
	state->execute("USE moodle");
	result = state->executeQuery("SELECT * FROM mdl_user LIMIT 10");
	while(result->next())  
	{    
		string name = result->getString("username");  
		string idnumber = result->getString("idnumber");
		cout << name << " ---- " << idnumber << endl;  
	}
	delete state;
	delete con;  
}  
int main()  
{  
	time_t bef = time(NULL);
	RunConnectMySQL();  
	time_t aft = time(NULL);
	cout << "Total cast time = " << aft - bef << " ms" << endl;
	return 0;  
}  