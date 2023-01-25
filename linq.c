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
int cptWord;
int quit = 0;
SDL_Event event;
int mx,my;
char sendBuffer[1000];
SDL_Window * window;
SDL_Renderer *renderer;
 
SDL_Surface *connectbutton;
SDL_Texture *texture_connectbutton;
TTF_Font* Sans; 
int tour = 0;
int role;
char guessWord[40];
int choix=0;
int tabChoix[2];
int choix0=0;
int choix1=0;
int choix2=0;
int choix3=0;
int choix4=0;
char* mot;

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
            int car=event.key.keysym.sym;
            printf("%d\n",event.key.keysym.sym);
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
                    if ((mx<1024/2+100) && (my<768/2+25) &&(mx>1024/2-100) && (my>768/2-25) && (connectEnabled==1))
                    {
                        sprintf(sendBuffer,"C %s %d %s",
                        gClientIpAddress,gClientPort,gName);
                        sendMessageToServer(gServerIpAddress,
                        gServerPort,sendBuffer);
                        connectEnabled=0;
                    }
                case 4:
                    SDL_GetMouseState( &mx, &my );
                    if ((mx<1024/2+100) && (my<768/2+25) &&(mx>1024/2-100) && (my>768/2-25) && (choix0==0))
                    {
                        if(gId!=0){
                            tabChoix[choix]=0;
                            choix++;
                            choix0=1;
                            if(choix==2){
                                sprintf(sendBuffer,"C %d %d %d",gId,tabChoix[0],tabChoix[1]);
                                sendMessageToServer(gServerIpAddress, gServerPort,sendBuffer);
                            }
                        }
                    }
                    //sur les 5 noms
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
                        screenNumber = 3;
                }
                switch(gbuffer[0])
                {
                        case 'W':
                                sscanf(gbuffer+2,"%s %s %s %s %s %s %s %s %s %s %d",words[0],words[1],words[2],words[3],words[4],words[5],words[6],words[7],words[8],words[9], &tour);
                                if (strcmp(words[9],"-")!=0)
                                        screenNumber=4;
                                break;
                }
                break;
        case 3 :
                printf("\nA toi d'écrire : ");
                scanf("%[^\n]", mot);
                sprintf(sendBuffer,"M %d %s",gId,mot);
                sendMessageToServer(gServerIpAddress,gServerPort,sendBuffer);
                screenNumber=2;
                break;
        case 4 :
                quit=1;
                break;

        }
        synchro=0;
    }
}

