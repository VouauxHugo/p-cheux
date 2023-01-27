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
char roleaff[100];
int cptWord;
int quit = 0;
SDL_Event event;
int mx,my;
char sendBuffer[1000];
SDL_Window * window;
SDL_Renderer *renderer;
 
SDL_Surface *connectbutton;
SDL_Surface *affiche;
SDL_Texture *texture_connectbutton;
SDL_Texture *texture_affiche;
SDL_Surface *fond[3],*personne[5];
SDL_Texture *texture_fond[3],*texture_personnes[5];
TTF_Font* Sans60; 
TTF_Font* Sans20; 
TTF_Font* Sans30; 
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
                    if ((mx<800) && (my<450) &&(mx>400) && (my>50) && (connectEnabled==1))
                    {
                        sprintf(sendBuffer,"C %s %d %s",
                        gClientIpAddress,gClientPort,gName);
                        sendMessageToServer(gServerIpAddress,
                        gServerPort,sendBuffer);
                        connectEnabled=0;
                    }
                case 4:
                    SDL_GetMouseState( &mx, &my );
                    if ((mx<1000/2+100) && (my<500/2+25) &&(mx>1000/2-100) && (my>500/2-25) && (choix0==0))
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
                        printf("\nA toi d'écrire : ");
                        scanf("%[^\n]", mot);
                        sprintf(sendBuffer,"M %d %s",gId,mot);
                        sendMessageToServer(gServerIpAddress,gServerPort,sendBuffer);
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
                quit=1;
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

            SDL_Rect pers1 = { 100, 350, 200, 100 };
            SDL_RenderCopy(renderer, texture_personnes[1], NULL, &pers1);
            SDL_Rect pers2 = { 100, 200, 200, 100 };
            SDL_RenderCopy(renderer, texture_personnes[1], NULL, &pers2);
            SDL_Rect pers3 = { 400, 50, 200, 100 };
            SDL_RenderCopy(renderer, texture_personnes[1], NULL, &pers3);
            SDL_Rect pers4 = { 700, 200, 200, 100 };
            SDL_RenderCopy(renderer, texture_personnes[1], NULL, &pers4);
            SDL_Rect pers5 = { 700, 350, 200, 100 };
            SDL_RenderCopy(renderer, texture_personnes[1], NULL, &pers5);

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
        }
        case 3 :
        {
            break;
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
     
    window = SDL_CreateWindow("SDL2 LINQ", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 500, 0);
     
    renderer = SDL_CreateRenderer(window, -1, 0);

    connectbutton = IMG_Load("connectbutton.png");
    texture_connectbutton = SDL_CreateTextureFromSurface(renderer, connectbutton);
    affiche = IMG_Load("linq.png");
    texture_affiche = SDL_CreateTextureFromSurface(renderer, affiche);

    int i;
    //Import des images
    fond[0]=IMG_Load("vert1000x500.png");
    fond[1]=IMG_Load("rouge1000x500.png");
    fond[2]=IMG_Load("noir976x700.png");
    for(i=0; i<5; i++)
        personne[i]=IMG_Load("gris250x100.png");

    //Création des textures
    for (i=0;i<3;i++)
        texture_fond[i] = SDL_CreateTextureFromSurface(renderer, fond[i]);
    for (i=0;i<5;i++)
        texture_personnes[i] = SDL_CreateTextureFromSurface(renderer, personne[i]);

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
