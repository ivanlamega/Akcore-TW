AKCore
======

The official DBO server that was made by Atidot3, ~~Daneos~~, and Kalisto
This is the DBO TW Branch for the most recent version of the game.


## Prerequisites

Very few things are needed for compiing the emulator.

1. [Boost 1.55.0](http://sourceforge.net/projects/boost/files/boost/1.55.0/boost_1_55_0.zip/download)
2. [x86 MySQL C Connector] (http://dev.mysql.com/get/Downloads/Connector-C/mysql-connector-c-6.1.5-win32.msi)
3. [x86 MySQL C++ Connector] (http://dev.mysql.com/get/Downloads/Connector-C++/mysql-connector-c++-1.1.3-win32.msi)
4. A MySQL Database 
5. Microsoft Visual Studio 2013


## Installation

### Compiling

Copy your everything from INSIDE your boost download boost download to Server/NtlDatabase/mysql/boost
When it looks like [this](http://puu.sh/a0q9o/946f96747f.png) it is correct.

Next in the NtlDatabase folder open mysql_wrapper.h and change the database credentials to your database credentials
(currently the stored procedures require the user to be root@% you can change them if you like)
 
Now open AKCore.sln with visual studio 2010.
Select the Debug/win32 configurations and compile.

Congrats you are done with this section
 
### SQL

Create a new database called dbo in your MySQL instance.

import the sql files from the SQL folder.

Create your first account by manually entering it in your database.

## Configuration

Now that you have everything compiled you can setup your server.

Go into the debug folder in the root directory (AKCore) 
there are 4 files you can edit.

1. AuthServer.ini
2. CharServer.ini
3. ChatServer.ini
4. GameServer.ini

All you have to do in here is set your ports. If you don't want to that is fine as well.
This is an optional step.


## Addendum

This software is licensed under the GPL for a reason.
Please share your work.
You see a bug you want to fix submit a pull request.
Think you can write an awesome feature that should be there.... Submit a pull request
We will go over the code and if it complies with the coding standard we have set forth
we will put it in our next commit. This Git repo was made with you all in mind. Please
contribute to it, bug fixes, patches all of it,  without all of us, we won't be able to 
play this awesome game again.

