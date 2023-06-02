/* Copyright(c) 2023 Igbe Chijioke
*  All Rights Reserved
*/

#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <cgicc/CgiDefs.h>
#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTMLClasses.h>

using namespace cgicc;
using namespace std;

/* The create_post_page() Function.
   It displays an input field using HTML, and CSS. Here the user can type in his/her post.
*/
void create_post_page(string username, string thread_name) 
{
    try 
    {
        cout << HTTPHTMLHeader() << endl;
        cout << html() << head(title("Oxford Brookes Online Forum")).add(link().set("rel", "stylesheet").set("href", "../../static_files/css/main.css")) << endl;
        cout << body().set("class", "create-post-index-html-body");
        
        cout << h1("Oxford Brookes Online Forum").set("style", "color:white") << endl;
        
        cout << cgicc::div().set("class", "create-post-index-html-container") << endl;
        cout << form().set("method", "post").set("action", "create_post.cgi?username="+username+"&thread_name="+thread_name) << endl;
        cout << label("Post: ").set("for", "thread-post").set("style","font-weight:bold;font-size:1.5rem") << endl;

        cout << input().set("type", "text").set("name", "thread-post").set("placeholder", "..Type your contribution (Note: not more than 250 characters)").set("style", "height:100px").set("autocomplete", "off") << endl;
        cout << "<br>" << endl;
            	
        cout << input().set("type", "submit").set("value", "create").set("class", "submit") << endl;
        cout << form() << endl;

		cout << cgicc::div().set("class", "create-post-footer") << endl;
		cout << h3("Igbe Chijioke: Oxford Brookes Online Forum (c) 2023").set("class", "h3-create-post") << endl;
		cout << a().set("href", "#").set("class", "create-post-rights-reserved-link") << "All Rights Reserved" << a() << endl;
		cout << cgicc::div() << endl;
		
        cout << body() << endl;
        cout << html();
    }
    catch (exception& e) 
    {
        cout << "This should not have happened. Please try reloading the page again." << endl; 
    }
}


/*  The Main Function
    It receives username and thread_name.
    
    It then calls the create_post_page() function with the required arguments.
*/
int main()
{
    Cgicc form_data;
	string username = "";
	string thread_name = "";
	
	form_iterator fi = form_data.getElement("username");  
   	if( !fi->isEmpty() && fi != (*form_data).end()) 
   	{  
      		username = **fi; 
      		
      		fi = form_data.getElement("thread_name");
        	if (!fi->isEmpty() && fi != (*form_data).end()) 
           		thread_name = **fi;
      		
			create_post_page(username, thread_name);
    }
	
	return 0;
}
