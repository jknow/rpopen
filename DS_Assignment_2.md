#Assignment 2#
Due Monday, October 6, 2014 4:30 pm via sakai

<h1> Group size </h1>
<p>
You may work on this assignment either individually or in a group of up to four members
</p>

<h1> Objective </h1>
<p>
Your goal in this assignment is to use sockets at the operating system level
to allow a program to open the output stream of a remote program. You will 
write a function called <em>rpopen</em>, which is similar to the
<a href="http://pubs.opengroup.org/onlinepubs/009695399/functions/popen.html">popen</a>
standard I/O library function 
that is found on Linux, BSD, OS X, and most other POSIX systems.

You will write a server and the client function and test it with a client program.
clients and a server.
</p>

<h1>Languages</h1>
<p>
This assignment must be done in C. You will use socket-related system calls as well as
<em>fork</em> and <em>execl</em> system calls to create processes and <em>dup2</em>
to duplicate file descriptors.
</p>

<h1> Background </h1>
<h2> The popen function </h2>
<p>
System calls that operate on files, such as 
<em>open</em>, 
<em>read</em>, 
<em>write</em>, 
<em>close</em>, 
and others
use file descriptors, which are small integers that refer to an
open file in the process' file table. Each process is started with file descriptor 0 representing the standard input, 
file descriptor 1 representing the standard output, and file descriptor 2 representing the standard
error output file. Unless input or output is redirected, these files usually correspond to the
terminal driver, with input coming from your keyboard and the output going to the terminal window in which
your application is running.
</p>
<p>
The C standard I/O library (stdio), which is a user-level library, adds a layer of I/O buffering in the process.
Hence, instead of accessing the operating system's file descriptors directly, it uses FILE pointers.
The FILE type is defined in <code>stdio.h</code> and represents an I/O file stream. Stdio functions include
<em>fopen</em>, 
<em>fread</em>, 
<em>fwrite</em>, 
<em>fgets</em>, 
<em>fputs</em>, 
<em>fprintf</em>, 
<em>fscanf</em>, and
<em>fclose</em>, among others.
The global variables <code>stdin</code>, <code>stdout</code>, and <code>stderr</code> are defined by
the stdio library to access the standard input, output, and error streams, respectively.
</p>

<p>
The <em>popen</em> function is part of the C standard I/O library and allows you to run a program
and get a file stream to either read its output or send it input. Like other other stdio functions, 
it uses a FILE pointer. It is given a command as its first parameter and returns a FILE pointer to either 
the input or output stream of that command. The second parameter is a string that specifies whether you
want to read from that command or write to it.
</p>

<p>
Here is an example of using popen to read lines of output from the command "<code>ls -lL /etc</code>", which
lists the contents of the <code>/etc</code> directory:
</p>

<div class="codeblock-box">
#include &lt;stdio.h&gt;
#define BSIZE 1024	/* buffer size */

main(int argc, char **argv) {
	FILE *fp;
	char buf[BSIZE];

	/* read from the command ls -lL /etc */

	if ((fp = popen("ls -lL /etc", "r")) != 0) {
		/* read the output from the command */

		while (fgets(buf, BSIZE, fp) != 0)
			fputs(buf, stdout);
	}
	pclose(fp);
}
</div>
<p>
&rArr; <a href="popen-demo.html">Download a popen demo program</a>
</p>
<p>
The <em>pclose</em> function closes the FILE stream and returns the exit status of the command.
</p>

<h2> Programming with Sockets </h2>

<p>
Be sure that you are understand the basics of TCP sockets programming.
To simplify matters, please go through the
<a href="../notes/sockets/index.html">TCP/IP Sockets Programming Tutorial</a>.
This includes a few simple programs and you can use much of that code 
to get your program up and running.
</p>
<p>
Be sure to compile and run the examples before starting your assignment.
This way, you can ensure that your build environment and networking environment
are working properly.
</p>

<h1> Specifications </h1>
<p>
<strong>Note:</strong> be sure to read through the
<a href="../notes/content/02r-assignment-2.pdf">recitation notes for assignment 2</a> 
(<a href="../notes/content/02r-assignment-2-6pp.pdf">6 per page</a>)
for additional details and step-by-step implementation advice.
</p>

<p>
For this assignment, you will write one <em>server</em> and one
<em>rpopen</em> client library.
</p>

<h2> The client </h2>

<p>
The client is a function that implements a "remote popen," which allows a program
to read the output of a specified remote program.
The syntax of the function is:
</p>
<pre><code>
FILE *rpopen(char *server, int port, char *cmd);
</code></pre>
<p>
Where
</p>
<dl>
<dt>server</dt>
<dd>
<p>
Is the name of the remote server. 
The value may be 0 or an empty string. In that case, 
the value of the environment variable <code>PHOST</code> will be used for the host.
You can set a shell environment variable in the bash shell with the command
</p>
<pre><code>
export PHOST=localhost
</code></pre>
<p>
This example sets <code>PHOST</code> to the string <code>localhost</code>, which
refers to the local host. 
</p>
</dd>

