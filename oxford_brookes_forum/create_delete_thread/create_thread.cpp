/* Copyright(c) 2023 Igbe Chijioke
*  All Rights Reserved
*  
*  Description: This file helps an admin to create a thread. The admin 
* 				is the only user with the permission to create and delete thread(s).
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
#include <cppconn/prepared_statement.h>
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
		cout << "alert(\"WARNING: You can\'t create a thread. You are not logged in as Admin.!!\");" << endl;
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

// After successful thread creation by the admin, redirect the admin to the threads page.
void redirect_to_threads_page()
{
	cout << HTTPRedirectHeader("../?username=admin") << endl;
}

/*  The save_thread() Function
	This function has a single string parameter.
	
	It saves the thread created by the admin.
*/
void save_thread(string thread_title)
{
	try
	{
		Driver *driver;
		Connection *con;
		PreparedStatement *prep_stmt = NULL;

		driver = get_driver_instance();
		
		// creates a database connection using the driver. The parameters to the driver is the :
		// DB_HOST_URL: MySQL server IP address and port, 
		// the user 'forum' that is allowed access to the threads table, and
		// lastly, its password.
		con = driver->connect(DB_HOST_URL, "forum", "uJkY%%usArs##d@^&)&@!*Tlo>Y"); 
		
		/* select appropriate database schema and connect to it. (We are making use of the 'Users' schema created using create_database.sql file) */
		con->setSchema("Users");

		prep_stmt = con->prepareStatement ("INSERT INTO threads (thread_name) VALUES (?)");
	
		prep_stmt -> setString (1, thread_title);
		prep_stmt -> executeUpdate();
	
		redirect_to_threads_page();
		exit(0);
		
		/* Clean up */
		delete prep_stmt;
		con->close();
		delete con;
	}
	catch (sql::SQLException& e) 
	{
        	error("ERROR: The connection to the database was not successful!.");
    }
	
	return;
}

/*  The main Funcion
    
	It receives a username and thread_title. It then calls the is_online() function 
	to check if the admin is online and not another malicious user trying to perform
	the action. If admin is online, the thread is saved by calling the save_thread() function.
*/
int main()
{	
	Cgicc form_data;
	string username = "admin";
	string thread_title = "";
	
	form_iterator fi = form_data.getElement("thread-title");  
   	if( !fi->isEmpty() && fi != (*form_data).end()) 
   	{  
      		thread_title = **fi; 
      		
			if(is_online(username))
				save_thread(thread_title);
			else
				redirect_to_login();
    }
	
	return 0;
}
