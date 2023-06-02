/* Copyright(c) 2023 Igbe Chijioke
*  All Rights Reserved
* 
*  Description: This file displays the HTML page for all thread posts made by users.
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
        	cout << "Please try reloading the web bpage again." << endl; 
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
        	cout << "Please try reloading the web apage again!." << endl; 
        }
       
        exit(EXIT_SUCCESS);
}

/* The make_query Function
	This is used to execute SELECT statements, which returns an SQL result.
    It has 2 parameters containing the SQL query and thread.
	After successful execution, it returns SQL::ResultSet
*/
ResultSet *make_query(string query, string thread) 
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
		// the user 'forum' that is allowed access to the thread_posts table, and
		// lastly, its password.
        con = driver->connect(DB_HOST_URL, "forum", "uJkY%%usArs##d@^&)&@!*Tlo>Y");
        
        /* select appropriate database schema and connect to it. (We are making use of the 'Users' schema created using create_database.sql file) */
        con->setSchema("Users");

        prep_stmt = con->prepareStatement(query);
        
        prep_stmt->setString(1, thread);
        res = prep_stmt->executeQuery();
        
        return res;
    }
    catch (sql::SQLException& e) 
    {
        error_redirect("ERROR: Database refused to connect!!!.");
        exit(EXIT_SUCCESS);
    }
}

// The show_thread_posts() function
// It takes in 2 parameters -- thread, and username and displays all the thread posts
// related to a specific thread.
void show_thread_posts(string thread, string username)
{
	ResultSet *res = make_query("SELECT thread_post, thread_post_id, username FROM thread_posts WHERE fk_thread_name=?", thread);
	try 
	{
        	cout << HTTPHTMLHeader() << endl;
        	cout << html() << head(title("Oxford Brookes Online Forum")).add(link().set("rel", "stylesheet").set("href", "../static_files/css/main.css")) << endl;
        	
        	cout << body().set("class", "forum-home-page-body") << endl;
        	
        	cout << h1("Thread: "+thread).set("id", "forum-home-page-h1");
          	
          	cout << cgicc::div().set("class", "make-a-post-btn") << endl;
			cout << a().set("href", "create_delete_post/?username="+username+"&thread_name="+thread).set("id", "make-a-post-link") << "Make a Post" << a() << endl;
			cout << cgicc::div() << endl;
          	
			cout << ol().set("class", "ordered-post-list") << endl;
			cout << h4("Posts by Users:") << endl;
			
            /* fetch the data : retrieve all the rows in the result set */
            while(res->next())
            {
                cout << "<li>" << endl;
                cout << cgicc::div().set("class", "users-profile-pic-post") << endl;
				cout << cgicc::div() << endl;
				cout << p("&#176; by "+res->getString("username")).set("class", "fk-username") << endl;
                cout << p(res->getString("thread_post")).set("class", "thread-post") << endl;
                
                string author_of_post = res->getString("username");
                short unsigned int post_id = res->getInt("thread_post_id");
                
                // The following sequence of if-block statements are needed to allow only the admin to be able
                // to delete his own post, and/or post of others. And also for other users to delete only their own post and not
                // the admin post or post of other users.
                
                // This is for the admin post. If the user is admin and the username on the address bar is admin, then the admin can delete
                // his own post.
                if((author_of_post == "admin") && (username == "admin"))
                {
					cout << a().set("href", "create_delete_post/delete_post.cgi?username="+author_of_post+"&id="+to_string(post_id)+"&thread="+thread).set("class", "delete-post") << "Delete" << a() << endl;
				}
				
				// If the author of the post is not admin and username on the address bar is admin, then the 
				// admin can delete the post of others and his own post also.
				if((author_of_post != "admin") && (username == "admin"))
				{
					cout << a().set("href", "create_delete_post/delete_post.cgi?username="+username+"&id="+to_string(post_id)+"&thread="+thread).set("class", "delete-post") << "Delete" << a() << endl;
               	}
               	
               	// If the author of the post is not admin and username on the address bar is not admin and the username on the address bar
               	// is the same as the author of the post, then the current user can delete his very own post and not the post of others.
               	if((author_of_post != "admin") && (username != "admin") && (username == author_of_post))
               	{
					cout << a().set("href", "create_delete_post/delete_post.cgi?username="+author_of_post+"&id="+to_string(post_id)+"&thread="+thread).set("class", "delete-post") << "Delete" << a() << endl;
               	}
               	
               	cout << "</li>" << endl;
            }

            cout << ol() << endl;
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
        	error("Please try reloading the web zpage.");
    	}
}

/* The main Function
	This receives the username and thread.
	It then calls the is_online() function to check if the user is online. If the user 
	is online, the list of thread posts related to the specific thread are shown to the user.
	If the user is offline, he/she is redirected to the logi page.
*/
int main()
{		
	Cgicc form_data;
	string thread = "";
	string username = "";
	
	form_iterator fi = form_data.getElement("thread");  
	
   	if (!fi->isEmpty() && fi != (*form_data).end()) 
   	{  
      		thread = **fi; 
      		
      		fi = form_data.getElement("username");
        	if(!fi->isEmpty() && fi != (*form_data).end()) 
				username = **fi;
				
			if(is_online(username))
				show_thread_posts(thread, username);
			else
				error_redirect("WARNING: You are not logged in and not authorized to access this page.");
	}
}