<dt>port</dt>
<dd>
Is the port number of the remote server. You should set this to whatever port your
server is using to accept connections. 
The value may be 0. In that case, 
the value of the environment variable <code>PPORT</code> will be used for the port number.
The port number should be in the range 1024...65536, although the use of ports above 49151 is
discouraged since that range is reserved for <em>ephemeral ports</em>, which are
port numbers that the operating system would allocate.
</dd>

<dt>cmd</dt>
<dd>
Is the remote command to execute. This can be an arbitrary command with arguments and
will be sent to and executed on the server.
</dd>
</dl>

<p>
An example of a program that uses your <em>rpopen</em> function is the following:
</p>
<div class="codeblock-box">
#include &lt;stdio.h&gt;
#define BSIZE 1024

FILE *rpopen(char *host, int port, char *cmd);

main(int argc, char **argv)
{
	FILE *fp;
	char buf[BSIZE];

	if ((fp = rpopen("localhost", 0, "ls -lL /etc")) != 0) {
		/* read the output from the command */

		while (fgets(buf, BSIZE, fp) != 0)
			fputs(buf, stdout);
	}
	fclose(fp);
}
</div>
&rArr; <a href="files/rpopen-client.c">Download the rpopen sample </a>

<p>
This program connects to a server that is running on the same machine on the client
and listening on a port number that is defined in the environment variable
<code>PPORT</code>. The server executes the command:
</p>
<pre><code>
ls -lL /etc
</code></pre>
and the client reads its results one line at a time, printing each line that it reads.
</p>

<h2> Client Protocol and Operation </h2>
<p>
The client employs a simple protocol for communicating with the server.
It performs the following steps:
</p>

<ol class="separated">
<li>
Get the host name and port number from the parameters or environment variables.
</li>

<li>
Connect to the server. This involves looking up the server's address, creating
a socket, and using the socket to connect to the server.
You should be able to use most of the code in the TCP client demo (demo-03) to
do this. You can use the function that connects to the service without modification.
</li>

<li> 
Send the command over the socket to the server. You can use the <em>write</em> system call to do this.
</li>

<li>
Convert the file descriptor for the socket to a <code>FILE</code> pointer opened for reading with the
<a href="http://man7.org/linux/man-pages/man3/fdopen.3.html">fdopen</a>
function.
</li>

<li>
Return the <code>FILE</code> pointer to the caller.
</li>
</ol>



<h2> The server </h2>
<p>
The server executable will be named <strong>rpserver</strong> (remote pipe server).
It accepts a port number on the command line following a <strong>-p</strong> flag. For example:
</p>
<div class=codeblock>
rpserver -p 12345
</div>
<p>
will start the server listening to port 12345.
If this parameter is missing, you should fall back to some hard-coded default
port number in the range 1024...49151. 
</p>
<p>
Use <em>getopt</em to parse the command-line arguments. You should 
be able to use most of the code in the TCP server demo (demo-03) directly to
do this.

If you have not used <em>getopt</em> before, see the man page to
understand what it does and how it works; it has an example.
You can also see my tutorial with sample code
<a href="../../416/notes/c-tutorials/getopt.html">here</a>.
Any other options or arguments should produce an error message.
</p>

<p>
The server will perform the following sequence of operations:
</p>
<ol class="separated">
<li>
Open a socket and set it up for listening - follow the
example in the TCP sockets demo code. Don't forget to setsockopt to
SO_REUSEADDR as in the sample code in the tutorial
if you are using C/C++). 
Your program does not have to be multithreaded. 
As explained later, each connection will result in the
spawning of a new process.
</li>

<li>
Loop forever, waiting for incoming connections with the <em>accept</em> system
call. When <em>accept</em> returns, it gives you a new socket that is used
for communicating with the client that made the connection.
</li>

<li>
<p>
Fork a child process.
The parent closes the new socket (it does not need to talk to the client)
and returns back to waiting for a new connection.
</p>
<p>
The child reads data from the client. The <em>only</em> data it will get
from the client is the command that the client wants it to execute.
After reading this data, the client can close the socket for reading with
the <a href="http://man7.org/linux/man-pages/man2/shutdown.2.html">shutdown</a> system call:
</p>
<pre><code>
shutdown(sock, SHUT_RD);
</code></pre>
<p>
This will ensure that the server will be unable to wait and try to read any
data from the client.
</p>
</li>

<li>
Now the child process needs to execute the requested command. Prior to
doing this, however, we want to make sure that the standard input is closed
and that the standard output goes to the socket. These settings will remain
in effect when the requested program is executed.
<p>
To close the current standard input and output, just close file descriptors 0 and 1:
</p>
<pre><code>
close(0);
close(1);
</code></pre>
<p>
Then duplicate the socket onto file descriptor 1 (standard output) using the
<a href="http://man7.org/linux/man-pages/man2/dup2.2.html">dup2</a> system call.
</li>

