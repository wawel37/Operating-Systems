#include "lib.h"

int gameBoard[3][3];
int place=0;
char name[32];
int myCharacter;
int oppositeCharacter;
int serverdesc;
int epoldesc;
char path[108];


int setSocketAsNetwork(char *arg){
    int portno;
    sscanf(arg, "%d", &portno);
    struct sockaddr_in netaddr;
    netaddr.sin_family = AF_INET;
    netaddr.sin_port = htons(portno);
    struct in_addr inaddr;
    inet_pton(AF_INET,"127.0.0.1",&inaddr);
    netaddr.sin_addr = inaddr;

    
    int netdesc = socket(AF_INET, SOCK_STREAM,0);
    if(netdesc ==-1){
        perror("client: make socket inet");
    }
    
    int connect_status = connect(netdesc, (struct sockaddr *)&netaddr, sizeof(netaddr));
    if(connect_status == -1){
        perror("client: connect network error");
        exit(-1);
    }

    return netdesc;
}

int setSocketAsLocal(char *arg){
    strcpy(path,arg);

    struct sockaddr_un unixaddr;
    unixaddr.sun_family=AF_UNIX;
    strcpy(unixaddr.sun_path,path);

    int undesc = socket(AF_UNIX, SOCK_STREAM,0);
    if(undesc==-1){
        perror("client: make socket unix");
    }
    int connect_status = connect(undesc, (struct sockaddr *)&unixaddr, sizeof(unixaddr));
    if(connect_status == -1){
        perror("client: connect local error");
        exit(-1);
    }
    
    return undesc;
}



void printGameBoard(){
    for(int i=0;i<3;i++){
        char line[7];
        int p=0;
        for(int j=0;j<3;j++){
            int sign = gameBoard[i][j];

            if(sign==FREE){
                line[p]='_';
            }
            else if(sign==X_CHARACTER){
                line[p]='X';
            }
            else {
                line[p]='O';
            }
            line[++p]=' ';
            p++;
        }
        printf("%s\n",line);
    }
}


int checkWinner(){
    
    for(int i=0;i<3;i++){
        int suma=gameBoard[0][i]+gameBoard[1][i]+gameBoard[2][i];
        int suma2=gameBoard[i][0]+gameBoard[i][1]+gameBoard[i][2];
        if(suma==3*myCharacter || suma2==3*myCharacter){
            return WINNER;
        }
    }
    if(gameBoard[0][0]+gameBoard[1][1]+gameBoard[2][2]==3*myCharacter){
        return WINNER;
    }
    if(gameBoard[0][2]+gameBoard[1][1]+gameBoard[2][0]==3*myCharacter){
        return WINNER;
    }
    
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            if(gameBoard[i][j]==FREE){
                return NOT_WINNER;
            }
        }
    }

    return DRAW; 
}

void disconnect(){
    printf("disconnect with server\n");
    
    if(shutdown(serverdesc, SHUT_RDWR)==-1){
        perror("server: shutdown socket error");
    }
    if(close(serverdesc)==-1){
        perror("server: close socket error");
    }
    exit(0);
}

void freeGameBoard(){
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++)
            gameBoard[i][j]=FREE;
    }
}

int saveMove(int cell, int sign){
    if(cell==ERROR){
        return 0;
    }
    int col=-1,row=0;

    while(cell>0){
        col++;
        cell--;

        if(col==3){
            col=0;
            row++;
        }
    }
    
    if(gameBoard[row][col]!=FREE){
        return ERROR;
    }
    gameBoard[row][col]=sign;
    place++;
    return 0;
}


void ping(){
    struct message msg4;
    msg4.type=PING;
    strcpy(msg4.name,name);
    write(serverdesc,&msg4,sizeof(struct message));
}

void disconnectWrapper(){
    struct message msg3;
    msg3.type=DISCONNECT;
    strcpy(msg3.name,name);
    write(serverdesc,&msg3,sizeof(struct message));
    disconnect();
}

void makeMove(int serverdesc){
    printGameBoard();
    printf("Your move. Choose number from 1 to 9.\n");

    int move;
    struct message msg,received;
    struct epoll_event event;


    while(1){
        epoll_wait(epoldesc,&event,1,-1);

        if(event.data.fd==serverdesc){

            read(serverdesc,&received,sizeof(struct message));
            if(received.type==PING){
               ping();
            }
            else if(received.type==DISCONNECT){
                disconnectWrapper();
            }
            else if(received.type==CONNECT && received.msg==WAITING_FOR_PLAYER){
                if(received.other==ERROR){
                    printf("Second player has left the game.\n");
                }
                printf("Waiting for other player...\n");
                return;
            }
        }
        if(event.data.fd==STDIN_FILENO){
            break;
        }
    }


    scanf("%d",&move);
    

    if(move>9 || move <1 || saveMove(move,myCharacter) == ERROR){
        printf("Wrong cell number. You have lost your turn\n");
        move = ERROR;
    }
    printGameBoard();
    place++;

    strcpy(msg.name,name);
    msg.type=MOVE;
    msg.msg=move;

    int result = checkWinner();
    if(result==WINNER){
        printf("CONGRATULATIONS! YOU WON!\n");
        msg.other=WINNER;
        write(serverdesc,&msg,sizeof(struct message));

        msg.type=DISCONNECT;
        write(serverdesc,&msg,sizeof(struct message));

        disconnect();
    }
    else if(result==DRAW){
        printf("CONGRATULATIONS! DRAW!\n");
        msg.other=DRAW;
        write(serverdesc,&msg,sizeof(struct message));

        msg.type=DISCONNECT;
        write(serverdesc,&msg,sizeof(struct message));

        disconnect();
    }
    else{
         write(serverdesc,&msg,sizeof(struct message));     
    }
}