void myRenderText(char *m,int x,int y)
{
    SDL_Color col1 = {0, 0, 0};
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, m, col1);
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
            SDL_Rect rect = {0, 0, 1024, 768};
            SDL_RenderFillRect(renderer, &rect);

            // si connectEnabled, alors afficher connect
            if (connectEnabled==1)
            {
                SDL_Rect dstrect = {1024/2-250/2, 768/2-65/2, 250, 65 };
                SDL_RenderCopy(renderer, texture_connectbutton, NULL, &dstrect);
            }

            if (cptWord>0)
            {
                myRenderText(word,105,350);
            }
            break;
        case 1:
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 230);
            SDL_Rect rect = {0, 0, 1024, 768};
            SDL_RenderFillRect(renderer, &rect);

            /*SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_Rect rect_j1;
            rect_j1.x = 400;//abscisse
            rect_j1.y = 350;//ordonnee
            rect_j1.w = 200;
            rect_j1.h = 200;
            SDL_RenderDrawRect(renderer, &rect_j1);
            //SDL_RenderDrawLine(renderer, rect_j1.x,rect_j1.y+rect_j1.h, rect_j1.x+rect_j1.w, J1_y+Height);*/

            myRenderText(gNames[0],105,50);
            myRenderText(gNames[1],105,110);
            myRenderText(gNames[2],105,160);
            myRenderText(gNames[3],105,210);
            myRenderText(gNames[4],105,260);
            break;
        }
        case 2:
        {
            // On efface l'écran
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 230);
            SDL_Rect rect = {0, 0, 1024, 768};
            SDL_RenderFillRect(renderer, &rect);

            //on définit l'interface graphique de la page de jeu
            //partie ajout des rectangles pour chaque joeur (les boites de mots)
            if(role)
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            else
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

            SDL_Rect rect_j1;
            rect_j1.x = Gauche_x;//abscisse
            rect_j1.y = J1_y;//ordonnee
            rect_j1.w = Width;
            rect_j1.h = Height;
            SDL_RenderDrawRect(renderer, &rect_j1);
            SDL_RenderDrawLine(renderer, Gauche_x,J1_y+Height/2, Gauche_x+Width, J1_y+Height/2);
            myRenderText(words[0],rect_j1.x+25,rect_j1.y+25);
            myRenderText(words[5],rect_j1.x+75,rect_j1.y+75);

            SDL_Rect rect_j2;
            rect_j2.x = Gauche_x;//abscisse
            rect_j2.y = J2_y;//ordonnee
            rect_j2.w = Width;
            rect_j2.h = Height;
            SDL_RenderDrawRect(renderer, &rect_j2);
            SDL_RenderDrawLine(renderer, Gauche_x,J2_y+Height/2, Gauche_x+Width, J2_y+Height/2);
            myRenderText(words[1],rect_j2.x+25,rect_j2.y+25);
            myRenderText(words[6],rect_j2.x+75,rect_j2.y+75);

            SDL_Rect rect_j3;
            rect_j3.x = Droite_x;//abscisse
            rect_j3.y = J3_y;//ordonnee
            rect_j3.w = Width;
            rect_j3.h = Height;
            SDL_RenderDrawRect(renderer, &rect_j3);
            SDL_RenderDrawLine(renderer, Droite_x,J3_y+Height/2, Droite_x+Width, J3_y+Height/2);
            myRenderText(words[2],rect_j3.x+25,rect_j3.y+25);
            myRenderText(words[7],rect_j3.x+75,rect_j3.y+75);

            SDL_Rect rect_j4;
            rect_j4.x = Droite_x;//abscisse
            rect_j4.y = J4_y;//ordonnee
            rect_j4.w = Width;
            rect_j4.h = Height;
            SDL_RenderDrawRect(renderer, &rect_j4);
            SDL_RenderDrawLine(renderer, Droite_x,J4_y+Height/2, Droite_x+Width, J4_y+Height/2);
            myRenderText(words[3],rect_j4.x+25,rect_j4.y+25);
            myRenderText(words[8],rect_j4.x+75,rect_j4.y+75);

            SDL_Rect rect_j5;
            rect_j5.x = Droite_x;//abscisse
            rect_j5.y = J5_y;//ordonnee
            rect_j5.w = Width;
            rect_j5.h = Height;
            SDL_RenderDrawRect(renderer, &rect_j5);
            SDL_RenderDrawLine(renderer, Droite_x,J5_y+Height/2, Droite_x+Width, J5_y+Height/2);
            myRenderText(words[4],rect_j5.x+25,rect_j5.y+25);
            myRenderText(words[9],rect_j5.x+75,rect_j5.y+75);

            //fin partie création rectangle
            //partie écriture des noms de personnages
            TTF_Font* Sans2 = TTF_OpenFont("mrsmonster.ttf", 42);
            char *role[]={"Espion", "Contre-espion"};
            SDL_Color col1 = {0, 0, 0};
            //écriture pour chaque joueur de son nom
            for(int i=0; i<5; i++)
            {
                SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans2, gNames[i], col1);
                SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
                SDL_Rect Message_rect;
                if (i < 2)
                {
                    
                    Message_rect.x = Gauche_x+40;
                    Message_rect.y = 230*i;
                    Message_rect.w = surfaceMessage->w;
                    Message_rect.h = surfaceMessage->h;
                }
                else
                {
                    Message_rect.x = Droite_x+40;
                    Message_rect.y = 195*(i-2);
                    Message_rect.w = surfaceMessage->w;
                    Message_rect.h = surfaceMessage->h;
                }

                SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
                SDL_DestroyTexture(Message);
                SDL_FreeSurface(surfaceMessage);
            }
            //fin partie message
        }
        case 3 :
        {
                // On efface l'écran
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 230);
            SDL_Rect rect = {0, 0, 1024, 768};
            SDL_RenderFillRect(renderer, &rect);

            //zone de saisie
            if(role)
                SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255);
            else
                SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);

            SDL_Rect rect_sai;
            rect_sai.x = 450;//abscisse
            rect_sai.y = 300;//ordonnee
            rect_sai.w = Width;
            rect_sai.h = Height;
            SDL_RenderDrawRect(renderer, &rect_sai);

            //on définit l'interface graphique de la page de jeu
            //partie ajout des rectangles pour chaque joeur (les boites de mots)
            SDL_Rect rect_j1;
            rect_j1.x = Gauche_x;//abscisse
            rect_j1.y = J1_y;//ordonnee
            rect_j1.w = Width;
            rect_j1.h = Height;
            SDL_RenderDrawRect(renderer, &rect_j1);
            SDL_RenderDrawLine(renderer, Gauche_x,J1_y+Height/2, Gauche_x+Width, J1_y+Height/2);
            myRenderText(words[0],rect_j1.x+25,rect_j1.y+25);
            myRenderText(words[5],rect_j1.x+75,rect_j1.y+75);

            SDL_Rect rect_j2;
            rect_j2.x = Gauche_x;//abscisse
            rect_j2.y = J2_y;//ordonnee
            rect_j2.w = Width;
            rect_j2.h = Height;
            SDL_RenderDrawRect(renderer, &rect_j2);
            SDL_RenderDrawLine(renderer, Gauche_x,J2_y+Height/2, Gauche_x+Width, J2_y+Height/2);
            myRenderText(words[1],rect_j2.x+25,rect_j2.y+25);
            myRenderText(words[6],rect_j2.x+75,rect_j2.y+75);

            SDL_Rect rect_j3;
            rect_j3.x = Droite_x;//abscisse
            rect_j3.y = J3_y;//ordonnee
            rect_j3.w = Width;
            rect_j3.h = Height;
            SDL_RenderDrawRect(renderer, &rect_j3);
            SDL_RenderDrawLine(renderer, Droite_x,J3_y+Height/2, Droite_x+Width, J3_y+Height/2);
            myRenderText(words[2],rect_j3.x+25,rect_j3.y+25);
            myRenderText(words[7],rect_j3.x+75,rect_j3.y+75);

            SDL_Rect rect_j4;
            rect_j4.x = Droite_x;//abscisse
            rect_j4.y = J4_y;//ordonnee
            rect_j4.w = Width;
            rect_j4.h = Height;
            SDL_RenderDrawRect(renderer, &rect_j4);
            SDL_RenderDrawLine(renderer, Droite_x,J4_y+Height/2, Droite_x+Width, J4_y+Height/2);
            myRenderText(words[3],rect_j4.x+25,rect_j4.y+25);
            myRenderText(words[8],rect_j4.x+75,rect_j4.y+75);

            SDL_Rect rect_j5;
            rect_j5.x = Droite_x;//abscisse
            rect_j5.y = J5_y;//ordonnee
            rect_j5.w = Width;
            rect_j5.h = Height;
            SDL_RenderDrawRect(renderer, &rect_j5);
            SDL_RenderDrawLine(renderer, Droite_x,J5_y+Height/2, Droite_x+Width, J5_y+Height/2);
            myRenderText(words[4],rect_j5.x+25,rect_j5.y+25);
            myRenderText(words[9],rect_j5.x+75,rect_j5.y+75);

            //fin partie création rectangle
            //partie écriture des noms de personnages
            TTF_Font* Sans2 = TTF_OpenFont("mrsmonster.ttf", 42);
            char *role[]={"Espion", "Contre-espion"};
            SDL_Color col1 = {0, 0, 0};
            //écriture pour chaque joueur de son nom
            for(int i=0; i<5; i++)
            {
                SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans2, gNames[i], col1);
                SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
                SDL_Rect Message_rect;
                if (i < 2)
                {
                    
                    Message_rect.x = Gauche_x+40;
                    Message_rect.y = 230*i;
                    Message_rect.w = surfaceMessage->w;
                    Message_rect.h = surfaceMessage->h;
                }
                else
                {
                    Message_rect.x = Droite_x+40;
                    Message_rect.y = 195*(i-2);
                    Message_rect.w = surfaceMessage->w;
                    Message_rect.h = surfaceMessage->h;
                }

                SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
                SDL_DestroyTexture(Message);
                SDL_FreeSurface(surfaceMessage);
            }
            //fin partie message
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
     
    window = SDL_CreateWindow("SDL2 LINQ", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, 0);
     
    renderer = SDL_CreateRenderer(window, -1, 0);

    connectbutton = IMG_Load("connectbutton.png");
    texture_connectbutton = SDL_CreateTextureFromSurface(renderer, connectbutton);

    // Initialisation de variables

    strcpy(gNames[0],"-");
    strcpy(gNames[1],"-");
    strcpy(gNames[2],"-");
    strcpy(gNames[3],"-");
    strcpy(gNames[4],"-");

    goEnabled=0;
    connectEnabled=1;
    screenNumber=0;

    strcpy(word,"");
    cptWord=0;

    Sans = TTF_OpenFont("sans.ttf", 60); 

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
