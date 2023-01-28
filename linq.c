#include <SDL.h>        
#include <SDL_image.h>        
#include <SDL_ttf.h>        
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define Gauche_x 10
#define Droite_x 780
#define Width 200
#define Height 150

#define J1_y 50
#define J2_y 280
#define J3_y 50
#define J4_y 240
#define J5_y 430

pthread_t thread_serveur_tcp_id;
char gbuffer[256];
char gServerIpAddress[256];
int gServerPort;
char gClientIpAddress[256];
int gClientPort;
char gName[256];
char gNames[5][256];
char words[10][40];
int gId;
int goEnabled;
int connectEnabled;
int screenNumber;
char word[40];
char roleaff[300];
int cptWord;
int quit = 0;
SDL_Event event;
int mx,my;
char sendBuffer[1000];
SDL_Window * window;
SDL_Renderer *renderer;
 
SDL_Surface *connectbutton;
SDL_Surface *affiche;
SDL_Surface *replay;
SDL_Texture *texture_connectbutton;
SDL_Texture *texture_affiche;
SDL_Texture *texture_replay;
SDL_Surface *fond[4],*personne;
SDL_Texture *texture_fond[4],*texture_personnes;
TTF_Font* Sans60; 
TTF_Font* Sans20; 
TTF_Font* Sans30; 
int tour = 0;
int role;
int vrai;
char guessWord[40];
int choix=0;
int tabChoix[2];
int choix0=0;
int choix1=0;
int choix2=0;
int choix3=0;
int choix4=0;
char *mot = "  ";
char *letter; //lettre appuyée
char buffer[40]; //résultat
int size=1;
int A, B;
int score0, score1, score2, score3, score4;

volatile int synchro;

void *fn_serveur_tcp(void *arg)
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd<0)
    {
        printf("sockfd error\n");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = gClientPort;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("bind error\n");
        exit(1);
    }

    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
        {
            printf("accept error\n");
            exit(1);
        }

        bzero(gbuffer,256);
        n = read(newsockfd,gbuffer,255);
        if (n < 0)
        {
                printf("read error\n");
                exit(1);
        }
        //printf("%s",gbuffer);

        synchro=1;

        while (synchro);
    }
}

void sendMessageToServer(char *ipAddress, int portno, char *mess)
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char sendbuffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server = gethostbyname(ipAddress);
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        printf("ERROR connecting\n");
        exit(1);
    }

    sprintf(sendbuffer,"%s\n",mess);
    n = write(sockfd,sendbuffer,strlen(sendbuffer));

    close(sockfd);
}

