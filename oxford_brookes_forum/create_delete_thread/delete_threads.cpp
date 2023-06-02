/* Copyright(c) 2023 Igbe Chijioke
*  All Rights Reserved
*  
*  Description: This file helps an admin to delete a thread or threads.
*/

#include <iostream>
#include <fstream>
#include <cgicc/CgiDefs.h>
#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTMLClasses.h>
#include <cgicc/HTTPRedirectHeader.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/connection.h>
#include "mysql_connection.h"

#include <openssl/sha.h>
#include <iomanip>
#include "../../is_user_loggedIn/check_online.h"

// The MySQL server is hosted locally. To connect to it,
// it uses 127.0.0.1 and port 3306  
#define DB_HOST_URL "tcp://127.0.0.1:3306"

using namespace cgicc;
using namespace std;
using namespace sql;

/* The error Function
	It takes one parameter for the error message to display.
	It pops the error message on the screen and redirects the user to the register page.
*/
void error(string err_message) 
{
    	cout << HTTPHTMLHeader() << endl;
		cout << html() << head(title("Oxford Brookes Online Forum")).add(meta().set("http-equiv", "refresh").set("content", "0; URL=.")) << endl;

       	cout << head() << endl;
       	cout << body();
        
       	cout << script() << endl;
       	cout << "alert('"+ err_message +"');" << endl;
       	cout << script() << endl;

       	cout << body() << endl;

		cout << html();	
   	
		exit(EXIT_SUCCESS);
}

/* The redirect_to_login Function
	This function redirects any user to the login page after successful registration.
	It alerts the user on successful registration and uses the HTTP redirect method
	to redirect the user to the login page.
*/
void redirect_to_login()
{
	try
	{
		cout << HTTPHTMLHeader() << endl;
		cout << html() << head(title("Oxford Brookes Online Forum")).add(meta().set("http-equiv", "refresh").set("content", "0; URL=../../")) << endl;
		cout << head() << endl;
		cout << body();
	
		cout << script() << endl;
		cout << "alert(\"WARNING: This is a dangerous action. You are not logged in as Admin!.\");" << endl;
		cout << script() << endl;

		cout << body() << endl;

		cout << html();	
        }
        catch(exception &e)
        {
        	cout << "Please, try reloading the web page." << endl; 
        }
       
        exit(EXIT_SUCCESS);
}

// After a successful thread deletion by the admin, redirect the admin to the 
// threads page.
void redirect_to_threads_page()
{
	cout << HTTPRedirectHeader("../?username=admin") << endl;
}

/*  The delete_threads() function
 *  It takes in no parameter and deletes all the threads at once.
*/
void delete_threads()
{
	try
	{
		Driver *driver;
		Connection *con;
		Statement *stmt = NULL;

		driver = get_driver_instance();
		
		// creates a database connection using the driver. The parameters to the driver is the :
		// DB_HOST_URL: MySQL server IP address and port, 
		// the user 'forum' that is allowed access to the threads table, and
		// lastly, its password.
		con = driver->connect(DB_HOST_URL, "forum", "uJkY%%usArs##d@^&)&@!*Tlo>Y"); 
		
		/* select appropriate database schema and connect to it. (We are making use of the 'Users' schema created using create_database.sql file) */
		con->setSchema("Users");
		
		stmt = con->createStatement();
		stmt->execute("DELETE FROM threads");
	
		redirect_to_threads_page();
		exit(0);
		
		/* Clean up */
		delete stmt;
		con->close();
		delete con;
	}
	catch (sql::SQLException& e) 
	{
        	error("ERROR: The connection to the database was not successful for the deletion!.");
    }
	
	return;
}

/*  The main Funcion
    
	Since the admin is the only user that can create and delete whole threads, the admin
	username, 'admin', is hardcoded and passed to the string variable, username.
	
	It then checks if admin is online. If admin is online, the delete_threads() function is called
	else he is redirected to login page.
*/
int main()
{	
	Cgicc form_data;
	string username = "admin";

	if(is_online(username))
		delete_threads();
	else
		redirect_to_login();
    
	return 0;
}
