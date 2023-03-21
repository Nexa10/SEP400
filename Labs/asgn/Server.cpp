// Server.cpp 
//
// 01-Mar-23  Yiyuan Dong         Created.
//

/* --- includes ---------------------------

---------------------------------------- */

// --- Declarations ---------------------
//  The server will shutdown gracefully via a ctrl-C via a shutdownHandler.


//  Recv function


//  Global variables


//  -------------------------------------
int main(int argc, char const *argv[])
{

    //  The server's main() function will create a non-blocking socket for UDP communications (AF_INET, SOCK_DGRAM).
    
    //  The server's main() function will bind the socket to its IP address and to an available port.

    //  The server's main() function will create a mutex and apply mutexing to any shared resources.

    //  The server's main() function will start a receive thread and pass the file descriptor to it.

    //  Show options in a while loop
    /*
        The server's main() function will present the user with three options via a user menu:
        1.Set the log level
            The user will be prompted to enter the filter log severity.
            The information will be sent to the logger. Sample code will look something like:
            memset(buf, 0, BUF_LEN);
            len=sprintf(buf, "Set Log Level=%d", level)+1;
            sendto(fd, buf, len, 0, (struct sockaddr *)&remaddr, addrlen);
        2.Dump the log file here
            The server will open its server log file for read only.
            It will read the server's log file contents and display them on the screen.
            On completion, it will prompt the user with:
            "Press any key to continue:"
        0. Shut down
            The receive thread will be shutdown via an is_running flag.
            The server will exit its user menu.
            The server will join the receive thread to itself so it doesn't shut down before the receive thread does.
    */      

    return 0;
}


//  Recv function implemetation
/*  The server's receive thread will:
        open the server log file for write only with permissions rw-rw-rw-
        run in an endless while loop via an is_running flag.
        apply mutexing to any shared resources used within the recvfrom() function.
        ensure the recvfrom() function is non-blocking with a sleep of 1 second if nothing is received.
        take any content from recvfrom() and write to the server log file.
*/
