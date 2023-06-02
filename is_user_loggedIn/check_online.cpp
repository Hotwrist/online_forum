/* Copyright(c) 2023 Igbe Chijioke
*  All Rights Reserved
*/

#include "check_online.h"

bool user_is_online = false;

bool is_online(string username)
{
	try 
    {
        Driver* driver;
        Connection *con;
        PreparedStatement *prep_stmt;
        ResultSet *res;

        driver = get_driver_instance();
        
        // creates a database connection using the driver. The parameters to the driver is the :
		// DB_HOST_URL: MySQL server IP address and port, 
		// the user 'login' that is allowed to modify the login table, and
		// lastly, its password.
        con = driver->connect(DB_HOST_URL, "login", "xcv6ertwq2#$!@098ujtyvbxmsk");
        con->setSchema("Users");

        prep_stmt = con->prepareStatement("SELECT online FROM login WHERE username=?");
        prep_stmt->setString(1, username);
        
        res = prep_stmt->executeQuery();
        
        while (res->next()) 	// fetch data based on the above query.
        {
			// retrieve the value of the 'online' column in the login table related to the username specified.
			user_is_online = res->getBoolean("online"); 
			
			delete res;
			con->close();
			delete con;
			delete prep_stmt;
					
			break;
		}
    }
    catch (sql::SQLException& e) 
    {
        return false;
    }

	if(user_is_online) return true;
	
	return false;
}
