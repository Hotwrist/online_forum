/* Copyright(c) 2023 Odey John Ebinyi
*  All Rights Reserved
* 
*  Description: This file displays the thread page. That is the list of threads created by the admin.
*/

#include <iostream>
#include <cgicc/CgiDefs.h>
#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTMLClasses.h>
#include <cgicc/HTTPRedirectHeader.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <string>
#include "../is_user_loggedIn/check_online.h"

// The MySQL server is hosted locally. To connect to it,
// it uses 127.0.0.1 and port 3306 
#define DB_HOST_URL "tcp://127.0.0.1:3306"

using namespace cgicc;
using namespace std;
using namespace sql;


/*The error Function
	Takes one parameter for the error message.
	It diplays the error message  alerting the user of the error.
*/
void error(string err_message)
{
	try
	{
		cout << HTTPHTMLHeader() << endl;
		cout << html() << head(title("Oxford Brookes Online Forum")) << endl;
		cout << head() << endl;
		cout << body();
	
		cout << script() << endl;
		cout << "alert('"+ err_message +"');" << endl;
		cout << script() << endl;

		cout << body() << endl;

		cout << html();	
        }
        catch(exception &e)
        {
        	cout << "Please try reloading the web page again." << endl; 
        }
       
        exit(EXIT_SUCCESS);
}

/* The error_redirect Function
	It takes one parameter for the error message to display.
	It pops the error message on the screen and redirects the user to the start page.
*/
void error_redirect(string err_message)
{
	try
	{
		cout << HTTPHTMLHeader() << endl;
		cout << html() << head(title("Oxford Brookes Online Forum")).add(meta().set("http-equiv", "refresh").set("content", "0; URL=../")) << endl;
		cout << head() << endl;
		cout << body();
	
		cout << script() << endl;
		cout << "alert('"+ err_message +"');" << endl;
		cout << script() << endl;

		cout << body() << endl;

		cout << html();	
        }
        catch(exception &e)
        {
        	cout << "Please try reloading the web page again!." << endl; 
        }
       
        exit(EXIT_SUCCESS);
}

void redirect_to_login()
{
	cout << HTTPRedirectHeader("..");
}

/* The make_query Function
	This is used to execute SELECT statements, which returns an SQL result.
    It has a single string parameter containing the SQL query.
	After successful execution, it returns SQL::ResultSet
*/
ResultSet *make_query(string query) 
{
    try 
    {
        Driver *driver;
        Connection *con;
        Statement *stmt;
        ResultSet *res;

        driver = get_driver_instance();
        
        // creates a database connection using the driver. The parameters to the driver is the :
		// DB_HOST_URL: MySQL server IP address and port, 
		// the user 'forum' that is allowed access to the thread table, and
		// lastly, its password.
        con = driver->connect(DB_HOST_URL, "forum", "uJkY%%usArs##d@^&)&@!*Tlo>Y");
        
        /* select appropriate database schema and connect to it. (We are making use of the 'Users' schema created using create_database.sql file) */
        con->setSchema("Users");

        stmt = con->createStatement();
        res = stmt->executeQuery(query);
        return res;
    }
    catch (sql::SQLException& e) 
    {
        error_redirect("ERROR: Database refused to connect!!!.");
        exit(EXIT_SUCCESS);
    }
}


/* The oxford_brookes_forum_home_page()
 * It takes in a single string parameter, username. Since only the admin is allowed to create and
 * delete threads, the username is used to check if the user is admin, it then displays the create and 
 * delete threads button. Otherwise nothing these 2 buttons are not displayed if its an ordinary user.
*/
void oxford_brookes_forum_home_page(string username)
{
	ResultSet *res = make_query("SELECT * FROM threads");
	
	try 
	{
        	cout << HTTPHTMLHeader() << endl;
        	cout << html() << head(title("Oxford Brookes Online Forum")).add(link().set("rel", "stylesheet").set("href", "../static_files/css/main.css")) << endl;
        	
        	cout << body().set("class", "forum-home-page-body") << endl;
        	
        	cout << h1("Classroom Introduction Threads").set("id", "forum-home-page-h1");
        	
        	if(username == "admin")
        	{
				cout << cgicc::div().set("class", "create-new-thread-btn") << endl;
				cout << a().set("href", "create_delete_thread/").set("id", "create-new-thread-link") << "Create Thread" << a() << endl;
				cout << cgicc::div() << endl;

				cout << cgicc::div().set("class", "delete-threads-btn") << endl;
				cout << a().set("href", "create_delete_thread/delete_threads.cgi").set("id", "create-new-thread-link") << "Delete Threads" << a() << endl;
				cout << cgicc::div() << endl;						
		}
          	
		cout << ol().set("class", "ordered-thread-list") << endl;

            	/* fetch the data : retrieve all the rows in the result set */
		while(res->next())
		{
			cout << "<li>" << endl;
			cout << a().set("href", "thread_posts.cgi?thread="+res->getString("thread_name")+"&username="+username).set("class", "thread-link") << res->getString("thread_name") << a() << endl;
			cout << "</li>" << endl;
		}

	        cout << ol() << endl;
	        cout << "<br>" << endl;

		cout << cgicc::div().set("class", "footer") << endl;
		cout << h3("Odey John Ebinyi: Oxford Brookes Online Forum (c) 2023");
		cout << a().set("href", "#").set("class", "rights-reserved-link") << "All Rights Reserved" << a() << endl;
		cout << cgicc::div() << endl;	

        	cout << body() << endl;
        	cout << html();
    	}
    	catch (exception& e) 
    	{
        	error("Please try reloading the web page.");
    	}
}

/* The main Function
	This receives the username through a POST request.
	It then calls the search_companies function with the argument of the username received.
	This is done to search for companies requiring the applicant skill.
*/
int main()
{	
	Cgicc form_data;
	string username = "";
	
	form_iterator fi = form_data.getElement("username");  
   	if( !fi->isEmpty() && fi != (*form_data).end()) 
   	{  
      		username = **fi; 
      		
			if(is_online(username))
				oxford_brookes_forum_home_page(username);
			else
				redirect_to_login();
    }
}