void manageEvent(SDL_Event event)
{
    switch (event.type)
    {
        case SDL_QUIT:
            quit = 1;
            break;
        case SDL_KEYDOWN:
        {
            if(screenNumber==2 && tour==gId){
                if (size+1<=40){
                        switch(event.key.keysym.sym){
                            case SDLK_a : 
                                letter = "a";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_b : 
                                letter = "b";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_c : 
                                letter = "c";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_d : 
                                letter = "d";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_e : 
                                letter = "e";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_f : 
                                letter = "f";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_g : 
                                letter = "g";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_h : 
                                letter = "h";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_i : 
                                letter = "i";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_j : 
                                letter = "j";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_k : 
                                letter = "k";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_l : 
                                letter = "l";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_m : 
                                letter = "m";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_n : 
                                letter = "n";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_o : 
                                letter = "o";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_p : 
                                letter = "p";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_q : 
                                letter = "q";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_r : 
                                letter = "r";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_s : 
                                letter = "s";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_t : 
                                letter = "t";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_u : 
                                letter = "u";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_v : 
                                letter = "v";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_w : 
                                letter = "w";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_x : 
                                letter = "x";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_y: 
                                letter = "y";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_z : 
                                letter = "z";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_BACKSPACE :
                                if (size-1>=0){
                                    mot[strlen(mot)-1] = 0;
                                    size--;
                                }
                                break;
                            case SDLK_RETURN : 
                                printf("%s\n",mot);
                                sprintf(sendBuffer,"M %d %s",gId,mot);
                                sendMessageToServer(gServerIpAddress,gServerPort,sendBuffer);
                                strcpy(mot, "  ");
                                strcpy(buffer, " ");
                                size=1;
                                break;
                        }
                }
                else 
                {
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_BACKSPACE :
                            if (size-1>=0){
                                mot[strlen(mot)-1] = 0;
                                size--;
                            }
                            break;
                    }
                }
            }

            if(screenNumber==4 && role==0 && vrai){
                if (size+1<=40){
                        switch(event.key.keysym.sym){
                            case SDLK_a : 
                                letter = "a";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_b : 
                                letter = "b";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_c : 
                                letter = "c";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_d : 
                                letter = "d";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_e : 
                                letter = "e";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_f : 
                                letter = "f";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_g : 
                                letter = "g";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_h : 
                                letter = "h";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_i : 
                                letter = "i";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_j : 
                                letter = "j";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_k : 
                                letter = "k";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_l : 
                                letter = "l";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_m : 
                                letter = "m";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_n : 
                                letter = "n";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_o : 
                                letter = "o";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_p : 
                                letter = "p";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_q : 
                                letter = "q";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_r : 
                                letter = "r";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_s : 
                                letter = "s";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_t : 
                                letter = "t";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_u : 
                                letter = "u";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_v : 
                                letter = "v";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_w : 
                                letter = "w";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_x : 
                                letter = "x";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_y: 
                                letter = "y";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_z : 
                                letter = "z";
                                strcat(strcpy(buffer,mot),letter);
                                mot = buffer;
                                size++;
                                break;
                            case SDLK_BACKSPACE :
                                if (size-1>=0){
                                    mot[strlen(mot)-1] = 0;
                                    size--;
                                }
                                break;
                            case SDLK_RETURN : 
                                printf("%s\n",mot);
                                sprintf(sendBuffer,"D %d %s",gId,mot);
                                sendMessageToServer(gServerIpAddress,gServerPort,sendBuffer);
                                strcpy(mot, "  ");
                                strcpy(buffer, " ");
                                size=1;
                                break;
                        }
                }
                else 
                {
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_BACKSPACE :
                            if (size-1>=0){
                                mot[strlen(mot)-1] = 0;
                                size--;
                            }
                            break;
                    }
                }
            }

            int car=event.key.keysym.sym;
            //printf("%d\n",event.key.keysym.sym);
            if (car==8)
            {
                    strcpy(word,"");
                    cptWord=0;
            }
            else if ((car>=97) && (car<=122)) 
            {
                    word[cptWord++]=car;
                    word[cptWord]='\0';
            }


            break;
        }
        case  SDL_MOUSEBUTTONDOWN:
            switch(screenNumber)
            {
                case 0:
                    SDL_GetMouseState( &mx, &my );
                    if ((mx<800) && (my<450) &&(mx>400) && (my>50) && (connectEnabled==1))
                    {
                        sprintf(sendBuffer,"C %s %d %s",
                        gClientIpAddress,gClientPort,gName);
                        sendMessageToServer(gServerIpAddress,
                        gServerPort,sendBuffer);
                        connectEnabled=0;
                    }
                    break;
                case 3:
                    SDL_GetMouseState( &mx, &my );
                    if ((mx<300) && (my<450) &&(mx>100) && (my>350))
                    {
                        if(gId!=0 && tabChoix[choix]==-1 && tabChoix[0]!=0){
                            tabChoix[choix]=0;
                            choix++;
                            if(choix==2 && role==0){
                                sprintf(sendBuffer,"S %d %d %d",gId,tabChoix[0],tabChoix[1]);
                                sendMessageToServer(gServerIpAddress, gServerPort,sendBuffer);
                            }
                            else if(role){
                                sprintf(sendBuffer,"S %d %d %d",gId,gId,tabChoix[0]);
                                sendMessageToServer(gServerIpAddress, gServerPort,sendBuffer);
                            }
                        }
                    }
                    if ((mx<300) && (my<300) &&(mx>100) && (my>200))
                    {
                        if(gId!=1 && tabChoix[choix]==-1 && tabChoix[0]!=1){
                            tabChoix[choix]=1;
                            choix++;
                            if(choix==2 && role==0){
                                sprintf(sendBuffer,"S %d %d %d",gId,tabChoix[0],tabChoix[1]);
                                sendMessageToServer(gServerIpAddress, gServerPort,sendBuffer);
                            }
                            else if(role){
                                sprintf(sendBuffer,"S %d %d %d",gId,gId,tabChoix[0]);
                                sendMessageToServer(gServerIpAddress, gServerPort,sendBuffer);
                            }
                        }
                    }
                    if ((mx<600) && (my<150) &&(mx>400) && (my>50))
                    {
                        if(gId!=2 && tabChoix[choix]==-1 && tabChoix[0]!=2){
                            tabChoix[choix]=2;
                            choix++;
                            if(choix==2 && role==0){
                                sprintf(sendBuffer,"S %d %d %d",gId,tabChoix[0],tabChoix[1]);
                                sendMessageToServer(gServerIpAddress, gServerPort,sendBuffer);
                            }
                            else if(role){
                                sprintf(sendBuffer,"S %d %d %d",gId,gId,tabChoix[0]);
                                sendMessageToServer(gServerIpAddress, gServerPort,sendBuffer);
                            }
                        }
                    }
                    if ((mx<900) && (my<300) &&(mx>700) && (my>200))
                    {
                        if(gId!=3 && tabChoix[choix]==-1 && tabChoix[0]!=3){
                            tabChoix[choix]=3;
                            choix++;
                            if(choix==2 && role==0){
                                sprintf(sendBuffer,"S %d %d %d",gId,tabChoix[0],tabChoix[1]);
                                sendMessageToServer(gServerIpAddress, gServerPort,sendBuffer);
                            }
                            else if(role){
                                sprintf(sendBuffer,"S %d %d %d",gId,gId,tabChoix[0]);
                                sendMessageToServer(gServerIpAddress, gServerPort,sendBuffer);
                            }
                        }
                    }
                    if ((mx<900) && (my<450) &&(mx>700) && (my>350))
                    {
                        if(gId!=4 && tabChoix[choix]==-1 && tabChoix[0]!=4){
                            tabChoix[choix]=4;
                            choix++;
                            if(choix==2 && role==0){
                                sprintf(sendBuffer,"S %d %d %d",gId,tabChoix[0],tabChoix[1]);
                                sendMessageToServer(gServerIpAddress, gServerPort,sendBuffer);
                            }
                            else if(role){
                                sprintf(sendBuffer,"S %d %d %d",gId,gId,tabChoix[0]);
                                sendMessageToServer(gServerIpAddress, gServerPort,sendBuffer);
                            }
                        }
                    }
                    break;
                case 5:
                    SDL_GetMouseState( &mx, &my );
                    if ((mx<900) && (my<400) &&(mx>500) && (my>200))
                    {
                        sprintf(sendBuffer,"N %d",gId);
                        sendMessageToServer(gServerIpAddress, gServerPort,sendBuffer);

                        //reset des variables
                        for(int i=0; i<10; i++){
                            strcpy(words[i], "-");
                        }
                        tabChoix[0]=-1;
                        tabChoix[1]=-1;
                        cptWord=0;
                        tour = 0;
                        vrai =0;
                        choix=0;
                        choix0=0;
                        choix1=0;
                        choix2=0;
                        choix3=0;
                        choix4=0;
                        mot = " ";
                        size=1;
                        screenNumber = 6;
                    }
                    break;
                default:
                    break;
           }
     }
}

