#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>
#include <arpa/inet.h>

#define NB_MOTS 10

struct _client
{
        char ipAddress[40];
        int port;
        char name[40];
        char words[2][40];
        char guess[40];
        int choix[2];
        int score;
        int role;
        int vrai;
} tcpClients[5];

int nbClients;
int fsmServer;
int deck[5]={0,0,0,1,1};//0 pour contre-espion, 1 pour espion
char *mpts[]={"bas","muse","boulet","spectre","patron","moulin","piano","oseille","billet","Rome"};
int joueurCourant, joueurSuivant, nbReponses, nbReponsesAtt, indiceJ, indiceJp = 5, indiceM = 0;
int ind_WordToGuess;
char noms[5][40];

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void melangerDeck()
{
        int i;
        int index1,index2,tmp;

        for (i=0;i<1000;i++)
        {
                index1=rand()%5;
                index2=rand()%5;

                tmp=deck[index1];
                deck[index1]=deck[index2];
                deck[index2]=tmp;
        }
}

void printDeck()
{
        int i;

        for (i=0;i<5;i++)
                printf("%d:%d\n",i,deck[i]);
}

void affecterRoles()
{
        int i;
        for (i=0;i<5;i++)
        {
                tcpClients[i].role = deck[i];

        }
        printf("Les rôles ont été affectés\n");

}

void razJoueurs()
{
        int i;
        for (i=0;i<5;i++)
        {
                strcpy(tcpClients[i].words[0], "-");
                strcpy(tcpClients[i].words[1],"-");
                tcpClients[i].vrai=0;
        }
        printf("Les mots ont été effacés\n");
}

void printClients()
{
        int i;

        for (i=0;i<nbClients;i++)
                printf("%d: %s %5.5d %s %s %s %d %d\n",i,tcpClients[i].ipAddress, tcpClients[i].port, tcpClients[i].name, tcpClients[i].words[0], tcpClients[i].words[1],tcpClients[i].score, tcpClients[i].role);
}

int findClientByName(char *name)
{
        int i;

        for (i=0;i<nbClients;i++)
                if (strcmp(tcpClients[i].name,name)==0)
                        return i;
        return -1;
}

void sendMessageToClient(char *clientip,int clientport,char *mess)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server = gethostbyname(clientip);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(clientport);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        {
                printf("ERROR connecting\n");
                exit(1);
        }

        sprintf(buffer,"%s\n",mess);
        n = write(sockfd,buffer,strlen(buffer));

    close(sockfd);
}

void broadcastMessage(char *mess)
{
        int i;

        for (i=0;i<nbClients;i++)
                sendMessageToClient(tcpClients[i].ipAddress,
                        tcpClients[i].port,
                        mess);
}

void broadcastRoles()
{
        char val[80];
        for(int i=0; i<nbClients; i++)
        {
                sprintf(val, "R %d", tcpClients[i].role);
                sendMessageToClient(tcpClients[i].ipAddress, tcpClients[i].port, val);   
        }
        printf("les roles ont été broadcastés à l'ensemble des joueurs\n");
}

void broadcastWord()
{
        char val[80];
        ind_WordToGuess = rand()%NB_MOTS;
        sprintf(val, "G %s", mpts[ind_WordToGuess]);
        for(int i=0; i<nbClients; i++)
        {
                if (tcpClients[i].role == 1)
                {
                        sendMessageToClient(tcpClients[i].ipAddress, tcpClients[i].port, val);                           
                }
        }
}

void resultatEspion(){
        int flagEspion = 0;
        for(int i=0; i<5; i++){
                if(tcpClients[i].role){//Cas des espions
                        if(tcpClients[tcpClients[i].choix[1]].role){//si il trouve l'autre espion
                                tcpClients[i].score+=3;
                        }
                        else{
                                flagEspion = 1;//Tous les contre-espion recoivent 1 point
                                tcpClients[i].score--;//l'espion qui s'est trompé perd un point
                                tcpClients[tcpClients[i].choix[1]].score++;//qu'il donne au contre-espion
                        }
                }
                else{//Cas des contre-espions
                        if(tcpClients[tcpClients[i].choix[0]].role && tcpClients[tcpClients[i].choix[1]].role){//si il trouve les deux espions
                                tcpClients[tcpClients[i].choix[0]].score--;
                                tcpClients[tcpClients[i].choix[1]].score--;
                                tcpClients[i].score+=2;
                                tcpClients[i].vrai=1;
                        }
                }
        }
        if(flagEspion){
                for(int i=0; i<5; i++){
                        if(tcpClients[i].role==0){
                                tcpClients[i].score++;
                        }
                }
        }
}

