
#include<iostream>    //cout
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<string>  //string
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<netdb.h> //hostent

using namespace std;

/**
 Classe utilitaire de la connexion TCP
 */
class tcp_client
{
private:
    int sock;
    std::string address;
    int port;
    struct sockaddr_in server;
    bool isMsgValid(unsigned char arr[], int len);
    
public:
    tcp_client();
    bool conn(string, int);
    bool send_data(string data);
    bool send_data_stream();
    string receive(int);
    
    bool check_message(unsigned char message[]);
    void display_message(unsigned char message[]);
};

// Constructeur
tcp_client::tcp_client()
{
    sock = -1;
    port = 0;
    address = "";
}

// Connexion à un hôte sur un port défini
bool tcp_client::conn(string address , int port)
{
    // créer le socket si pas connecté
    if(sock == -1)
    {
        // création du socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)
        {
            perror("Could not create socket");
        }
        
        cout<<"Socket created\n";
    }
    else    {   /* OK , nothing */  }
    
    //setup address structure
    if(inet_addr(address.c_str()) == -1)
    {
        struct hostent *he;
        struct in_addr **addr_list;
        
        // résolution du nom de l'hôte (pas d'adresse IP passée)
        if ( (he = gethostbyname( address.c_str() ) ) == NULL)
        {
            herror("gethostbyname");
            cout<<"Failed to resolve hostname\n";
            
            return false;
        }
        
        // transtypage de h_addr_list en in_addr
        addr_list = (struct in_addr **) he->h_addr_list;
        
        for(int i = 0; addr_list[i] != NULL; i++)
        {
            //strcpy(ip , inet_ntoa(*addr_list[i]) );
            server.sin_addr = *addr_list[i];
            
            cout<<address<<" resolved to "<<inet_ntoa(*addr_list[i])<<endl;
            
            break;
        }
    }
    
    // plain ip address
    else
    {
        server.sin_addr.s_addr = inet_addr( address.c_str() );
    }
    
    server.sin_family = AF_INET;
    server.sin_port = htons( port );
    
    // connexion au serveur distant
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
    
    cout<<"Connected\n";
    return true;
}

// envoyer des données à l'hôte connecté
bool tcp_client::send_data(string data)
{
    // resultat de l'envoi (nombre de bytes)
    int result = 0;
    
    // tentative de composition d'un message
    unsigned char opened[6];
    opened[0] = 0x02;
    opened[1] = 0x02;
    opened[2] = 0x01;
    opened[3] = 0x31;
    opened[4] = 0x03;
    opened[5] = 0x03;
    
    unsigned char opened2[] = {0x02, 0x02, 0x00, 0x31, 0x02, 0X03};
    
    
    result = send(sock , opened2 , sizeof( opened2 ) , 0);

    if( result < 0)
    {
        perror("Send failed : ");
        return false;
    }
    cout<<"Data send\n";
    cout<<"(" << result << " bytes)\n\n" ;
    
    return true;
}


// Méthode qui permet de recevoir des données du client
string tcp_client::receive(int size=512)
{
    char buffer[size];
    string reply;
    
    // Réception d'une réponse du serveur
    if( recv(sock , buffer , sizeof(buffer) , 0) < 0)
    {
        puts("recv failed");
    }
    
    reply = buffer;
    return reply;
}

// Methode qui permet de vérifier la valeur du message en l'affichant sur la console
void tcp_client::display_message(unsigned char *message) {
    cout << "\n\nValeur du message : \n";
    
    int len;
    
    len = sizeof(message);
    cout << "\nlongueur : \n" << len << "\n";
    for(int i = 0; i < (len-1); ++i) {
        cout << "\npassage " << i << "\n";
//        cout << message[i];
        putchar(    message[i]);
    }
    
    cout << "\n--------------Fin...\n\n";
    cout << "\n--------------printf\n";
    printf("%d", message);
    cout << "\n----------fin printf\n\n";
    
    
    
}

// Méthode qui permet de checker la validité du message
bool tcp_client::isMsgValid(unsigned char arr[], int len) {
    int sum = 0;
    for(int i = 0; i < (len-1); ++i) {
        sum += arr[i];
    }
    //modulo 256 sum
    sum %= 256;
    
    char ch = sum;
    
    //twos complement
    unsigned char twoscompl = ~ch + 1;
    
    return arr[len-1] == twoscompl;
}

bool tcp_client::check_message(unsigned char message[]) {
    int arrsize = sizeof(message) / sizeof(message[0]);
    
    return tcp_client::isMsgValid(message, arrsize);
}

int main(int argc , char *argv[])
{
    tcp_client c;
    string host;
    
    cout<<"Enter hostname (192.168.2.188:5000) : ";

    string s;
    
    if (getline( cin, s ) && s.compare("\n")) {
        cout << "Good job. Host saved\n";
        host = "192.168.0.188";
    }
    else {
        cout << "New server address.\n";
        host = s;
    }
    
    // Affichage du host appelé
//    cout << "valeur de host \"" << host << "\" \n\n";
    
    // connexion à l'host
    c.conn(host , 5000);
    
    // envoi de données
    c.send_data("exemple");
    
    // réception de données et affichage
    cout<<"----------------------------\n\n";
    cout<<"Receiving...\n\n";
    cout<<c.receive(1024);
    cout<<"\n\n----------------------------\n\n";
    cout<<"\n\nEnd of receiving.\n\n";
    
    //done
    return 0;
}




