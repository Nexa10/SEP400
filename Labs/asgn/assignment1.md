
<h2>SEP400 - Assignment 1: Embedded Debug Logging</h2>

<p>The recording for assignment 1 can be found at <a href="https://seneca-my.sharepoint.com/:v:/g/personal/miguel_watler_senecacollege_ca/EVnPIIbye39FhRXw6E_wA44BcYG48QVudhMHs3J8nwZG3g?e=AXtgwC" target="_blank">Assignment1.mp4</a>.</p>

<p><b>NOTE:</b> This assignment is to be done in groups of two.  See <a href="Assignment1Groups.docx" target="_blank">Assignment1Groups.docx</a></p>

<h3>Due: Friday, March 24, 2023</h3>

<h4><b><u>Business Case</u></b> (Marketing, Project Manager, Senior Executives)</h4>
<p>A product goes through the phases of business case, requirements, design, implementation, testing and rework, and eventually
    deployment to customers. Testing and rework attempts to simulate conditions at a customer site, but does not always catch
    every defect in the product. There are cases where the product malfunctions or stops working at a customer site, with very
    few clues as to what went wrong.</p>

<p>Linux has some built in logging mechanisms such as <b>syslog</b>, <b>rsyslog</b> and <b>syslog-ng</b>. These logs can be found in the <b>/var/log/</b>
    directory and can be useful. More useful is if the code for the product itself had logging embedded into it. Embedding
    debug logs in the code assists in debugging such problems in the field which are difficult to recreate in the lab. This
    will greatly assist in working through problems with a customer without losing customer confidence and thereby harming
    the reputation of the company.</p>

<p>Our task then is to develop an embedded logging mechanism in our code with logs being sent to a central server which can keep track of how the code is performing.</p>

<h4><b><u>Requirements</u></b> (Marketing, Project Manager, Project Lead)</h4>
<ul>
<li>Logging must be terse but contain useful information.</li>
<li>Each embedded log entry must contain a log severity. It must also contain the filename, the function name, the line number that the log appears in. Finally, it must contain a brief message.</li>
<li>Logging must be fast. It cannot slow down the performance of the product.</li>
<li>Logging must the filtered by the filter level severity. This means any logs at the filter level severity or higher will be reported. Any logs of lower severity than the filter level severity will be ignored.</li>
<li>The severity levels will be debug, warning, error, and critical.</li>
<li>The code itself will initialize logging and set the filter severity level.</li>
<li>Logs must be sent to a central server on another machine for future reference.</li>
<li>The server will collect all log messages on one common log file called the server log file.</li>
<li>The server gives the option to dump the server log file to the screen.</li>
<li>The server gives the option to overwrite the filter level severity.</li>
</ul>

<h4><b><u>Design</u></b> (Project Lead, Senior Engineers)</h4>
<b>Each Process:</b>
<ul>
<li>Each process will initialize logging for its process, set the filter level severity, call log functions to report any unusual activity, and shutdown the logger before exiting.</li>
</ul>

<b>The Logger:</b>
<ul>
<li>The logging code will be contained within one file which can be included in the <b>Makefile</b> for each process.</li>
<li>The logging code will communicate with the server via an IP address and port on the server.</li>
<li>The logging code will collect logs from the process.
<ul>
<li>If the logs are below the filter level severity, they are thrown away.</li>
<li>If the logs are at or higher than the filter level severity, they are written to a server via UDP for fast delivery.</li>
</ul></li>
<li>The logging code will have a separate thread for reading UDP messages from the server.</li>
</ul>
<b>The Server:</b>
<ul>
<li>The server must bind a socket to its IP address and to an available port.</li>
<li>The server must have the ability to write messages to the logger via UDP for fast delivery.</li>
<li>The server will have a separate thread for reading UDP messages from the logger.</li>
<li>The server must give the user the option to dump the server log file to the screen.</li>
<li>The server must give the user the option to overwrite the filter level severity of the logs.</li>
</ul>

<h4><b><u>Implementation</u></b> (Junior and Senior Engineers)</h4>
<b>Each Process:</b>
<ul>
<li>The process code will include a <b>Logger.h</b> file.</li>
<li>The process <b>Makefile</b> will include <b>Logger.cpp</b> to its list of files.</li>
<li>The process will initialize logging with a call to <b>InitializeLog()</b> of the logger.</li>
<li>The process will set the filter severity level with a call to <b>SetLogLevel()</b>.</li>
<li>The process will send log messages to the logger by calling <b>Log()</b> of the logger with the following information:
<ul>
<li>Log Severity - DEBUG, WARNING, ERROR, CRITICAL.</li>
<li>File name - <b>__FILE__</b>. This is a Linux macro which contains the filename. It is of type <b>char *</b>.</li>
<li>Function name - <b>__func__</b>. This is a Linux macro which contains the function name. It is of type <b>char *</b>.</li>
<li>Line number - <b>__LINE__</b>. This is a Linux macro which contains the line number of the log. It is of type <b>int</b>.</li>
<li>Message - The log message. This can be any user defined message contained as a string.<br>
Example:<br>
<b>Log(DEBUG, __FILE__, __func__, __LINE__, "Created the objects");</b></li>
</ul></li>
<li>When the process is ready to exit, it will call <b>ExitLog()</b> to shut down the logger.</li>
</ul>

