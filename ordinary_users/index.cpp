/* Copyright(c) 2023 Igbe Chijioke
*  All Rights Reserved
* 
*  Description: This C++ File is responsible for displaying the
* 				profile page of each user.
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
#include <cppconn/prepared_statement.h>
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

// Redirects the user to the login page if the user is not logged in or offline.
void redirect_to_login()
{
	cout << HTTPRedirectHeader("..");
}

/* The make_query Function
	This is used to execute SELECT statements, which returns an SQL result.
    It has 2 parameters containing the SQL query and the username parameter
	After successful execution, it returns SQL::ResultSet
*/
ResultSet *make_query(string query, string username) 
{
    try 
    {
        Driver *driver;
        Connection *con;
        PreparedStatement *prep_stmt;
        ResultSet *res;

        driver = get_driver_instance();
        
        // creates a database connection using the driver. The parameters to the driver is the :
		// DB_HOST_URL: MySQL server IP address and port, 
		// the user 'login' that is allowed access to the login table of the database, and
		// lastly, its password.
        con = driver->connect(DB_HOST_URL, "login", "xcv6ertwq2#$!@098ujtyvbxmsk");
        
        /* select appropriate database schema and connect to it. (We are making use of the 'Users' schema created using create_database.sql file) */
        con->setSchema("Users");

        prep_stmt = con->prepareStatement(query);
        prep_stmt->setString(1, username);
        res = prep_stmt->executeQuery();
        return res;
    }
    catch (sql::SQLException& e) 
    {
        error_redirect("ERROR: Database refused to connect!!!.");
        exit(EXIT_SUCCESS);
    }
}


/* The user_profile() Function
	It takes one parametre: username of type string. 
	 
	This is displayed alongside the online notification dot.
	This function displays the user's profile information.
*/
void user_profile(string username)
{
	ResultSet *res = make_query("SELECT * FROM login WHERE username=?", username);
	
	try 
	{
        	cout << HTTPHTMLHeader() << endl;
        	cout << html() << head(title("Oxford Brookes Online Forum")).add(link().set("rel", "stylesheet").set("href", "../static_files/css/main.css")) << endl;
        	
        	cout << body().set("class", "user-profile-body") << endl;
        	
        	cout << h1("Profile Page").set("id", "profile-page");
        	
        	cout << form().set("method", "post").set("action", "../logout/?username="+username) << endl;
			cout << input().set("type", "submit").set("value", "Logout").set("class", "logout-btn") << endl;
			cout << form() << endl;
			
			cout << form().set("method", "post").set("action", "../oxford_brookes_forum/?username="+username) << endl;
			cout << input().set("type", "submit").set("value", "Forum").set("class", "forum-btn") << endl;
			cout << form() << endl;
			
			cout << cgicc::div().set("class", "online-dot") << endl;
        	cout << cgicc::div() << endl;
        	
        	cout << cgicc::div().set("class", "profile-pic") << endl;
        	cout << cgicc::div() << endl;
        	
        	cout << cgicc::div().set("class", "username-box") << endl;
        	cout << p("Username: "+username);
        	cout << cgicc::div() << endl;     	
        	
        	//cout << cgicc::div().set("class", "user-profile-details") << endl;
			cout << table().set("class", "container") << endl;
               
            cout << "<thead>" << endl;
            cout << "<tr>" << endl;
            cout << "<th><h1>Full Name</h1></th>" << endl;
            cout << "<th><h1>Username</h1></th>" << endl;
            cout << "<th><h1>E-mail</h1></th>" << endl;
            cout << "</tr>" << endl;
            cout << "</thead>" << endl;
                
            cout << "<tbody>" << endl;
            
            /* fetch the data : retrieve all the rows in the result set */
            while(res->next())
            {
                cout << "<tr>" << endl;
                cout << "<td>" << endl;
                cout << res->getString("fullname");
               	cout << "</td>" << endl;
             	cout << "<td>" << endl;
             	cout << res->getString("username");
                cout << "</td>" << endl;
                cout << "<td>" << endl;
             	cout << res->getString("email");
                cout << "</td>" << endl;
                cout << "</tr>" << endl;
            }
            cout << "</tbody>" << endl;
            cout << table() << endl;
            cout << "<br>" << endl;
   
			cout << cgicc::div() << endl;
			cout << h3("Igbe Chijioke: Oxford Brookes Online Forum (c) 2023");
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
	It then calls the is_online() function with the argument of the username to check if the user
	is online. If the user is online, he/she is shown his profile page. otherwise he/she is redirected
	to the login page.
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
				user_profile(username);
			else
				redirect_to_login();
   	}
}