void manageNetwork()
{
    if (synchro==1)
    {
        printf("consomme |%s\n",gbuffer);
        switch (screenNumber)
        {
            case 0:
                switch (gbuffer[0])
                {
                    // Message 'I' : le joueur recoit son Id
                    case 'I':
                    sscanf(gbuffer+2,"%d",&gId);
                    screenNumber=1;
                    break;
                }
                break;
            case 1:
                switch (gbuffer[0])
                {
                // Message 'L' : le joueur recoit les noms des joueurs connectés 
                case 'L':
                    sscanf(gbuffer+2,"%s %s %s %s %s", gNames[0], gNames[1], gNames[2], gNames[3], gNames[4]);                        
                    break;
                case 'R':
                        sscanf(gbuffer+2,"%d", &role);
                        if(role==0){
                              screenNumber=2;  
                        }
                        break;
                case 'G':
                        sscanf(gbuffer+2, "%s", guessWord);
                        screenNumber=2;
                        break;
                }
                break;
            case 2:
                if(tour==gId){
                        printf("\nA toi d'écrire : ");
                }
                switch(gbuffer[0])
                {
                        case 'W':
                                sscanf(gbuffer+2,"%s %s %s %s %s %s %s %s %s %s %d",words[0],words[1],words[2],words[3],words[4],words[5],words[6],words[7],words[8],words[9], &tour);
                                if (strcmp(words[9],"-")!=0)
                                        screenNumber=3;
                                break;
                        default:
                            break;
                }
                break;
            case 3 :
                switch(gbuffer[0])
                {
                        case 'S':
                                sscanf(gbuffer+2,"%d %d", &screenNumber, &vrai);
                                break;
                        default:
                            break;
                }
                break;
            case 4 :
                switch(gbuffer[0])
                {
                        case 'R':
                                sscanf(gbuffer+2,"%d %d %s %d %d %d %d %d", &A, &B, word, &score0, &score1, &score2, &score3, &score4);
                                screenNumber = 5;
                                break;
                        default:
                            break;
                }
                break;
            case 6 :
                switch(gbuffer[0])
                {
                        case 'L':
                                sscanf(gbuffer+2,"%s %s %s %s %s", gNames[0], gNames[1], gNames[2], gNames[3], gNames[4]);
                                break;
                        case 'R':
                                sscanf(gbuffer+2,"%d", &role);
                                if(role==0){
                                      screenNumber=2;  
                                }
                                break;
                        case 'G':
                                sscanf(gbuffer+2, "%s", guessWord);
                                screenNumber=2;
                                break;
                        default:
                            break;
                }
                break;
            default:
                break;

        }
        synchro=0;
    }
}

