/* Copyright(c) 2023 Igbe Chijioke
*  All Rights Reserved
*  
* Description: This C++ file sets up the session cookies and validate the user.
*/

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>

#include <openssl/sha.h>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <chrono>

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <cgicc/CgiDefs.h>
#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTMLClasses.h>

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
		cout << "Please try reloading the web page." << endl; 
	}
       
	exit(EXIT_SUCCESS);
}

/* The error Function
	It takes one parameter for the error message to display.
	It displays the error message on the screen.
*/
void error(string err_message)
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
		cout << "Please try reloading the web page." << endl; 
	}
       
    exit(EXIT_SUCCESS);
}

/* The string_starts_with Function.
	It takes 2 parametres: main_str and to_match
	It uses the c++ string 'find method' to check for a match between the main_str and the to_match string.
	If the main_str begins with the exact words found in the to_match string, then it returns true.
	It returns false if it finds no match.
*/
bool string_starts_with(string main_str, string to_match)
{
	// If the two strings match, return true
	if(main_str.find(to_match) == 0)
		return true;
	else // No match found
		return false;
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
        	con = driver->connect(DB_HOST_URL, "admin", "vxbyt&^%rxcdquthy$@#990234&");
        	
        	// select appropriate database schema and connect to it. 
        	//(We are making use of the 'Users' schema created using create_database.sql file) 
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
			// the user 'admin' that is allowed to modify the Sessions table and
			// lastly, its password.
        	con = driver->connect(DB_HOST_URL, "admin", "vxbyt&^%rxcdquthy$@#990234&");
        	
        	/* select appropriate database schema and connect to it. (We are making use of the 'Users' schema created using create_database.sql file) */
        	con->setSchema("Users");      	
        	
        	// Delete the Session ID from the Sessions table of the database. 
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


// This function has a single string parameter.
// On the event that a user's session expires, it sets
// the user to offline. This is needed so that an illegitimate
// user can't login when offline.
void set_login_online_to_zero(string username)
{
	try 
	{
        	Driver *driver;
        	Connection *con;
        	PreparedStatement *prep_stmt;
        	
        	driver = get_driver_instance();
        	
        	// creates a database connection using the driver. The parameters to the driver is the :
			// DB_HOST_URL: MySQL server IP address and port, 
			// the user 'login' that is allowed to modify the login table, and
			// lastly, its password.
        	con = driver->connect(DB_HOST_URL, "login", "xcv6ertwq2#$!@098ujtyvbxmsk");
        	
        	/* select appropriate database schema and connect to it. (We are making use of the 'Users' schema created using create_database.sql file) */
        	con->setSchema("Users");      	
        	
        	// Make the user offline.
			prep_stmt = con->prepareStatement("UPDATE login SET online='0' WHERE username=?");
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

/* The make_online Function
	It takes a single parametre, username.
	Any user that successfully logs in, the function makes him/her online by updating the online
	field in the login database to 1 signifying online/active. A 0 value means the user is offline. 
*/
void make_online(string username)
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
            		if(!online)
            		{
            			//set admin to online
                		prep_stmt = con->prepareStatement("UPDATE login SET online='1' WHERE username=?");
                		prep_stmt->setString(1, username);
                		prep_stmt->execute();
                		
                		// set totp to online(i.e active)
                   		prep_stmt = con->prepareStatement("UPDATE TOTP SET online='1' WHERE fk_username=? AND online=?");
                   		prep_stmt->setString(1, username);
                   		prep_stmt->setBoolean(2, false);
                    		prep_stmt->execute();                    		
                	}
                	else
                	{
                		error_redirect("ERROR: You are online and active!.");
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
            		
            		if (!online) 
            		{
                    	// set user to online
                    	prep_stmt = con->prepareStatement("UPDATE login SET online='1' WHERE username=?");
                    	prep_stmt->setString(1, username);
						prep_stmt->execute();	
            		}
            		else
            		{
            			error_redirect("ERROR: You are online and active!.");
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

/* The redirect_to_admin_page Function
	This redirects the admin user to his page after successfull authentication.
*/
void redirect_to_admin_page(string username)
{
	// make admin online
	make_online(username);
	
	try
	{
		cout << HTTPHTMLHeader() << endl;
		cout << html() << head(title("Oxford Brookes Online Forum")).add(meta().set("http-equiv", "refresh").set("content", "0; URL=../admin/index.cgi?username="+username)) << endl;
        cout << head() << endl;
        cout << body();
        
       	cout << script() << endl;
        cout << "alert(\"Congratulations, you have been authenticated successfully!!\");" << endl;
        cout << script() << endl;

        cout << body() << endl;

       	cout << html();	
	}
	catch(exception &e)
	{
		cout << "Please try reloading the web page." << endl; 
	}
   
	exit(EXIT_SUCCESS);
}

/* The redirect_to_ordinary_users_page() Function
	This redirects the user to its page after successfull authentication.
*/
void redirect_to_ordinary_users_page(string username)
{
	// make the user
	make_online(username);
	
	try
	{
		cout << HTTPHTMLHeader() << endl;
		cout << html() << head(title("Oxford Brookes Online Forum")).add(meta().set("http-equiv", "refresh").set("content", "0; URL=../ordinary_users/index.cgi?username="+username)) << endl;
		cout << head() << endl;
		cout << body();
	
		cout << script() << endl;
		cout << "alert(\"Congratulations, You have been authenticated successfully!!\");" << endl;
		cout << script() << endl;

		cout << body() << endl;

		cout << html();	
	}
	catch(exception &e)
	{
		cout << "Please try reloading the web page." << endl; 
	}
   
	exit(EXIT_SUCCESS);
}

/* The set_sessionID_valid Function
    	This sets the SessionID to online/active within the 'Users' database.
    	It uses the database user, 'login'

    	The SessionID, is the parameter that should be set to active.

    	It returns true if operation is succesfull

*/
bool set_sessionID_valid(string SessionID) 
{
    try 
    {
        Driver* driver;
        Connection *con;
        PreparedStatement *prep_stmt;

        driver = get_driver_instance();
        
        // creates a database connection using the driver. The parameters to the driver is the :
		// DB_HOST_URL: MySQL server IP address and port, 
		// the user 'login' that is allowed to modify the Sessions table, and
		// lastly, its password.
        con = driver->connect(DB_HOST_URL, "login", "xcv6ertwq2#$!@098ujtyvbxmsk");
        con->setSchema("Users");

        prep_stmt = con->prepareStatement("UPDATE Sessions SET online='1' WHERE SessionID=?");
        prep_stmt->setString(1, SessionID);
        prep_stmt->execute();
        
        // close connection to database
        con->close(); 
        
        // clean up
        delete con;
        delete prep_stmt;
        return true;
    }
    catch (sql::SQLException& e) 
    {
        return false;
    }

}

/* The check_sessionID Function
   	This checks if the SessionID is valid and belongs to the username requested.
    It uses the database user, 'stateful'

    It returns true, if username and SessionID is valid.
*/
bool check_sessionID(string username, string SessionID) 
{
    try 
    {
        time_t     zero = time(0);
        struct tm  tstruct;
        char       time_now[80];
        tstruct = *localtime(&zero);
        strftime(time_now, sizeof(time_now), "%Y-%m-%d %X", &tstruct);

        Driver *driver;
        Connection *con;
        ResultSet *res;
		PreparedStatement *prep_stmt;
	
        driver = get_driver_instance();
        
        // creates a database connection using the driver. The parameters to the driver is the :
		// DB_HOST_URL: MySQL server IP address and port, 
		// the user 'stateful' that is allowed access to the Sessions database, and
		// lastly, its password.
        con = driver->connect(DB_HOST_URL, "stateful", "mxyct654@3$$%uity987#&hicdf");
        
        /* select appropriate database schema and connect to it. (We are making use of the 'Users' schema created using create_database.sql file) */
        con->setSchema("Users");

        prep_stmt = con->prepareStatement("SELECT fk_username, SessionID FROM Sessions WHERE fk_username=? AND expires > ?");
        prep_stmt->setString(1, username);
        prep_stmt->setString(2, time_now);
        
        res = prep_stmt->executeQuery();
        
        while (res->next()) // fetch data based on the query above
        {
            string dbname = res->getString("fk_username");
            string dbSession = res->getString("SessionID");
            if (dbname == username && dbSession == SessionID)
            {   
                //close the connection to the database
                con->close();
                
                // clean up
                delete res;
                delete con;
                delete prep_stmt;
               
                return true;
            }
        }
    }
    catch (sql::SQLException& e) 
    {
        return false;
    }
    return false;
}

/* The check_returned_2FA Function
    This checks if the OTP code entered is valid and belongs to the username
    It makes use of the user 'login'.
    
    It takes 2 params: username and returned_2fa( the OTP Code).

    It returns true, if OTP code and username match and they are valid.

*/
bool check_returned_2fa(string username, string returned_2fa) 
{
    try 
    {
        time_t     zero = time(0);
        struct tm  tstruct;
        char       time_now[80];
        tstruct = *localtime(&zero);
        strftime(time_now, sizeof(time_now), "%Y-%m-%d %X", &tstruct);

        Driver *driver;
        Connection *con;
        ResultSet *res;
        PreparedStatement *prep_stmt;

        driver = get_driver_instance();
        
        // creates a database connection using the driver. The parameters to the driver is the :
		// DB_HOST_URL: MySQL server IP address and port, 
		// the user 'login' that is allowed access to the 2FA database, and
		// lastly, its password.
        con = driver->connect(DB_HOST_URL, "login", "xcv6ertwq2#$!@098ujtyvbxmsk");
        
        /* select appropriate database schema and connect to it. (We are making use of the 'Users' schema created using create_database.sql file) */
        con->setSchema("Users");

        prep_stmt = con->prepareStatement("SELECT fk_username,code FROM 2FA WHERE fk_username=? AND expires > ?");
        prep_stmt->setString(1, username);
        prep_stmt->setString(2, time_now);
   
        res = prep_stmt->executeQuery();
        
        while (res->next()) // fetch data based on the query above
        {
            string dbname = res->getString("fk_username");
            string dbcode = res->getString("code");
            if (dbname == username && dbcode == returned_2fa) 
            {
                delete res;
                con->close();
                delete con;
                delete prep_stmt;
                return true;
            }
        }
    }
    catch (sql::SQLException& e) 
    {
        return false;
    }
    return false;
}


/* The check_returned_3fa Function
	checks if the TOTP code entered is valid and blongs to the admin
    It uses the database user, 'login'.

	It takes 2 params: username which should be admin and the returned_3fa( the TOTP code).   	

    It returns true, if TOTP code and username match and are valid.

*/
bool check_returned_3fa(string username, string returned_3fa) 
{
        try 
        {
        	Driver *driver;
        	Connection *con;
            ResultSet *res;
			PreparedStatement *prep_stmt;
		
            driver = get_driver_instance();
            	
           	// creates a database connection using the driver. The parameters to the driver is the :
			// DB_HOST_URL: MySQL server IP address and port, 
			// the user 'login' that is allowed access to the TOTP database, and
			// lastly, its password.
            con = driver->connect(DB_HOST_URL, "login", "xcv6ertwq2#$!@098ujtyvbxmsk");
            	
            /* select appropriate database schema and connect to it. (We are making use of the 'Users' schema created using create_database.sql file) */
            con->setSchema("Users");

            prep_stmt = con->prepareStatement("SELECT seed FROM TOTP WHERE fk_username=? AND online=?");
        	prep_stmt->setString(1, username);
        	prep_stmt->setBoolean(2, false);
            	
            res = prep_stmt->executeQuery();
            	
            while (res->next()) // fetch data based on the query above.
            {
               	string totp_seed = res->getString("seed");	// get the seed string.
                	
               	if (totp_seed == returned_3fa) 
               	{
                   		delete res;
                		con->close();
                    	delete con;
                    	delete prep_stmt;
                    		
                    	return true;
                }
            }
        }
        catch (sql::SQLException& e) 
        {
            return false;
        }

		return false;
}

/* The check_session_alive Function
    	This checks if the Session is still active or alive and redirects the user directly to his/her main user page.
    	If the session is dead, the user is redirected to the login page.
    	
    	It uses the database user: 'stateful'
	
		It takes 2 params: username and the session ID.
*/
bool check_session_alive(string username, string SessionID) 
{
    string database_username = "";
    bool database_session_status = false;

    string ENV = "localhost";	//giving it a default value of localhost, since this is not a production environment
    
    // for production environment, uncomment this 2 lines and comment the above ENV
    //try { ENV = getenv("REMOTE_ADDR"); }
    //catch (exception& e) { exit(EXIT_SUCCESS); }
    
    try 
    {
        time_t     zero = time(0);
        struct tm  tstruct;
        char       time_now[80];
        tstruct = *localtime(&zero);
        strftime(time_now, sizeof(time_now), "%Y-%m-%d %X", &tstruct);

        Driver *driver;
        Connection *con;
        ResultSet *res;
        PreparedStatement *prep_stmt;

        driver = get_driver_instance();
        
        // creates a database connection using the driver. The parameters to the driver is the :
		// DB_HOST_URL: MySQL server IP address and port, 
		// the user 'stateful' that is allowed access to the Sessions database, and
		// lastly, its password.
        con = driver->connect(DB_HOST_URL, "stateful", "mxyct654@3$$%uity987#&hicdf");
        
        /* select appropriate database schema and connect to it. (We are making use of the 'Users' schema created using create_database.sql file) */
        con->setSchema("Users");

        prep_stmt = con->prepareStatement("SELECT fk_username,SessionID,online FROM Sessions WHERE IP=? AND SessionID=? AND expires > ?");
        prep_stmt->setString(1, ENV);
        prep_stmt->setString(2, SessionID);
        prep_stmt->setString(3, time_now);
        
        res = prep_stmt->executeQuery();
        
        while (res->next()) 	// fetch data based on the above query.
        {
            string database_session = res->getString("SessionID");
            if (database_session == SessionID) 
            {
                database_username = res->getString("fk_username");
                database_session_status = res->getBoolean("online");
                delete res;
                con->close();
                delete con;
                delete prep_stmt;
                
                break;
            }
        }
    }
    catch (sql::SQLException& e) 
    {
		clear_user_2fa_email_totp(username); // Clear user 2FA, Email, and TOTP table
		clear_user_sessionID(username);	// Clear user session ID
		set_login_online_to_zero(username);	// Make the user offline
		
        error_redirect("ERROR: Session Expired. Please Login Again!!.");
        
        exit(EXIT_SUCCESS);
    }
        
	//if(SessionID exists)
	if (database_username == username) //if there is a match between username and SessionID 
	{
		if (database_session_status) //if SessionID is active
		{
			return true;
		}
	}
	else
	{
		clear_user_2fa_email_totp(username); // Clear user 2FA, Email, and TOTP table
		clear_user_sessionID(username);	// Clear user session ID
		set_login_online_to_zero(username);	// Make the user offline
		
		error_redirect("ERROR: Session Expired. Please Login Again!!.");
	}
	
	return false;
}

/* The main Function
	It receives the OTP(or 2FA Code), and TOTP through POST request. It also retrieves the cookie values
	for 'session ID' and 'username' to check for session alive.
	
	If any user is successully authenticated, he/she is redirected to their profile page.
*/
int main()
{
	string returned_2fa = "";
	string returned_3fa = "";
	string sessionID = "";
	string username = "";
	
	Cgicc form_data;
	const_cookie_iterator cci;
	
	cout << "Content-type:text/html\r\n\r\n";
   	cout << "<html>\n";
  	cout << "<head>\n";
   	cout << "<title>Cookies in CGI</title>\n";
   	cout << "</head>\n";
   	cout << "<body>\n";
   	
	const CgiEnvironment& env = form_data.getEnvironment();

	for (cci = env.getCookieList().begin(); cci != env.getCookieList().end(); ++cci) 
	{
		if (cci->getName() == "SessionID")
		{
			sessionID = cci->getValue();
		}
		if (cci->getName() == "username") 
		{
			username = cci->getValue();
		}
	}

	// If the username and session ID is not empty, check if session is still alive.
	if(!username.empty() && !sessionID.empty())
	{
		if(check_session_alive(username, sessionID))
		{
			if(username == "admin")
			{
				redirect_to_admin_page(username);
				exit(EXIT_SUCCESS);
			}
			else
			{
				redirect_to_ordinary_users_page(username);
				exit(EXIT_SUCCESS);
			}
		}
		
		// Continue from here since session is not alive. Obtain the 2FA and TOTP(3FA) and validate the user. 
		form_iterator fi = form_data.getElement("2FA");
		
		if (!fi->isEmpty() && fi != (*form_data).end()) 
		{
			returned_2fa = **fi;
		}
		
		fi = form_data.getElement("3FA");
		
		if (!fi->isEmpty() && fi != (*form_data).end()) 
		{
			returned_3fa = **fi;
		}
		
		if (username == "admin" && (returned_2fa.empty() || returned_3fa.empty())) 
		{
			error("Admin authentication error");
			exit(EXIT_SUCCESS);
		}
		else
		{ 
			if (returned_2fa.empty() && username != "admin") 
			{
				error("User authentication error");
				exit(EXIT_SUCCESS);
			}
		}
	}
		
	else
	{
		error_redirect("ERROR: No login details provided!");
		exit(EXIT_SUCCESS);
	}
		
	if (!username.empty()) 
	{
			// if the TOTP and OTP returned is not empty, and username = admin, execute the block
        	if ((!returned_3fa.empty()) && (!returned_2fa.empty()) && (username == "admin"))
        	{
					// If admin's TOTP is valid, set the session ID up and redirect the admin to his profile page.
                	if (check_returned_3fa(username, returned_3fa) && check_sessionID(username,sessionID) && check_returned_2fa(username,returned_2fa)) 
                	{
                    		set_sessionID_valid(sessionID);
                    		redirect_to_admin_page(username);]
							exit(EXIT_SUCCESS);
                	}
                	else 
                	{
							error("Authentication error");
                    		exit(EXIT_SUCCESS);
                	}
            }
            
            else if ((returned_3fa.empty() && !returned_2fa.empty() && username != "admin")) 
            {
				// If session ID is valid, set it up to active and redirect the ordinary user to his profile page
            	if (check_sessionID(username, sessionID) && check_returned_2fa(username, returned_2fa)) 
               	{
					set_sessionID_valid(sessionID);
					redirect_to_ordinary_users_page(username);
					exit(EXIT_SUCCESS);
               	}
               	else 
               	{
              		error("Authentication error");
                   	exit(EXIT_SUCCESS);
               	}
            }
	}
    else 
        error_redirect("ERROR: Your username field is empty!"); 
        	
    cout << "</body>\n";
    cout << "</html>\n";
        	
    return 0;
}