void sigHandler(int signum){
    struct message msg;
    strcpy(msg.name,name);
    msg.type=DISCONNECT;
    msg.other=CTRLC;
    write(serverdesc,&msg,sizeof(struct message));

    disconnect();
}



int main(int argc, char ** argv){
    if(argc<4){
        printf("Client: wrong number of arguments!\n");
        return 1;
    }

    signal(SIGINT,sigHandler);

    strcpy(name,argv[1]);

    if(strcmp(argv[2],"network")==0){
        serverdesc = setSocketAsNetwork(argv[3]);
    }
    else if(strcmp(argv[2], "local")==0){
        serverdesc = setSocketAsLocal(argv[3]);
    }
    else{
        perror("Client: wrong connection type");
        return 1;
    }

    epoldesc = epoll_create1(0);
    if(epoldesc==-1){
        perror("server: epol create error");
    }

    struct epoll_event epoll_ev,epoll_ev2;
    epoll_ev.events=EPOLLIN ;
    epoll_ev2.events=EPOLLIN ;
    union epoll_data epoll_da,epoll_da2;
    epoll_da.fd=serverdesc;
    epoll_ev.data=epoll_da;
    
    if(epoll_ctl(epoldesc,EPOLL_CTL_ADD,serverdesc,&epoll_ev)==-1){
        perror("server: epoll unix ctl error");
    }

    epoll_da2.fd=STDIN_FILENO;
    epoll_ev2.data=epoll_da2;
    if(epoll_ctl(epoldesc,EPOLL_CTL_ADD,STDIN_FILENO,&epoll_ev2)==-1){
        perror("server: epoll net ctl error");
    }

    struct message received;
    received.type=-1;

    while(1){
        int rcv_status = read(serverdesc,&received,sizeof(struct message));

        if(rcv_status==0){
            disconnect();
        }

        if(received.type==GIVE_NAME){
            if(received.msg==ERROR){
                printf("It's impossible to add another player, server error.\n");
                exit(-1);
            }
            struct message msg;
            strcpy(msg.name,argv[1]);
            msg.type=CONNECT;

            int send_status = write(serverdesc,&msg,sizeof(struct message));
            if(send_status==-1){
                perror("client: send name error");
            }
            received.type=-1;
        }
        else if(received.type==CONNECT){

            if(rcv_status==-1){
                perror("client: receive msg error");
            }
            else{

                if(received.msg==ERROR){
                    printf("Player with this name is already connected.\n");
                    exit(-1);
                }
                else if(received.msg==WAITING_FOR_PLAYER){
                    if(received.other==ERROR){
                        printf("Second player has left the game.\n");
                    }
                    printf("Waiting for other player...\n");
                }
                else{
                    myCharacter=received.msg;
                    
                    char s;
                    if(myCharacter==O_CHARACTER){
                        s='O';
                        oppositeCharacter=X_CHARACTER;
                    }
                    else{
                        s='X';
                        oppositeCharacter=O_CHARACTER;
                    }

                    printf("Connected with player %s. Your sign is %c.\n",received.name,s);
                    freeGameBoard();
                    if(received.other==myCharacter){
                        printf("You have to start.\n");
                        makeMove(serverdesc);
                    }
                    
                }
                received.type=-1;
            }
        }
        else if(received.type==MOVE){

            saveMove(received.msg,oppositeCharacter);
            printf("After second player move:\n");

            if(received.other==WINNER){
                printGameBoard();
                printf("YOU LOST.\n");
                struct message msg;
                strcpy(msg.name,name);
                msg.type=DISCONNECT;
                write(serverdesc,&msg,sizeof(struct message));
                disconnect();
            }
            else if(received.other==DRAW){
                printGameBoard();
                printf("IT'S A DRAW!\n");
                struct message msg;
                strcpy(msg.name,name);
                msg.type=DISCONNECT;
                write(serverdesc,&msg,sizeof(struct message));
                disconnect();
            }
            else         
                makeMove(serverdesc);
        }
        else if(received.type==DISCONNECT){
            disconnectWrapper();
        }
        else if(received.type==PING){
            ping();
        }
    }
}   
