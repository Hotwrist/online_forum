/* Copyright(c) 2023 Odey John Ebinyi
*  All Rights Reserved
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
#include <cppconn/statement.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <openssl/sha.h>
#include <iomanip>
#include <string>
#include <cstdlib>

// The MySQL server is hosted locally. To connect to it,
// it uses 127.0.0.1 and port 3306 
#define DB_HOST_URL "tcp://127.0.0.1:3306"

using namespace cgicc;
using namespace std;
using namespace sql;


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

// This function has a single string parameter.
// On the event that a user's session expires, it clears
// the 2FA, email, and TOTP table in the database. This is needed
// so that the user can not supply old login details to gain access into 
// the web app. 
void clear_user_2fa_email_totp(string username)
{
	try 
	{
        	Driver *driver;
        	Connection *con;
        	PreparedStatement *prep_stmt;
        	
        	driver = get_driver_instance();
        	
        	// creates a database connection using the driver. The parameters to the driver is the :
			// DB_HOST_URL: MySQL server IP address and port, 
			// the user 'admin' that is allowed to modify the 2FA, Email and TOTP table of the database, and
			// lastly, its password.
        	con = driver->connect(DB_HOST_URL, "logout", "u787%$pass##d@^&)097Tlo#$%*");
        	
        	/* select appropriate database schema and connect to it. (We are making use of the 'Users' schema created using create_database.sql file) */
        	con->setSchema("Users"); 

        	prep_stmt = con->prepareStatement("DELETE FROM 2FA WHERE fk_username=?");
        	prep_stmt->setString(1, username);
        	prep_stmt->execute();     	

        	prep_stmt = con->prepareStatement("DELETE FROM sent_emails WHERE name=?");
        	prep_stmt->setString(1, username);
        	prep_stmt->execute();
        	
        	prep_stmt = con->prepareStatement("DELETE FROM admin_email WHERE name=?");
        	prep_stmt->setString(1, username);
        	prep_stmt->execute();
        	
        	prep_stmt = con->prepareStatement("DELETE FROM TOTP WHERE fk_username=?");
        	prep_stmt->setString(1, username);
        	prep_stmt->execute();
        	
        	con->close();
        	delete con;
        	delete prep_stmt;
    	}
    	catch (sql::SQLException& e) 
    	{
    	    error_redirect("ERROR: Try repeating the action again!.");
    	    exit(EXIT_SUCCESS);
    	}
    	
}

// This function has a single string parameter.
// On the event that a user's session expires, it clears
// the Session table in the database. This is needed
// so that the user can not be allowed access with old session ID.
void clear_user_sessionID(string username)
{
	try 
	{
        	Driver *driver;
        	Connection *con;
        	PreparedStatement *prep_stmt;
        	
        	driver = get_driver_instance();
        	
        	// creates a database connection using the driver. The parameters to the driver is the :
			// DB_HOST_URL: MySQL server IP address and port, 
			// the user 'logout' that is allowed to modify the sessions table, and
			// lastly, its password.
        	con = driver->connect(DB_HOST_URL, "logout", "u787%$pass##d@^&)097Tlo#$%*");
        	
        	/* select appropriate database schema and connect to it. (We are making use of the 'Users' schema created using create_database.sql file) */
        	con->setSchema("Users");      	
        	
        	// Delete the approved company from the unapproved companies database. 
        	prep_stmt = con->prepareStatement("DELETE FROM Sessions WHERE fk_username=?");
        	prep_stmt->setString(1, username);
        	prep_stmt->execute();
        	
        	con->close();
        	delete con;
        	delete prep_stmt;
    	}
    	catch (sql::SQLException& e) 
    	{
    	    error_redirect("ERROR: Try repeating the action again!.");
    	    exit(EXIT_SUCCESS);
    	}
}