void myRenderText(char *m,int x,int y, int taille)
{
    SDL_Color col1 = {0, 0, 0};
    SDL_Surface* surfaceMessage;
    if(taille==60)
        surfaceMessage = TTF_RenderText_Solid(Sans60, m, col1);
    else if(taille==30)
        surfaceMessage = TTF_RenderText_Solid(Sans30, m, col1);
    else if(taille==20)
        surfaceMessage = TTF_RenderText_Solid(Sans20, m, col1);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect Message_rect;
    Message_rect.x = x;
    Message_rect.y = y;
    Message_rect.w = surfaceMessage->w;
    Message_rect.h = surfaceMessage->h;

    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
    SDL_DestroyTexture(Message);
    SDL_FreeSurface(surfaceMessage);
}

void manageRedraw()
{
    switch (screenNumber)
    {
        case 0:
            // On efface l'écran
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 230);
            SDL_Rect rect = {0, 0, 1000, 500};
            SDL_RenderFillRect(renderer, &rect);

            // si connectEnabled, alors afficher connect
            if (connectEnabled==1)
            {
                SDL_Rect dstrect = {400, 50, 400, 400 };
                SDL_RenderCopy(renderer, texture_connectbutton, NULL, &dstrect);
                SDL_Rect aff = {0, 0, 250, 500 };
                SDL_RenderCopy(renderer, texture_affiche, NULL, &aff);
            }

            if (cptWord>0)
            {
                myRenderText(word,105,350,60);
            }
            break;
        case 1:
        {
            if(gId==0)
                SDL_SetRenderDrawColor(renderer, 100, 255, 100, 230);//vert
            if(gId==1)
                SDL_SetRenderDrawColor(renderer, 100, 100, 255, 230);//bleu
            if(gId==2)
                SDL_SetRenderDrawColor(renderer, 255, 100, 255, 230);//violet
            if(gId==3)
                SDL_SetRenderDrawColor(renderer, 255, 100, 100, 230);//rouge
            if(gId==4)
                SDL_SetRenderDrawColor(renderer, 255, 255, 100, 230);//jaune
            SDL_Rect rect = {0, 0, 1000, 500};
            SDL_RenderFillRect(renderer, &rect);

            myRenderText("Personnes connectees :", 100, 50,60);
            myRenderText(gNames[0],150,100,60);
            myRenderText(gNames[1],150,160,60);
            myRenderText(gNames[2],150,210,60);
            myRenderText(gNames[3],150,260,60);
            myRenderText(gNames[4],150,310,60);
            myRenderText("Le jeu va bientot commencer ...", 50, 410,60);
            break;
        }
        case 2:
        {
            // On efface l'écran
            if(role){
                SDL_SetRenderDrawColor(renderer, 255, 100, 100, 230);//rouge
            }  
            else{
                SDL_SetRenderDrawColor(renderer, 100, 255, 100, 230);//vert
            }
            SDL_Rect rect = {0, 0, 1000, 500};
            SDL_RenderFillRect(renderer, &rect);

            SDL_Rect fondN = { 25, 25, 950, 440 };
            SDL_RenderCopy(renderer, texture_fond[2], NULL, &fondN);
            SDL_Rect fondJ = { 330, 250, 340, 100 };
            SDL_RenderCopy(renderer, texture_fond[3], NULL, &fondJ);

            SDL_Rect pers1 = { 100, 350, 200, 100 };
            SDL_RenderCopy(renderer, texture_personnes, NULL, &pers1);
            SDL_Rect pers2 = { 100, 200, 200, 100 };
            SDL_RenderCopy(renderer, texture_personnes, NULL, &pers2);
            SDL_Rect pers3 = { 400, 50, 200, 100 };
            SDL_RenderCopy(renderer, texture_personnes, NULL, &pers3);
            SDL_Rect pers4 = { 700, 200, 200, 100 };
            SDL_RenderCopy(renderer, texture_personnes, NULL, &pers4);
            SDL_Rect pers5 = { 700, 350, 200, 100 };
            SDL_RenderCopy(renderer, texture_personnes, NULL, &pers5);

            myRenderText(gNames[0], 110, 350 ,30);
            myRenderText(gNames[1], 110, 200 ,30);
            myRenderText(gNames[2], 410, 50 ,30);
            myRenderText(gNames[3], 710, 200 ,30);
            myRenderText(gNames[4], 710, 350 ,30);

            myRenderText(words[0], 120, 380 ,20);
            myRenderText(words[1], 120, 240 ,20);
            myRenderText(words[2], 420, 90 ,20);
            myRenderText(words[3], 720, 240 ,20);
            myRenderText(words[4], 720, 390 ,20);

            myRenderText(words[5], 120, 410 ,20);
            myRenderText(words[6], 120, 270 ,20);
            myRenderText(words[7], 420, 120 ,20);
            myRenderText(words[8], 720, 270 ,20);
            myRenderText(words[9], 720, 420 ,20);

            if(role){
                strcpy(roleaff, "Vous etes ESPION :   ");
                strcat(roleaff, guessWord);
            }  
            else{
                strcpy(roleaff, "Vous etes CONTRE-ESPION");
            }
            myRenderText(roleaff, 25, 465 ,30);

            strcpy(roleaff, gNames[tour]);
            strcat(roleaff, ", quel ton mot ?");
            myRenderText(roleaff,335,250,30);
            if(tour==gId)
                myRenderText(mot,335,300,30);
            break;
        }
        case 3 :
        {
            if(role){
                SDL_SetRenderDrawColor(renderer, 255, 100, 100, 230);//rouge
            }  
            else{
                SDL_SetRenderDrawColor(renderer, 100, 255, 100, 230);//vert
            }
            SDL_Rect rect = {0, 0, 1000, 500};
            SDL_RenderFillRect(renderer, &rect);

            SDL_Rect fondN = { 25, 25, 950, 440 };
            SDL_RenderCopy(renderer, texture_fond[2], NULL, &fondN);
            SDL_Rect fondJ = { 330, 250, 340, 140 };
            SDL_RenderCopy(renderer, texture_fond[3], NULL, &fondJ);

            SDL_Rect pers1 = { 100, 350, 200, 100 };
            SDL_RenderCopy(renderer, texture_personnes, NULL, &pers1);
            SDL_Rect pers2 = { 100, 200, 200, 100 };
            SDL_RenderCopy(renderer, texture_personnes, NULL, &pers2);
            SDL_Rect pers3 = { 400, 50, 200, 100 };
            SDL_RenderCopy(renderer, texture_personnes, NULL, &pers3);
            SDL_Rect pers4 = { 700, 200, 200, 100 };
            SDL_RenderCopy(renderer, texture_personnes, NULL, &pers4);
            SDL_Rect pers5 = { 700, 350, 200, 100 };
            SDL_RenderCopy(renderer, texture_personnes, NULL, &pers5);

            myRenderText(gNames[0], 110, 350 ,30);
            myRenderText(gNames[1], 110, 200 ,30);
            myRenderText(gNames[2], 410, 50 ,30);
            myRenderText(gNames[3], 710, 200 ,30);
            myRenderText(gNames[4], 710, 350 ,30);

            myRenderText(words[0], 120, 380 ,20);
            myRenderText(words[1], 120, 240 ,20);
            myRenderText(words[2], 420, 90 ,20);
            myRenderText(words[3], 720, 240 ,20);
            myRenderText(words[4], 720, 390 ,20);

            myRenderText(words[5], 120, 410 ,20);
            myRenderText(words[6], 120, 270 ,20);
            myRenderText(words[7], 420, 120 ,20);
            myRenderText(words[8], 720, 270 ,20);
            myRenderText(words[9], 720, 420 ,20);

            if(role){
                strcpy(roleaff, "Vous etes ESPION :   ");
                strcat(roleaff, guessWord);
            }  
            else{
                strcpy(roleaff, "Vous etes CONTRE-ESPION");
            }
            myRenderText(roleaff, 25, 465 ,30);

            if(choix==0){
                strcpy(roleaff, "Vous devez maintenant");
                myRenderText(roleaff,335,250,30);
                if(role){
                    strcpy(roleaff, "trouver l'autre espion.");
                }
                else{
                    strcpy(roleaff, "trouver les 2 espions.");
                }
                myRenderText(roleaff,335,280,30);
                if(role){
                    strcpy(roleaff, "Cliquez sur son nom");
                }
                else{
                    strcpy(roleaff, "Cliquez sur leur nom.");
                }
                myRenderText(roleaff,335, 310,30);
            }
            else{
                strcpy(roleaff, "Vous avez choisi :");
                myRenderText(roleaff,335,250,30);
                strcpy(roleaff, gNames[tabChoix[0]]);
                myRenderText(roleaff,335,280,30);
                if(role){
                    strcpy(roleaff, "En attente des autres");
                    myRenderText(roleaff,335, 340,30);
                }
                else{
                    if(choix==2){
                        strcpy(roleaff, gNames[tabChoix[1]]);
                        myRenderText(roleaff,335,310,30);
                        strcpy(roleaff, "En attente des autres");
                        myRenderText(roleaff,335, 340,30);
                    }
                }
            }
            
            break;
        }
        case 4 :
        {
            if(role){
                SDL_SetRenderDrawColor(renderer, 255, 100, 100, 230);//rouge
            }  
            else{
                SDL_SetRenderDrawColor(renderer, 100, 255, 100, 230);//vert
            }
            SDL_Rect rect = {0, 0, 1000, 500};
            SDL_RenderFillRect(renderer, &rect);

            SDL_Rect fondN = { 25, 25, 950, 440 };
            SDL_RenderCopy(renderer, texture_fond[2], NULL, &fondN);
            SDL_Rect fondJ = { 330, 250, 340, 100 };
            SDL_RenderCopy(renderer, texture_fond[3], NULL, &fondJ);

            SDL_Rect pers1 = { 100, 350, 200, 100 };
            SDL_RenderCopy(renderer, texture_personnes, NULL, &pers1);
            SDL_Rect pers2 = { 100, 200, 200, 100 };
            SDL_RenderCopy(renderer, texture_personnes, NULL, &pers2);
            SDL_Rect pers3 = { 400, 50, 200, 100 };
            SDL_RenderCopy(renderer, texture_personnes, NULL, &pers3);
            SDL_Rect pers4 = { 700, 200, 200, 100 };
            SDL_RenderCopy(renderer, texture_personnes, NULL, &pers4);
            SDL_Rect pers5 = { 700, 350, 200, 100 };
            SDL_RenderCopy(renderer, texture_personnes, NULL, &pers5);

            myRenderText(gNames[0], 110, 350 ,30);
            myRenderText(gNames[1], 110, 200 ,30);
            myRenderText(gNames[2], 410, 50 ,30);
            myRenderText(gNames[3], 710, 200 ,30);
            myRenderText(gNames[4], 710, 350 ,30);

            myRenderText(words[0], 120, 380 ,20);
            myRenderText(words[1], 120, 240 ,20);
            myRenderText(words[2], 420, 90 ,20);
            myRenderText(words[3], 720, 240 ,20);
            myRenderText(words[4], 720, 390 ,20);

            myRenderText(words[5], 120, 410 ,20);
            myRenderText(words[6], 120, 270 ,20);
            myRenderText(words[7], 420, 120 ,20);
            myRenderText(words[8], 720, 270 ,20);
            myRenderText(words[9], 720, 420 ,20);

            if(role){
                strcpy(roleaff, "Vous etes ESPION :   ");
                strcat(roleaff, guessWord);
                myRenderText(roleaff, 25, 465 ,30);

                strcpy(roleaff, "Les CONTRE-ESPIONS");
                myRenderText(roleaff,335,250,30);
                strcpy(roleaff, "cherchent le mot.");
                myRenderText(roleaff,335,280,30);
            }  
            else{
                strcpy(roleaff, "Vous etes CONTRE-ESPION");
                myRenderText(roleaff, 25, 465 ,30);

                if(vrai){
                    strcpy(roleaff, "Selon vous, quel est");
                    myRenderText(roleaff,335,250,30);
                    strcpy(roleaff, "le mot secret ?");
                    myRenderText(roleaff,335,280,30);
                    myRenderText(mot,335, 310,30);
                }
                else{
                    strcpy(roleaff, "Vous n'avez pas trouve");
                    myRenderText(roleaff,335,250,30);
                    strcpy(roleaff, "les 2 ESPIONS !!!");
                    myRenderText(roleaff,335,280,30);
                    strcpy(roleaff, "Dommage :)");
                    myRenderText(roleaff,335, 310,30);
                }
            }
                             
            break;
        }
        case 5 :
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 230);//blanc
            SDL_Rect rect = {0, 0, 1000, 500};
            SDL_RenderFillRect(renderer, &rect);
            myRenderText("La partie est terminee !",10, 10,60);
            myRenderText("Les ESPIONS etaient :",10, 90,30);
            myRenderText(gNames[A],10, 120,30);
            myRenderText(gNames[B],10, 150,30);
            strcpy(roleaff, "Le mot secret etait : ");
            strcat(roleaff, word);
            myRenderText(roleaff ,10, 180,30);
            myRenderText("Voici les score : ",10, 210,30);
            sprintf(roleaff, "%s : %d", gNames[0], score0);
            myRenderText(roleaff ,10, 240,30);
            sprintf(roleaff, "%s : %d", gNames[1], score1);
            myRenderText(roleaff ,10, 270,30);
            sprintf(roleaff, "%s : %d", gNames[2], score2);
            myRenderText(roleaff ,10, 300,30);
            sprintf(roleaff, "%s : %d", gNames[3], score3);
            myRenderText(roleaff ,10, 330,30);
            sprintf(roleaff, "%s : %d", gNames[4], score4);
            myRenderText(roleaff ,10, 360,30);

            SDL_Rect replayBUTTON = { 500, 200, 400, 200 };
            SDL_RenderCopy(renderer, texture_replay, NULL, &replayBUTTON);
            break;
        }
        case 6:
        {
            if(gId==0)
                SDL_SetRenderDrawColor(renderer, 100, 255, 100, 230);//vert
            if(gId==1)
                SDL_SetRenderDrawColor(renderer, 100, 100, 255, 230);//bleu
            if(gId==2)
                SDL_SetRenderDrawColor(renderer, 255, 100, 255, 230);//violet
            if(gId==3)
                SDL_SetRenderDrawColor(renderer, 255, 100, 100, 230);//rouge
            if(gId==4)
                SDL_SetRenderDrawColor(renderer, 255, 255, 100, 230);//jaune
            SDL_Rect rect = {0, 0, 1000, 500};
            SDL_RenderFillRect(renderer, &rect);

            myRenderText("Personnes pretes a rejouer :", 100, 50,60);
            myRenderText(gNames[0],150,100,60);
            myRenderText(gNames[1],150,160,60);
            myRenderText(gNames[2],150,210,60);
            myRenderText(gNames[3],150,260,60);
            myRenderText(gNames[4],150,310,60);
            myRenderText("Le jeu va bientot commencer ...", 50, 410,60);
            break;
        }
        default:
            break;
    }
    SDL_RenderPresent(renderer);
}