<b>The Logger:</b>
<ul>
<li>The logger will type-define an enumeration called <b>LOG_LEVEL</b> in its header file (<b>Logger.h</b>) specifying the log levels: DEBUG, WARNING, ERROR, CRITICAL.
<li>The logger will expose the following functions to each process via its header file (<b>Logger.h</b>):
<ul><b>
<li>int  InitializeLog();</li>
<li>void SetLogLevel(LOG_LEVEL level);</li>
<li>void Log(LOG_LEVEL level, const char *prog, const char *func, int line, const char *message);</li>
<li>void ExitLog();</li></b>
</ul></li>

<li>The logger will implement the above functions in its CPP file (<b>Logger.cpp</b>):

<ul>
<li><b>InitializeLog()</b> will
<ul>
<li>create a non-blocking socket for UDP communications (<b>AF_INET, SOCK_DGRAM</b>).</li>
<li>Set the address and port of the server.</li>
<li>Create a mutex to protect any shared resources.</li>
<li>Start the receive thread and pass the file descriptor to it.</li>
</ul></li>

<li><b>SetLogLevel()</b> will set the filter log level and store in a variable global within Logger.cpp.</li>

<li><b>Log()</b> will
<ul>
<li>compare the severity of the log to the filter log severity. The log will be thrown away if its severity is lower than the filter log severity.</li>
<li>create a timestamp to be added to the log message. Code for creating the log message will look something like:<br>
                       <b>time_t now = time(0);<br>
                          char *dt = ctime(&now);<br>
                          memset(buf, 0, BUF_LEN);<br>
                          char levelStr[][16]={"DEBUG", "WARNING", "ERROR", "CRITICAL"};<br>
                          len = sprintf(buf, "%s %s %s:%s:%d %s\n", dt, levelStr[level], file, func, line, message)+1;<br>
                          buf[len-1]='\0';</b></li>
<li>apply mutexing to any shared resources used within the <b>Log()</b> function.</li>
<li>The message will be sent to the server via UDP <b>sendto()</b>.</li>
</ul>
</li>

<li>ExitLog() will stop the receive thread via an <b>is_running</b> flag and close the file descriptor.</li>

<li>The receive thread is waiting for any commands from the server. So far there is only one command from the server: "<b>Set Log Level=&lt;level&gt;</b>". The receive thread will
<ul>
<li>accept the file descriptor as an argument.
<li>run in an endless loop via an <b>is_running</b> flag.
<li>apply mutexing to any shared resources used within the <b>recvfrom()</b> function.
<li>ensure the <b>recvfrom()</b> function is non-blocking with a sleep of 1 second if nothing is received.
<li>act on the command "<b>Set Log Level=&lt;level&gt;</b>" from the server to overwrite the filter log severity.
</ul>
</li>
</ul>
</li><!--The logger will implement...-->

<b>The Server:</b>
<ul>
<li>The server will shutdown gracefully via a ctrl-C via a <b>shutdownHandler</b>.</li>
<li>The server's <b>main()</b> function will create a non-blocking socket for UDP communications (<b>AF_INET, SOCK_DGRAM</b>).</li>
<li>The server's <b>main()</b> function will bind the socket to its IP address and to an available port.</li>
<li>The server's <b>main()</b> function will create a mutex and apply mutexing to any shared resources.</li>
<li>The server's <b>main()</b> function will start a receive thread and pass the file descriptor to it.</li>

<li>The server's <b>main()</b> function will present the user with three options via a user menu:

<ul>
<li>1.Set the log level
<ul>
<li>The user will be prompted to enter the filter log severity.</li>
<li>The information will be sent to the logger. Sample code will look something like:<br>
<b>memset(buf, 0, BUF_LEN);<br>
len=sprintf(buf, "Set Log Level=%d", level)+1;<br>
sendto(fd, buf, len, 0, (struct sockaddr *)&remaddr, addrlen);</b></li></ul></li>
<li>2.Dump the log file here
<ul>
<li>The server will open its server log file for read only.</li>
<li>It will read the server's log file contents and display them on the screen.</li>
<li>On completion, it will prompt the user with:<br>
"<b>Press any key to continue:</b>"</li></ul></li>
<li>0. Shut down
<ul>
<li>The receive thread will be shutdown via an <b>is_running</b> flag.
<li>The server will exit its user menu.
<li>The server will join the receive thread to itself so it doesn't shut down before the receive thread does.</li></ul></li>
</ul>
</li>