<li>
<p>
The child process can overwrite itself by executing the requested command.
The easiest way to do this and avoid the hassle of parsing arguments is to 
simply execute the shell and give it that command as a parameter:
</p>
<pre><code>
/bin/sh -c <em>requested_command_with_arguments</em>
</code></pre>
<p>
The <a href="http://man7.org/linux/man-pages/man3/execlp.3.html">execlp</a>
function makes this simple. It's a thin wrapper over the 
<a href="http://man7.org/linux/man-pages/man2/execve.2.html">execve</a>
system call that takes an arbitrary set of parameters that become the 
command and its arguments. I this case, your command is the shell, <code>/bin/sh</code>
and the parameters are: (1) the shell's name, (2) the <code>-c</code> flag, and 
(3) the command that you read from the client. The list is terminated with
a <code>(char *)0</code> sentinel. The call will be something like:
</p>
<pre><code>
execlp("/bin/sh", "/bin/sh", "-c", command, (char *)0);
</code></pre>
<p>
If the program executes, it will overwrite the code in the child process and
no further statements in your program will be executed. If an error occurs, then
<em>execlp</em> will return. You may print a message on the standard error and,
since no feedback is sent to the client, just exit the client.
</li>


</ol>


<h1> Template files and Makefile </h1>

<p>
Download <a href="rpopen.zip">rpopen.zip</a>. This is a zip file containing the following:
</p>
<dl> 

<dt> Makefile
</dt>
<dd>
<p>
This is a simple makefile that assumes that all client-side logic to implement
<em>rpopen</em> is in a file called <code>rpopen.c</code> and that the entire
server is in a file called <code>rpserver.c</code>. It also builds a demo
client called <em>rpopen-demo</em> from a file called <code>rpopen-demo.c</code>
</p>
<p>
You can run the command <code>make</code> to compile all these files. It works
with the templates but, of course, does not build anything useful.
<p>
If you need to add additional files to compile your code, modify this makefile as
necessary.
You must submit a <strong>Makefile</strong> so that I can run <strong>make</strong>
to compile your program (see <a href="../notes/make/index.html">some basic notes on makefiles</a>).
Do NOT modify the name of the server. I expect to execute it with the name <em>rpserver</em>.
</dd>

<dt> rpopen.c
</dt>
<dd>
This is a template for the client-side <em>rpopen</em> function. 
You will, no doubt, add other functions to this file.
</dd>

<dt> rpserver.c
</dt>
<dd>
This is a template for the server program. 
You will, no doubt, add a bunch of other functions to this file.
</dd>

<dt> rpopen-demo.c
</dt>
<dd>
<p>
This is a demo program that uses <em>rpopen</em>. 
You should be able to use it with your client implementation with no modification.
Prior to running it, be sure to set your <code>PHOST</code> and <code>PPORT</code>
environment variables:
</p>
<pre><code>
export PHOST=name_of_server
export PPORT=port_of_server
</code></pre>
</dd>
<p>
The Makefile will compile an executable version of this.
</p>

</dl>

<h1> Clean your code!  </h1>
<p>
Code reuse is great and I expect you to use code from the TCP demo program: client and
server socket setup and server option processing. However, don't be sloppy!
Be sure to structure your code cleanly and get rid of ALL extraneous code. There should 
not be a single variable or line of code that does not make sense for this assignment.
</p>
<p>
Make sure you have ample comments in your code so that someone scanning it can
quickly understand what every part of your code does.
</p>
<p>
Be sure to use good functional decomposition. I don't want to see all your
server code sitting within a single main() function, for example, or all your
client code inside of rpopen().
</p>


<h1> Submission </h1>

<p>
Be sure to write the names of all authors in comments at the start of each program.
This makes it easy for me to identify you.
</p>
<p>
Submit everything I need to compile your program. 
This will, at minimum, be two files that implement your client library and server program,
makefile, and test program.
</p>


<h2> Documentation </h2>
<p>
In addition to your code, you should also submit documentation in a plain text, HTML, or pdf format.
The documentation should be well-formatted and clearly explain:
</p>
<ul>
<li> The files you’re submitting and what they do </li>
<li> How to compile and run (test this with a non-technical person!) </li>
<li> What tests you ran to validate the software (e.g., modifications on <code>rpopen-demo.c</code> </li>
</ul>

<p>
Submit only what is required. Do not submit object files, executables, or temporary editor files. 
</p>

<p>
To submit the assignment, you will create a tar archive or zip file of
all the files that are needed to compile the 
program, including the makefile:
<div class="codeblock">
	tar cvf a2.tar *.c Makefile readme.pdf
</div>
<p>
This assumes that the directories have been purged of all the
files that you do not want to submit (class files, temporary
files, etc.).
</p>
<p>
Before sending the file to me, make sure that all the components
are there. If I can't
compile any part, you will get <strong>no</strong> credit.
</p>

<p>
Submit the assignment via <a href="https://sakai.rutgers.edu/portal">sakai</a> as Assignment 2.
</p>
