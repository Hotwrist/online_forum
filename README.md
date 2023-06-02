# online_forum
This repository houses the source codes of a CGI website written in C/C++.

# Description of the functional requirements
The function of the software is an on-line forum management system. Users will use it to view threads which consist 
of an ordered list of posts. Each post is a string of html text. One user will be the administrator and they can also delete posts.


There are two kinds of user: ordinary users and the administrator. Both must be able to register an account and set a password.
Both types of user can see a list of the threads available, view all the posts in a thread of their choice, add a post of their own 
at the end of a thread and delete any of their posts.
The administrator can also create a new thread with their contribution as the first post, delete posts of others, and delete whole 
threads. There is only one administrator account.

# Description of the security requirements
*(1) Both kinds of user must be able to log in with the password they have chosen.*

*(2) Both kinds of user must be able to log out and will be automatically logged out after a fixed period of time of inactivity.*

*(3) The software must maintain a session and it should do so using cookies.*

*(4) If a user attempts to access a page without being logged in, they should be redirected to the log in page.*

*(5) Passwords must be encrypted when stored in the database; you may use cryptographic libraries for this if you wish.*

*(6) All user inputs should be sanitised before they are used.*

*(7) The process of logging in should use two-factor authentication. The user must enter a second password sent by email after the main 
password has been entered. The email address to be used is the one entered when registering the account. If you are not able to install 
the relevant mail library, you can simulate the process of emailing by appending to a “mail spool” text file representing all the emails that have been sent.*

*(8)The administrator account, in addition to the protections of SR7, must be authenticated by a challenge-response software that simulates the concept of a 
hardware token. You are encouraged to do the necessary research into the concept of hardware token to understand how they work and then think carefully about 
how to solve the problem of replicating that behaviour in software.*


# Images
Testing Site for HTTPS
![testing_https](https://github.com/Hotwrist/online_forum/assets/38595935/03b2fea6-dd02-48ac-87dd-60a112cb0762)


Registration Page
![register](https://github.com/Hotwrist/online_forum/assets/38595935/1dcec856-7899-4be2-b113-7f814fed98cc)

List of Thread(admin can view it)
![list_thread](https://github.com/Hotwrist/online_forum/assets/38595935/8ad4e939-981e-4628-a68a-1f98ab1873c7)

List of Thread(ordianry users can view it)
![list_thread2](https://github.com/Hotwrist/online_forum/assets/38595935/b4bd217a-8662-4ec4-a397-156a8c5005a9)

List of Posts in a specific thread
![posts](https://github.com/Hotwrist/online_forum/assets/38595935/b651d793-b13d-47e1-bc5c-84918b0ff7c6)


# Security Requirements

2FA Page
![2fa](https://github.com/Hotwrist/online_forum/assets/38595935/474da39e-b4a0-451b-b992-123c22da12fa)


Admin 3FA
![3fa](https://github.com/Hotwrist/online_forum/assets/38595935/a9fb934c-6836-4f4d-a48c-1a8907f7be24)


Admin Profile Page
![admin_pp](https://github.com/Hotwrist/online_forum/assets/38595935/119029b7-de6d-4649-8da4-dceee8818a8a)


Ordinary Users Profile Page
![users_pp](https://github.com/Hotwrist/online_forum/assets/38595935/eac0df4b-df8e-4ff8-8aca-aa4627867d7a)

Logout Page
![logout](https://github.com/Hotwrist/online_forum/assets/38595935/be2a90a2-563f-4bb4-ac21-9ba9c8ad2f59)


Handling Unauthorized Access (i.e An offline user trying to access a page)
![u_access](https://github.com/Hotwrist/online_forum/assets/38595935/278e2735-4fcb-4dda-a30d-11b8e4962828)


Sessions ID, Encrypted Passwords
![e_pass](https://github.com/Hotwrist/online_forum/assets/38595935/5ed67890-2ab3-4e68-bb09-f64d74ed44f5)
![e_session](https://github.com/Hotwrist/online_forum/assets/38595935/ebd6134f-bb36-4f74-b268-1457d6177e55)





