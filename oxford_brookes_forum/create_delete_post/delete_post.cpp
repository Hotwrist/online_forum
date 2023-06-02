/* Copyright(c) 2023 Igbe Chijioke
*  All Rights Reserved
* 
*  Description: This file helps a user delete a post.
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
#include <string>
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

// After a successful post deletion, redirect the user to the post page.
void redirect_to_post_page(string username, string thread_name)
{
	cout << HTTPRedirectHeader("../thread_posts.cgi?username="+username+"&thread="+thread_name) << endl;
}

/*  The delete_post() Function
    This uses the username, post_id, and thread_name to delete a user's post from the 
	thread_posts database table. It uses the post_id to identify the specific post to delete.
*/
void delete_post(string username, string post_id, string thread_name)
{
	try
	{
		Driver *driver;
		Connection *con;
		Statement *stmt = NULL;

		driver = get_driver_instance();
		
		// creates a database connection using the driver. The parameters to the driver is the :
		// DB_HOST_URL: MySQL server IP address and port, 
		// the user 'forum' that is allowed to modify the thread_post table, and
		// lastly, its password.
		con = driver->connect(DB_HOST_URL, "forum", "uJkY%%usArs##d@^&)&@!*Tlo>Y"); 
		
		/* select appropriate database schema and connect to it. (We are making use of the 'Users' schema created using create_database.sql file) */
		con->setSchema("Users");

		stmt = con->createStatement();
		
		stmt->execute("DELETE FROM thread_posts WHERE thread_post_id='"+post_id+"'");
		
		redirect_to_post_page(username, thread_name);
		
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
    
	It receives username, thread_name, and post_id. It then calls the is_online() function
	to confirm if the user is online. This is a security approach to prevent any offline user from
	deleting a post. If the user is not online, he is redirected to the login page.
*/
int main()
{	
	Cgicc form_data;
	string username = "";
	string thread_name = "";
	string post_id = "";
	
	form_iterator fi = form_data.getElement("username");  
   	if( !fi->isEmpty() && fi != (*form_data).end()) 
   	{  
      		username = **fi; 
 
			fi = form_data.getElement("id");
			if( !fi->isEmpty() && fi != (*form_data).end()) 
			{
				post_id = **fi;
			}
			
			fi = form_data.getElement("thread"); 
			if( !fi->isEmpty() && fi != (*form_data).end()) 
			{
				thread_name = **fi;
			}			
			
			if(is_online(username))
					delete_post(username, post_id, thread_name);
			else
				redirect_to_login();
    }
    
	return 0;
}