<li>The server's receive thread will:
<ul>
<li>open the server log file for write only with permissions <b>rw-rw-rw-</b></li>
<li>run in an endless while loop via an <b>is_running</b> flag.</li>
<li>apply mutexing to any shared resources used within the <b>recvfrom()</b> function.</li>
<li>ensure the <b>recvfrom()</b> function is non-blocking with a sleep of 1 second if nothing is received.</li>
<li>take any content from <b>recvfrom()</b> and write to the server log file.</li>
</ul>
</li>
</ul>

<p>On the process side you have been provided with a 
    <a href="logger/Makefile" target="_blank">Makefile</a>, 
    <a href="logger/TravelSimulator.cpp" target="_blank">TravelSimulator.cpp</a>, 
    <a href="logger/Automobile.cpp" target="_blank">Automobile.cpp</a>, and 
    <a href="logger/Automobile.h" target="_blank">Automobile.h</a>. You have to implement <b>Logger.cpp</b> and <b>Logger.h</b>.</p>

<p>On the server side you have to implement <b>LogServer.cpp</b> and you also have to create a <b>Makefile</b>.</p>

<h4><b><u>Testing and Rework</u></b> (Junior and Senior Engineers, Product Support)</h4>
<ul>
<li>Start the server on one machine. Make sure you have the correct IP address and port.</li>
<li>Start the test application on another machine, which in this case is called <b>TravelSimulator</b>.</li>
<li>Make sure your logger has the correct IP address and port of the server.</li>
<li>Logging has been added to the <b>TravelSimulator</b>, along with calls to <b>InitializeLog()</b>, <b>SetLogLevel()</b> and <b>ExitLog()</b>.</li>
<li>The <b>TravelSimulator</b> runs indefinitely until the user administers a ctrl-C.</li>
<li>Observe the server log file as it grows either by dumping to the screen or looking at the file itself.</li>
<li>Change the filter log severity on the server.</li>
<li>Observe the server log file again.
<ul>
<li>Is it getting all of the log messages?</li>
<li>Are the log messages in sequence?</li>
<li>For each log message verify the timestamp, log level, file name, function, line number and the message.</li>
</ul>
</li>
</ul>


<h4><b><u>Questions</u></b></h4>
<ol>
<li>Generally, what are syslog and rsyslog? Specifically, name three features of syslog/rsyslog and compare them to your embedded debug logging. Will there be any overlap of information?</li>
<br>
<li>Name five features of syslog-ng.</li>
<br>
<li>Name five ways syslog-ng is an improvement over syslog/rsyslog.</li>
<br>
<li>This assignment assumes one server and one application (the travel simulator plus the logger). How would the design of your server change if there were ten applications instead, each 
on a different machine?</li>
<br>
<li>Consider a Log Server that has to manage embedded logs for a massive amount of applications on a massive amount of machines. With such a large amount of data in the logs,
name three ways a user could extract useful information from them (be general).</li>
<br>
<li>Explain how gdb could be used on a Linux machine to attach to a process and get thread information. Is this also useful in debugging?</li>
</ol>






<h4><b><u>Timeline</u></b></h4>

<p><b>Days 1-7:</b> Create the Logger.</p>

<p><b>Days 8-14:</b> Create the Server.</p>

<p><b>Days 15-21:</b> Test and Rework. Answer questions. Submit assignment.</p>


<h4><b><u>Marking Rubric</u></b></h4>

<p>Assignment 1 is worth 15% of your final grade and as such is marked out of 15 as follows:</p>


<table>
    <tr><th></th><th>Does not meet expectations</th><th>Satisfactory</th><th>Good</th><th>Exceeds Expectations</th></tr>
    <tr><td><b>The Logger<br>(5 marks)</b></td><td>Does not meet requirements</td><td>Meets the most important requirements</td><td>Meets all requirements with minor errors</td><td>Meets all requirements with no errors</td></tr>
    <tr><td><b>The Server<br>(6 marks)</b></td><td>Does not meet requirements</td><td>Meets the most important requirements</td><td>Meets all requirements with minor errors</td><td>Meets all requirements with no errors</td></tr>
    <tr><td><b>Documentation<br>(1 marks)</b></td><td>Does not contain documentation</td><td>Contains header documentation for either all files or for all functions within each file</td>
        <td>Contains header documentation for all files and for most functions within each file</td><td>Contains header documentation for all files and for all functions within each file.
            Documents unclear code.</td></tr>
    <tr><td><b>Questions<br>(3 marks)</b></td><td>Answers no question correctly</td><td>Answers some questions correctly</td>
        <td>Answers most questions correctly</td><td>Answers all Questions correctly</td></tr>
</table>
    
