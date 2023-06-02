/* Copyright(c) 2023 Odey John Ebinyi
*  All Rights Reserved
* 
*  Description: This C++ File helps the users to create posts.
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

// After a successful creation of a post by the user, redirect him/her to the posts page to see
// the posts he/she has added.
void redirect_to_posts_page(string username, string thread_name)
{
	cout << HTTPRedirectHeader("../thread_posts.cgi?username="+username+"&thread="+thread_name) << endl;
}

/*  The save_post() Function
    This saves the username, thread_name, and thread_post to the thread_posts database table.
    It uses prepared statement to validate the inputs preventing SQL Injections.
    After saving the valid inputs, it redirects any of the users it is working on, to the posts page.
*/
void save_post(string username, string thread_name, string thread_post)
{
	try
	{
		Driver *driver;
		Connection *con;
		PreparedStatement *prep_stmt;

		driver = get_driver_instance();
		
		// creates a database connection using the driver. The parameters to the driver is the :
		// DB_HOST_URL: MySQL server IP address and port, 
		// the user 'forum' that is allowed access to the thread_posts table, and
		// lastly, its password.
		con = driver->connect(DB_HOST_URL, "forum", "uJkY%%usArs##d@^&)&@!*Tlo>Y"); 
		
		/* select appropriate database schema and connect to it. (We are making use of the 'Users' schema created using create_database.sql file) */
		con->setSchema("Users");
		
		// add the details in the parameter to the thread_posts table.
		prep_stmt = con->prepareStatement ("INSERT INTO thread_posts(fk_thread_name, username, thread_post) VALUES(?,?,?)");

		prep_stmt->setString(1, thread_name);
		prep_stmt->setString(2, username);
		prep_stmt->setString(3, thread_post);
		prep_stmt->executeUpdate();
	
		redirect_to_posts_page(username, thread_name);
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
    
    It receives a user's username, thread_post, and thread_name.
    It then calls the is_online() function to check if the user is online. If he/she is online,
    the post they created is saved by calling save_post() function with the required arguments.
    
    If the user is not online, they are redirected to the login page to login first.
*/
int main()
{	
	Cgicc form_data;
	string username = "";
	string thread_post = "";
	string thread_name = "";
	
	form_iterator fi = form_data.getElement("username");  
   	if( !fi->isEmpty() && fi != (*form_data).end()) 
   	{  
      		username = **fi; 
      		
      		fi = form_data.getElement("thread_name");
        	if(!fi->isEmpty() && fi != (*form_data).end()) 
				thread_name = **fi;
           		
           	fi = form_data.getElement("thread-post");
        	if(!fi->isEmpty() && fi != (*form_data).end()) 
				thread_post = **fi;
      		
			if(is_online(username))
				save_post(username, thread_name, thread_post);
			else
				redirect_to_login();
    }
	
return 0;
}
