#include <errno.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <unistd.h>

#define NAME_SIZE 16

using namespace std;

int main()
{
    int fd;
    int ret;
    int selection;
    struct ifreq ifr;
    char if_name[NAME_SIZE];
    unsigned char *mac = NULL;

    cout << "Enter the interface name: ";
    cin >> if_name;

    size_t if_name_len = strlen(if_name);
    if (if_name_len < sizeof(ifr.ifr_name))
    {
        memcpy(ifr.ifr_name, if_name, if_name_len);
        ifr.ifr_name[if_name_len] = 0;
    }
    else
    {
        cout << "Interface name is too long!" << endl;
        return -1;
    }

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        cout << strerror(errno);
        return -1;
    }

    system("clear");
    do
    {
        cout << "Choose from the following:" << endl;
        cout << "1. Hardware address" << endl;
        cout << "2. IP address" << endl;
        cout << "3. Network mask" << endl;
        cout << "4. Broadcast address" << endl;
        cout << "0. Exit" << endl
             << endl;
        cin >> selection;
        switch (selection)
        {
        case 1:
            ret = ioctl(fd, SIOCGIFHWADDR, &ifr);
            if (ret < 0)
            {
                cout << strerror(errno);
            }
            else if (ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER)
            {
                cout << "not an Ethernet interface" << endl;
            }
            else
            {
                mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
                printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            }
            break;
        case 2:

            // repeat the same process for case 3 and 4, only changing the "Flag word for the action" in ioctl,
            //  and sa family whereever is needed.

            // to test your code, type "ifconfig" in your linux terminal, copy your interface name. That is what you would input in your code.
            //
            ret = ioctl(fd, SIOCGIFHWADDR, &ifr);
            if (ret < 0)
            {
                cout << strerror(errno);
            }
            else if (ifr.ifr_addr.sa_family != AF_INET)
            {
                cout << "Not IP address" << endl;
            }
            else
            {
                struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;
                cout << ("System IP Address is: ");
                cout << inet_ntoa(addr->sin_addr);
            }
            break;
        case 3:
            // SIOCGIFHWADDR is for ip_address. Network mask: SIOCGIFNETMASK (check man7.org for others)
            ret = ioctl(fd, SIOCGIFHWADDR, &ifr);
            if (ret < 0)
            {
                cout << strerror(errno);
            }
            // sa_family of network mask is ifr_netmask. so: ifr.ifr_netmask.sa_family
            else if (ifr.ifr_addr.sa_family != AF_INET)
            {
                cout << "Not an Ethernet interface" << endl;
            }
            else
            {
                // Notice i changed the sa_family here too
                mac = (unsigned char *)inet_ntoa(((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr);
                printf("Network Mask: %s \n", mac); // use printf, cos mac is C type
            }
            break;
        case 4:

            ret = ioctl(fd, SIOCGIFHWADDR, &ifr);
            if (ret < 0)
            {
                cout << strerror(errno);
            }
            else if (ifr.ifr_addr.sa_family != AF_INET)
            {
                cout << "Not an Ethernet interface" << endl;
            }
            else
            {
                struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_broadaddr;
                cout << ("System Broadcast Address is: ");
                cout << inet_ntoa(addr->sin_addr);
            }

            break;
        }
        if (selection != 0)
        {
            char key;
            cout << "Press any key to continue: ";
            cin >> key;
            system("clear");
        }
    } while (selection != 0);

    close(fd);
    return 0;
}