void resultatMot(){
        for(int i=0; i<5; i++){
                if(tcpClients[i].vrai){
                        if(strcmp(tcpClients[i].guess, mpts[ind_WordToGuess])==0){
                                tcpClients[i].score+=5;
                        }
                }
        }
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     int i;
     nbReponsesAtt = 0;

     char com;
     char clientIpAddress[256], clientName[256];
     int clientPort;
     int id;
     char reply[256];


     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);

        printDeck();
        melangerDeck();
        printDeck();
        joueurCourant=0;

        for (i=0;i<5;i++)
        {
                strcpy(tcpClients[i].ipAddress,"localhost");
                tcpClients[i].port=-1;
                strcpy(tcpClients[i].name,"-");
                strcpy(tcpClients[i].words[0],"-");
                strcpy(tcpClients[i].words[1],"-");
                strcpy(tcpClients[i].guess,"-");
                tcpClients[i].score = 0;
                tcpClients[i].vrai = 0;
                strcpy(noms[i], "-");
        }

     while (1)
     {    

        //==============================================================================================================================================
        printf("Jusqu'ici tout va bien (mais le sol arrive) : server_linq.c (ligne 272)\n");
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);//cette ligne pose problème si aucun contre-espion ne trouve les deux espions
        printf("Bon bah on s'est pris le sol si ce message ne s'affiche pas.\n");
        //==============================================================================================================================================


        if (newsockfd < 0) {
                error("ERROR on accept");
        }
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        if (n < 0) 
                error("ERROR reading from socket");
        printf("Received packet from %s:%d\nData: [%s]\n\n",
                inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer);
        if (fsmServer==0)
        {
                switch (buffer[0])
                {
                        case 'C':
                                sscanf(buffer,"%c %s %d %s", &com, clientIpAddress, &clientPort, clientName);
                                printf("COM=%c ipAddress=%s port=%d name=%s\n",com, clientIpAddress, clientPort, clientName);

                                // fsmServer==0 alors j'attends les connexions de tous les joueurs
                                strcpy(tcpClients[nbClients].ipAddress,clientIpAddress);
                                tcpClients[nbClients].port=clientPort;
                                strcpy(tcpClients[nbClients].name,clientName);
                                nbClients++;

                                printClients();

                                // rechercher l'id du joueur qui vient de se connecter

                                id=findClientByName(clientName);
                                printf("id=%d\n",id);

                                // lui envoyer un message personnel pour lui communiquer son id

                                sprintf(reply,"I %d",id);
                                sendMessageToClient(tcpClients[id].ipAddress,tcpClients[id].port,reply);

                                // Envoyer un message broadcast pour communiquer a tout le monde la liste des joueurs actuellement
                                // connectes

                                sprintf(reply,"L %s %s %s %s %s", tcpClients[0].name, tcpClients[1].name, tcpClients[2].name, tcpClients[3].name,tcpClients[4].name);
                                broadcastMessage(reply);

                                // Si le nombre de joueurs atteint 4, alors on peut lancer le jeu

                                if (nbClients==5)
                                {
                                        fsmServer=1;
                                        melangerDeck();
                                        affecterRoles();
                                        razJoueurs();//mots que chaque joueur a dit pour la manche précédente doivent être remis à zéro
                                        broadcastRoles();
                                        broadcastWord();//tiere un mot au hasard et l'envoyer le mot pour les espions UNIQUEMENT
                                        printClients();
                                        joueurSuivant=0;
                                        nbReponses=0;
                                }
                                break;
                }
        }
        else if (fsmServer==1)
        {
                switch (buffer[0])
                {
                        case 'M':
                                {
                                        sscanf(buffer+2,"%d", &indiceJ);
                                        if(indiceJ != indiceJp)
                                        {
                                                sscanf(buffer+4,"%s",tcpClients[indiceJ].words[indiceM]);
                                                indiceJp=indiceJ;
                                                joueurSuivant = indiceJ+1;
                                                if(joueurSuivant==5){
                                                        joueurSuivant=0;
                                                        indiceM=1;
                                                }
                                                nbReponses++;
                                                if(nbReponses==10)
                                                {
                                                        indiceM = 0;
                                                        nbReponses = 0;
                                                        indiceJp = 5;
                                                        fsmServer = 2;                                                
                                                }
                                        }
                                        sprintf(reply,"W %s %s %s %s %s %s %s %s %s %s %d", tcpClients[0].words[0], tcpClients[1].words[0], tcpClients[2].words[0], tcpClients[3].words[0],tcpClients[4].words[0], tcpClients[0].words[1], tcpClients[1].words[1], tcpClients[2].words[1], tcpClients[3].words[1],tcpClients[4].words[1], joueurSuivant);
                                        broadcastMessage(reply);
                                }
                        default:
                                break;
                }
        }
        else if (fsmServer==2)
        {
                switch (buffer[0])
                {
                        case 'S':
                                {
                                        sscanf(buffer+2,"%d", &indiceJ);
                                        if(indiceJ != indiceJp)
                                        {
                                                sscanf(buffer+4,"%d %d",&(tcpClients[indiceJ].choix[0]), &(tcpClients[indiceJ].choix[1]));
                                                indiceJp=indiceJ;
                                                nbReponses++;
                                                if(nbReponses==5)
                                                {
                                                        nbReponses = 0;
                                                        indiceJp = 5;  
                                                        fsmServer = 3;
                                                        resultatEspion();
                                                        for(int i=0; i<5; i++){
                                                                if(tcpClients[i].vrai){
                                                                        nbReponsesAtt++;
                                                                }
                                                                sprintf(reply,"S 4 %d",tcpClients[i].vrai);
                                                                sendMessageToClient(tcpClients[i].ipAddress,tcpClients[i].port,reply);
                                                        }
                                                        printf("Nombre de réponses attendues : %d\nfsmServer : %d\n", nbReponsesAtt, fsmServer);                              
                                                }
                                        }
                                }
                                break;
                        default:
                                break;
                }
        }
        else if(fsmServer==3)
        {
                printf("fsmServer pour la deuxième fois : %d\n", fsmServer);
                if(nbReponsesAtt)
                {
                        printf("On attend les mots\n");
                        switch (buffer[0])
                        {
                                case 'D':
                                        {
                                                sscanf(buffer+2,"%d", &indiceJ);
                                                if(indiceJ != indiceJp)
                                                {
                                                        sscanf(buffer+4,"%s",tcpClients[indiceJ].guess);
                                                        indiceJp=indiceJ;
                                                        nbReponsesAtt--;
                                                        if(nbReponsesAtt==0)
                                                        {
                                                                resultatMot();
                                                                int A=5, B;
                                                                for(int i=0; i<5; i++){
                                                                        if(tcpClients[i].role){
                                                                                if(A==5){
                                                                                        A=i;
                                                                                }
                                                                                else{
                                                                                        B=i;
                                                                                }
                                                                        }
                                                                        strcpy(noms[i], "-");
                                                                }
                                                                indiceJp = 5;
                                                                fsmServer = 4;
                                                                sprintf(reply,"R %d %d %s %d %d %d %d %d", A, B, mpts[ind_WordToGuess], tcpClients[0].score, tcpClients[1].score, tcpClients[2].score, tcpClients[3].score, tcpClients[4].score);
                                                                broadcastMessage(reply);                           
                                                        }
                                                }
                                        }
                                        break;
                                default:
                                        break;
                        }
                }
                else
                {
                        printf("On envoie directement les résultats\n");
                        int A=5, B;
                        for(int i=0; i<5; i++){
                                if(tcpClients[i].role){
                                        if(A==5){
                                                A=i;
                                        }
                                        else{
                                                B=i;
                                        }
                                }
                                strcpy(noms[i], "-");
                        }
                        indiceJp = 5;
                        fsmServer = 4;
                        sprintf(reply,"R %d %d %s %d %d %d %d %d", A, B, mpts[ind_WordToGuess], tcpClients[0].score, tcpClients[1].score, tcpClients[2].score, tcpClients[3].score, tcpClients[4].score);
                        broadcastMessage(reply); 
                        printf("résultats envoyés\n");
                }
        }
        else if (fsmServer==4)
        {
                switch (buffer[0])
                {
                        case 'N':
                        {
                                sscanf(buffer+2,"%d", &indiceJ);
                                if(indiceJ != indiceJp)
                                {
                                        indiceJp=indiceJ;
                                        nbReponses++;
                                        strcpy(noms[indiceJ], tcpClients[indiceJ].name);
                                        sprintf(reply,"L %s %s %s %s %s", noms[0], noms[1], noms[2], noms[3],noms[4]);
                                        broadcastMessage(reply); 
                                        if(nbReponses==5)
                                        {
                                                indiceJp = 5;
                                                fsmServer=1;
                                                melangerDeck();
                                                affecterRoles();
                                                razJoueurs();//mots que chaque joueur a dit pour la manche précédente doivent être remis à zéro
                                                broadcastRoles();
                                                broadcastWord();//tiere un mot au hasard et l'envoyer le mot pour les espions UNIQUEMENT
                                                printClients();
                                                joueurSuivant=0;
                                                nbReponses=0;                       
                                        }
                                }
                        }
                        default:
                                break;
                }
        }
        close(newsockfd);
     }
     close(sockfd);
     return 0; 
}

