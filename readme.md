# How to compile 
for client
gcc Client.c -o client
for server 
gcc ServerV1.c -o server

running the application

open the server 
./server 8000 
where 8000 is port number it can be any allowed by your OS
Now you can connect multiple client by
./client localhost 8000

Now use one of these credentials to interact 

UserName A
Pass 1234 

or

UserName B
Pass 2341

Under Enter OP: following operations are allowed 

1.) T UserName Amount
Entering this would mean transfer Amount to UserName 
E.g T B 1000

2.) Status
Gives total amount in your bank account initialised by 5000



# Understanding Implementation

Implemented bank server with multi threading hence allowing multiple client to connect to bank server at same time and allowing following task to execute
1 Transfer – T B 500
Which means transfer 500 to B’s account.
2 Status – Money in account
Now transfer is critcal section where no two users should simuntaneoulsy be able to 
add or subtract money to same account at the same time, that is we need to avoid race 
condition so to avoid race condition in money tansfer code which is critical section of 
code is locked for one user at a time using mutex lock which make that section of 
code to be used by single client or process at a time.
Demonstration
For now 2 users are maintained in bank database with following credentials
User name : A
Pass : 1234
Initial Amount : 5000
User name : B
Pass : 2341
Initial Amount : 5000
As shown above server will show users connected to it.
Two Users A and B are simultaneoulsy connected
And various operations(op) are performed
Various checks in application in also made like self transfer is not possible transfer 
more than amount present in bank account is not possible, if any of the operations are 
tried client will get Can’t pay message.
Note B status is checked when A has transferred 1000 that why its show 6000 in 
Status operation.
Any other command will disconnect you from the server.
To succesfully exit bank server type break in Enter Op: and then use SIGINT