int main(int argc, char ** argv)
{
    int ret;
    strcpy(word, "");
    for(int i=0; i<10; i++){
        strcpy(words[i], "-");
    }
    vrai = 0;

    if (argc<6)
    {
        printf("<app> <Main server ip address> <Main server port> <Client ip address> <Client port> <player name>\n");
        exit(1);
    }

    strcpy(gServerIpAddress,argv[1]);
    gServerPort=atoi(argv[2]);
    strcpy(gClientIpAddress,argv[3]);
    gClientPort=atoi(argv[4]);
    strcpy(gName,argv[5]);

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
     
    window = SDL_CreateWindow("SDL2 LINQ", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 500, 0);
     
    renderer = SDL_CreateRenderer(window, -1, 0);

    connectbutton = IMG_Load("connectbutton.png");
    texture_connectbutton = SDL_CreateTextureFromSurface(renderer, connectbutton);
    replay = IMG_Load("replay.png");
    texture_replay = SDL_CreateTextureFromSurface(renderer, replay);
    affiche = IMG_Load("linq.png");
    texture_affiche = SDL_CreateTextureFromSurface(renderer, affiche);

    int i;
    //Import des images
    fond[0]=IMG_Load("vert1000x500.png");
    fond[1]=IMG_Load("rouge1000x500.png");
    fond[2]=IMG_Load("noir976x700.png");
    fond[3]=IMG_Load("jaune340x100.png");
    personne=IMG_Load("gris250x100.png");

    //Création des textures
    for (i=0;i<4;i++)
        texture_fond[i] = SDL_CreateTextureFromSurface(renderer, fond[i]);
    texture_personnes= SDL_CreateTextureFromSurface(renderer, personne);

    // Initialisation de variables

    strcpy(gNames[0],"-");
    strcpy(gNames[1],"-");
    strcpy(gNames[2],"-");
    strcpy(gNames[3],"-");
    strcpy(gNames[4],"-");
    tabChoix[0]=-1;
    tabChoix[1]=-1;

    goEnabled=0;
    connectEnabled=1;
    screenNumber=0;

    strcpy(word,"");
    cptWord=0;

    Sans60 = TTF_OpenFont("sans.ttf", 60);
    Sans30 = TTF_OpenFont("sans.ttf", 30);
    Sans20 = TTF_OpenFont("sans.ttf", 20); 

    /* Creation du thread serveur tcp. */
    printf ("Creation du thread serveur tcp !\n");
    synchro=0;
    ret = pthread_create ( & thread_serveur_tcp_id, NULL, fn_serveur_tcp, NULL);

    while (!quit)
    {
        if (SDL_PollEvent(&event))
        {
            manageEvent(event);
            manageNetwork();
            manageRedraw();
        }
    }
     
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
     
    SDL_Quit();
     
    return 0;
}