// This function displays the logout web page to the user. Telling the user
// that he has been successfully logged out.
void display_logout_msg()
{
    try 
    {
        cout << HTTPHTMLHeader() << endl;
        cout << html() << head(title("Oxford Brookes Online Forum")).add(link().set("rel", "stylesheet").set("href", "../static_files/css/main.css")) << endl;
        cout << body().set("class", "logout-body");
 
        cout << h1("Oxford Brookes Online Forum").set("id", "logout-h1") << endl;
        cout << p("You are logged out!!.").set("id", "logout-msg");
        
        cout << form().set("method", "post").set("action", "../") << endl;            	
        cout << input().set("type", "submit").set("value", "Login").set("class", "login-btn") << endl;
        cout << form() << endl;

        cout << body() << endl;
        cout << html();
    }
    catch (exception& e) 
    {
        cout << "This should not have happened. Please try reloading the page again." << endl; 
    }
}

// This function makes the user offline by setting the user's online column in the login table
// to zero.
void make_user_offline(string username)
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
			// the user 'login' that is allowed access and modification to the login and TOTP database, and
			// lastly, its password.
        	con = driver->connect(DB_HOST_URL, "login", "xcv6ertwq2#$!@098ujtyvbxmsk");
        	
        	/* select appropriate database schema and connect to it. (We are making use of the 'Users' schema created using create_database.sql file) */
        	con->setSchema("Users");
        		
        	if(username == "admin")
			{
					bool online = false;
            		prep_stmt = con->prepareStatement("SELECT online FROM login WHERE username=?");
            		prep_stmt->setString(1, username);
            		
            		res = prep_stmt->executeQuery();
            		
            		// fetch data from the database login, based on the query above
            		if (res->next()) 
            		{
                		online = res->getBoolean("online");
            		}
            		if(online)
            		{
            			//set admin to offline
                		prep_stmt = con->prepareStatement("UPDATE login SET online='0' WHERE username=?");
                		prep_stmt->setString(1, username);
                		prep_stmt->execute();
                		
                		// set totp to offline(i.e active)
                   		prep_stmt = con->prepareStatement("UPDATE TOTP SET online='0' WHERE fk_username=? AND online=?");
                   		prep_stmt->setString(1, username);
                   		prep_stmt->setBoolean(2, false);
                    	prep_stmt->execute(); 
                    	
                    	clear_user_2fa_email_totp(username);
                    	clear_user_sessionID(username);
                    	
                    	display_logout_msg();                   		
                	}
                	else
                	{
                		error_redirect("ERROR: You are offline!.");
                	}
        	}
        	
			else
			{
					bool online = false;
			
            		prep_stmt = con->prepareStatement("SELECT online FROM login WHERE username=?");
            		prep_stmt->setString(1, username);
            		res = prep_stmt->executeQuery();
            		
            		// fetch data from the database login, based on the query above
            		if (res->next()) 
            		{
                		online = res->getBoolean("online");
            		}
            		
            		if (online) 
            		{
						// set user to offline
						prep_stmt = con->prepareStatement("UPDATE login SET online='0' WHERE username=?");
						prep_stmt->setString(1, username);
                   		prep_stmt->execute();	
                   		
                       	clear_user_2fa_email_totp(username);
                    	clear_user_sessionID(username);
                    	
                   		display_logout_msg();
            		}
            		else
            		{
            			error_redirect("ERROR: You are offline.");
            		}
				}
		
			con->close();
    		delete con;
    		delete prep_stmt;
    		delete res;
	}
	catch (sql::SQLException& e) 
	{
        	error_redirect("ERROR: Please try reloading the web page.");
    }
    	
    return;
}

// The main function receives the username field through a POST request. It then calls
// the make_user_offline() function with the username as an argument.
int main()
{
	Cgicc form_data;
	string username = "";
	
	form_iterator fi = form_data.getElement("username");  
   	if( !fi->isEmpty() && fi != (*form_data).end()) 
   	{  
      		username = **fi; 
      		make_user_offline(username);
   	}
}
