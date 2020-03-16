#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h> 
#include <stdlib.h>
#include <sys/wait.h>
#include <dirent.h>
#include <pwd.h>
/* portul folosit */
#define PORT 2024

int client[1000];
int match_counter = 0;
int play_ready[1000];
int client_counter = 0;
int player_counter = 0;
int accounts[1000];
int pip[1000][2];
int pip2[1000][2];


int login(char s[], char s1[]){
	int log = 0;
	FILE * fd;
	FILE * fd2;	
	fd = fopen("user_accounts", "r");
	fd2 = fopen("user_passwords", "r");
	char s2[100], s3[100];
	int acc_number = 0;
	while( fgets(s2, 100, fd) != NULL ){
		int count = 0;
		for(int i = 0; i<strlen(s2) ; ++i){
	   		 if( s2[i] != ' ' && s2[i] != '\n'){
				s2[count] = s2[i];
				++count;
	   		 }
		}
		s2[count] = '\0';
		++acc_number; 
		if( strcmp(s, s2) == 0 ){
			int count3 = 0; 
			int count_passwords = 0;
			while( count_passwords < acc_number ){
				fgets(s3, 100, fd2);
				++count_passwords;
			}
			for( int j = 0; j < strlen(s3); ++j ){
				if( s3[j] != ' ' && s3[j] != '\n'){
					s3[count3] = s3[j];
					++count3;
				}
			}
			s3[count3] = '\0';
			if( strcmp( s1, s3 ) == 0 ){	
				if( accounts[acc_number] == 0 ){		
					log = 0;
					accounts[acc_number] = 1;
			    	    	break;
				}
				else{
					++log;
				}
			}
			else
				++log;
		}
		else{
		    	++log;
		}
	}
	if( log == 0 ){
		return acc_number;
	}
	else{
		return 0;
	}
}

void handle_rank( char name[] ){
	FILE *fd;
	fd = fopen("rankings", "r");
	int rankings[10], rank_value[10];
	for( int i = 0; i < 10; ++i){
		rankings[i] = -1;
		rank_value[i] = -1;
	}
	int check_rank, counter = 0;
	while(  fscanf( fd, "%d", &check_rank) && !feof(fd) ){
		for( int i = 0; i < 10; ++i ){
			if( rank_value[i] < check_rank ){
				for( int j = 9; j > i; --j ){
					rankings[j] = rankings[j-1];
					rank_value[j] = rank_value[j-1];
					
				}
				rankings[i] = counter;
				rank_value[i] = check_rank;	
				break;
			}
		}
		++counter;

	}
	fclose(fd);
	name[0] = '\0';
	for( int j = 0; j < 10; ++j ){
		FILE *acc;
		acc = fopen("user_accounts", "r");
		char  holder[20];
		int count;
		counter = 0;
		while( fgets(holder, 100, acc) != NULL ){
			//for( int i = 0; i < 10; ++i ){
				if( counter == rankings[j] ){
					count = 0;						
					for(int i = 0; i<strlen(holder) ; ++i){
				   		 if( holder[i] != ' ' && holder[i] != '\n'){
							holder[count] = holder[i];
							++count;
				   		 }
					}
					holder[count] = '\0';
					if( j == 0 ){
						strcpy(name, holder);
					}
					else{
						strcat(name, "\n");
						strcat(name, holder);
					}	
				}
			//}
			++counter;
		}
		fclose(acc);
	}
	name[strlen(name)] = '\0';
}


void update_rank(int acc_nr){
	FILE *fd;
	fd = fopen("rankings", "r");
	int rank_hold[100], i = 0, count_acc = 0;
	while( fscanf( fd, "%d", &rank_hold[i]) && !feof(fd) ){
		if( count_acc == acc_nr ){	
			++rank_hold[i];
							
		}
		++i;	
		++count_acc;
	}
	fclose(fd);
	fd = fopen("rankings", "w");
	for(int j = 0; j <= i; ++j ){
		fprintf(fd, "%d\n", rank_hold[j]);
	}
	fclose(fd);
	
}




/* codul de eroare returnat de anumite apeluri */
extern int errno;

void handle_match(int player1, int player2);

int main ()
{

for( int i = 0; i < 1000; ++i){
	play_ready[i] = 0;
	accounts[i] = 0;
}
  struct sockaddr_in server;	// structura folosita de server
  struct sockaddr_in from;	
  //char msg[100];		//mesajul primit de la client 
  //char msgrasp[100]=" ";        //mesaj de raspuns pentru client
  int sd;			//descriptorul de socket 
  /* crearea unui socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      return errno;
    }

  /* pregatirea structurilor de date */
  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));
  
  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;	
  /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
  /* utilizam un port utilizator */
    server.sin_port = htons (PORT);
  
  /* atasam socketul */
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      return errno;
    }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 5) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    }



  while (1)
    {
	socklen_t length = sizeof (from);
	
	printf ("[server]Asteptam la portul %d...\n",PORT);
	fflush (stdout);
	fprintf(stdout,"client_counter is %d\n",client_counter);

	/* acceptam un client (stare blocanta pina la realizarea conexiunii) */
	client[client_counter] = accept (sd, (struct sockaddr *) &from, &length);



	if( pipe(pip[client_counter]) ==-1){
	    fprintf(stderr,"pip\n");
	    exit(1);
   	 }
	if( pipe(pip2[client_counter]) ==-1){
	    fprintf(stderr,"pip\n");
	    exit(1);
   	 }

  	 if (fcntl(pip[client_counter][0], F_SETFL,  fcntl(pip[client_counter][0], F_GETFL) | O_NONBLOCK) < 0) {
	    fprintf(stderr,"set unblocking\n");
	    exit(1);
   	 }

	switch( fork() ){
		case -1:
			fprintf(stderr,"fork -1\n");
			exit(1);
		case 0:
			close(pip[client_counter][0]);
			close(pip2[client_counter][1]);
			fprintf(stdout,"i get here swt\n");
			char comm[100];
			int leng;
			sleep(5);
			//login part here			
			char ussname[20], pass[20];
			int ussnamel, passl;
			int lgkey = 0;
			while( lgkey == 0 ){
				lgkey = 0;
				read( client[client_counter], &ussnamel, sizeof(int) );
				ussnamel = htonl(ussnamel);
				read( client[client_counter], &ussname, sizeof(char) * ussnamel );
				read( client[client_counter], &passl, sizeof(int) );
				passl = htonl(passl);
				read( client[client_counter], &pass, sizeof(char) * passl );
				ussname[ussnamel] = '\0';
				lgkey = login(ussname, pass);
				lgkey = ntohl(lgkey);
				write( client[client_counter], &lgkey, sizeof(int) );
				lgkey = htonl(lgkey);

			}


			while( 1 ){

				read(client[client_counter], &leng, sizeof(int));
				leng = htonl(leng);
				if (read (client[client_counter], &comm, sizeof(char)*leng) <= 0)
				{
					perror ("[server]Eroare la read() de la client.\n");
					fprintf(stdout,"what was read was %s\n",comm);
					close (client[client_counter]);	
					continue;		
				}
				//command for play
				if( strncmp( comm, "find match", 10) == 0 ){
					char asdd[20] = "fm";
					int asdln = strlen(asdd);
					if( write(pip[client_counter][1], &asdln, sizeof(int)) < 0 ){
						//printf("----wrote nothing\n");
						perror("Error was");
						sleep(1);
					}
					else
					if( write(pip[client_counter][1], &asdd, sizeof(char)*asdln) < 0 ){
						printf("----wrote nothing\n");
						perror("Error was");
						sleep(1);
					}
					else
						fprintf(stdout,"what is happening?\n");
					int opponent;
					read(pip2[client_counter][0], &opponent, sizeof(int));
					if( opponent != -1 ){
						handle_match(client_counter, opponent); // function that will handle clients during matches
						write(pip[client_counter][1], "mf", sizeof(char) * 2 );
						write(pip[client_counter][1], &client_counter, sizeof(int));
						write(pip[client_counter][1], &opponent, sizeof(int));
						update_rank(lgkey);
					}
					else{
						char waiting[10] = "wait";
						while( strncmp( waiting, "wait", 4) == 0 ){ 
							write(pip[client_counter][1], "wt", sizeof(char) * 2 );
							read(pip2[client_counter][0], &waiting, sizeof(char) * 4 ); 
						}
					} 


				}
				if( strncmp( comm, "get rank", 8) == 0 ){
					char ranks[200];
					handle_rank(ranks); // function that will provide rankings
					int rnln = strlen(ranks);
					rnln = ntohl(rnln);
					write( client[client_counter], &rnln, sizeof(int) );
					write( client[client_counter], &ranks, sizeof(char) * strlen(ranks) );
					
				}
				// help and quit do not require information from the server
				//close(pip[client_counter][0]);
			}//while reapeat commands
			exit(1);
	} 			/* switch */
	close(pip[client_counter][1]);
	close(pip2[client_counter][0]);
	for( int i = 0; i <= client_counter; ++i){
		sleep(5);
		int commsk[100];
		int nread;
		int lncms;
		read(pip[i][0], &lncms, sizeof(int));
		nread = read(pip[i][0], &commsk, sizeof(char)*lncms);
		if(nread>0){
				if( strncmp( commsk, "fm", 2) == 0 ){
					++player_counter;
					play_ready[i] = 1;
					if( player_counter % 2 == 1 ){
						int minusone = -1;
						write(pip2[i][1], &minusone, sizeof(int));
					}
					else{
						for( int j = 0; j <= client_counter; ++j){
							if( play_ready[j] && j != i ){
								write( pip2[i][1], &j, sizeof(int));
								play_ready[i] = -1;
								play_ready[j] = -1;
								player_counter = player_counter - 2;
								break;
							}							
						}
					}
				}
				if( strncmp( commsk, "mf", 2) == 0 ){
					player_counter -= 2;
					int pl1, pl2;
					read(pip[i][0], &pl1, sizeof(int));
					read(pip[i][0], &pl2, sizeof(int));
					play_ready[pl1] = 0;
					play_ready[pl2] = 0;
				}
				if( strncmp( commsk, "wt", 2) == 0 ){
					if( play_ready[i] == -1 || play_ready[i] == 1 )
						write(pip2[i][1], "wait", sizeof(char) * 4 );
					else
						write(pip2[i][1], "done", sizeof(char) * 4 );
						
				}
					
					
		}
		else{
		        printf("----Read nothing\n");
		        //perror("Error was");
		        sleep(1);
            }
		

	}
		

	
	

    

    ++client_counter;



    }				/* while */
}				/* main */




int check_pos(int board[8][8], int x, int y, int player){
	if( player == 1 ){
		if( board[x-1][y] == 2 ){
			for( int i = x - 2; i > -1; --i){
				if( board[i][y] == 1 )
					return 1;
				if( board[i][y] == 0 )
					break;
			}	
		}
		if( board[x-1][y-1] == 2 ){
			int j = y - 2;
			for( int i = x - 2; i > -1 && j > -1; --i){
				if( board[i][j] == 1 )
					return 1;
				if( board[i][j] == 0 )
					break;
				--j;
			}	
		}
		if( board[x-1][y+1] == 2 ){
			int j = y + 2;
			for( int i = x - 2; i > -1 && j < 8; --i){
				if( board[i][j] == 1 )
					return 1;
				if( board[i][j] == 0 )
					break;
				++j;
			} 
		}
		if( board[x][y-1] == 2 ){
			for( int i = y - 2; i > -1; --i ){
				if( board[x][i] == 1 )
					return 1;
				if( board[x][i] == 0 )
					break;
			}	
		}
		if( board[x][y+1] == 2 ){
			for( int i = y + 2; i < 8; ++i ){
				if( board[x][i] == 1 )
					return 1;
				if( board[x][i] == 0)
					break;
			}
		}
		if( board[x+1][y] == 2 ){
			for( int i = x + 2; i < 8; ++i ){
				if( board[i][y] == 1 )
					return 1;
				if( board[i][y] == 0 )
					break;
			}
		}
		if( board[x+1][y-1] == 2 ){
			int j = y - 2;
			for( int i = x + 2; i < 8 && j > - 1; ++i){
				if( board[i][j] == 1 )
					return 1;
				if( board[i][j] == 0 )
					break;
				--j;
			}
		}
		if( board[x+1][y+1] == 2 ){
			int j = y + 2;
			for( int i = x + 2; i < 8 && j < 8; ++i){
				if( board[i][j] == 1 )
					return 1;
				if( board[i][j] == 0 )
					break;	
				++j;
			}
		}
		return 0;
	}


	if( player == 2 ){
		if( board[x-1][y] == 1 ){
			for( int i = x - 2; i > -1; --i){
				if( board[i][y] == 2 )
					return 1;
				if( board[i][y] == 0 )
					break;
			}	
		}
		if( board[x-1][y-1] == 1 ){
			int j = y - 2;
			for( int i = x - 2; i > -1 && j > -1; --i){
				if( board[i][j] == 2 )
					return 1;
				if( board[i][j] == 0 )
					break;
				--j;
			}	
		}
		if( board[x-1][y+1] == 1 ){
			int j = y + 2;
			for( int i = x + 2; i > -1 && j < 8; --i){
				if( board[i][j] == 2 )
					return 1;
				if( board[i][j] == 0 )
					break;
				++j;
			} 
		}
		if( board[x][y-1] == 1 ){
			for( int i = y - 2; i > -1; --i ){
				if( board[x][i] == 2 )
					return 1;
				if( board[x][i] == 0 )
					break;
			}	
		}
		if( board[x][y+1] == 1 ){
			for( int i = y + 2; i < 8; ++i ){
				if( board[x][i] == 2 )
					return 1;
				if( board[x][i] == 0)
					break;
			}
		}
		if( board[x+1][y] == 1 ){
			for( int i = x + 2; i < 8; ++i ){
				if( board[i][y] == 2 )
					return 1;
				if( board[i][y] == 0 )
					break;
			}
		}
		if( board[x+1][y-1] == 1 ){
			int j = y - 2;
			for( int i = x + 2; i < 8 && j > - 1; ++i){
				if( board[i][j] == 2 )
					return 1;
				if( board[i][j] == 0 )
					break;
			--j;
			}
		}
		if( board[x+1][y+1] == 1 ){
			int j = y + 2;
			for( int i = x + 2; i < 8 && j < 8; ++i){
				if( board[i][j] == 2 )
					return 1;
				if( board[i][j] == 0 )
					break;	
			++j;
			}
		}
		return 0;
	}
	return 0;
}


void turn_around(int board[8][8], int x, int y);
void handle_match( int player1, int player2 ){
	int turn = 1;
	write(client[player1], "match start", sizeof(char) * 11);
	turn = ntohl(turn);
	write(client[player1], &turn, sizeof(int));
	write(client[player2], "match start", sizeof(char) * 11);
	turn = 2;
	turn = htonl(turn);
	write(client[player2], &turn, sizeof(int));
	int board[8][8];
	for( int i = 0; i < 8; ++i){
		for(int j = 0; j < 8; ++j){
			board[i][j] = 0;
		}
	}
	board[3][3] = 1, board[3][4] = 2, board[4][3] = 2, board[4][4] = 1;
	int piece_count = 30;
	int p1x, p1y, p2x, p2y, chk = 0;
	char ready_chk[10];
	int count_p1p = 0, count_p2p = 0;
	while( piece_count > 0){
		chk = 0;
		while(chk == 0 ){ 
			write(client[player1], "your turn", sizeof(char) * 9);
			read(client[player1], &ready_chk, sizeof(char) * 5);
			if( strncmp( ready_chk, "withd", 5) == 0 ){
				count_p1p = 0, count_p2p = 64;
				int m1 = -5;
				m1 = ntohl(m1);
				write(client[player2], &m1, sizeof(int) );
				goto end_game;
			}
			if( strncmp( ready_chk, "ready", 5) == 0 ){
				read(client[player1], &p1x, sizeof(int));
				read(client[player1], &p1y, sizeof(int));
				p1x = htonl(p1x), p1y = htonl(p1y);
				if( board[p1x][p1y] == 0 ){
					if( board[p1x-1][p1y] == 2 || board[p1x+1][p1y] == 2 || board[p1x][p1y+1] == 2 || board[p1x][p1y-1] == 2 ){
						if( check_pos( board, p1x, p1y, 1) ){
							board[p1x][p1y] = 1;
							turn_around(board, p1x, p1y);
							chk = 1;
						}
						else
							chk = 0;
					}
					else 
						chk = 0;
				}
				else
					chk = 0;
				chk = ntohl(chk);
				write(client[player1], &chk, sizeof(int));
				chk = htonl(chk);
			}
		}
		chk = ntohl(chk);
		write(client[player2], &chk, sizeof(int));
		p1x = ntohl(p1x); p1y = ntohl(p1y);
		write(client[player2], &p1x, sizeof(int));
		write(client[player2], &p1y, sizeof(int));
		//check placement
		chk = 0;
		while( chk == 0 ){
			write(client[player2], "your turn", sizeof(char) * 9);
			read(client[player2], &ready_chk, sizeof(char) * 5);
			if( strncmp( ready_chk, "withd", 5) == 0 ){
				count_p1p = 64, count_p2p = 0;
				int m1 = -5;
				m1 = ntohl(m1);
				write(client[player1], &m1, sizeof(int) );
				goto end_game;
			}
			if( strncmp( ready_chk, "ready", 5) == 0 ){
				read(client[player2], &p2x, sizeof(int));
				read(client[player2], &p2y, sizeof(int));
				p2x = htonl(p2x), p2y = htonl(p2y);
				if( board[p2x][p2y] == 0 ){
					if( board[p2x-1][p2y] == 1 || board[p2x+1][p2y] == 1 || board[p2x][p2y+1] == 1 || board[p2x][p2y-1] == 1 ){
						if( check_pos( board, p2x, p2y, 2) ){
							board[p2x][p2y] = 2;
							turn_around(board, p2x, p2y);
							chk = 1;
						}
						else
							chk = 0;
					}
					else 
						chk = 0;
				}
				else
					chk = 0;
				chk = ntohl(chk);
				write(client[player2], &chk, sizeof(int));
				chk = htonl(chk);
			}
		}
		p2x = ntohl(p2x); p2y = ntohl(p2y);
		write(client[player1], &p2x, sizeof(int));
		write(client[player1], &p2y, sizeof(int));
		//check placement
				for( int i = 0; i < 8; ++i ){
					for( int j = 0; j < 8; ++j){
						fprintf(stdout, "%d   ",board[i][j]);
					}
					fprintf(stdout,"\n\n");
				}
		--piece_count;		
	}	
	for( int i = 0; i < 8; ++i ){
		for( int j = 0; j < 8; ++j ){
			if( board[i][j] == 1 )
				++count_p1p;
			if( board[i][j] == 2 )
				++count_p2p;
		}
	}

	end_game:

	if( count_p1p > count_p2p ){
		write( client[player1], "win", sizeof(char) * 3 );
		write( client[player2], "los", sizeof(char) * 3 );
	}
	else if( count_p1p < count_p2p ){
		write( client[player1], "los", sizeof(char) * 3 );
		write( client[player2], "win", sizeof(char) * 3 );
	}
	else if( count_p1p == count_p2p ){
		write( client[player1], "drw", sizeof(char) * 3 );
		write( client[player2], "drw", sizeof(char) * 3 );
	}


}


void turn_around(int board[8][8], int x, int y){
	if( board[x][y] == 1 ){
		if( board[x-1][y] == 2 ){
			int dnd = 0;
			int l1 = -1;
			for( int i = x - 2; i > l1; --i){
				if( board[i][y] == 1 && dnd == 0){
					dnd = 1;
					l1 = i - 1;
					i = x;
				}
				if( dnd == 1 )
					board[i][y] = 1;
					
			}	
		}
		if( board[x-1][y-1] == 2 ){
			int dnd = 0;
			int l1 = -1, l2 = -1;
			for( int j = y - 2, i = x - 2; i > l1 && j > l2; --i, --j){
				if( board[i][j] == 1 && dnd == 0){
					dnd = 1;
					l1 = i - 1, l2 = j - 1;
					i = x, j = y;
				}
				if( dnd == 1 )
					board[i][j] = 1;
			}	
		}
		if( board[x-1][y+1] == 2 ){
			int dnd = 0;
			int l1 = -1, l2 = 8;
			for( int j = y + 2, i = x - 2; i > l1 && j < l2; --i, ++j){
				if( board[i][j] == 1 && dnd == 0){
					dnd = 1;
					l1 = i - 1, l2 = j + 1;
					i = x, j = y;
				}
				if( dnd == 1 )
					board[i][j] = 1;
			} 
		}
		if( board[x][y-1] == 2 ){
			int dnd = 0;
			int l1 = -1;
			for( int i = y - 2; i > l1; --i ){
				if( board[x][i] == 1 && dnd == 0 ){
					dnd = 1;
					l1 = i - 1;
					i = y;
				}
				if( dnd == 1 )
					board[x][i] = 1;
			}	
		}
		if( board[x][y+1] == 2 ){
			int dnd = 0;
			int l1 = 8;
			for( int i = y + 2; i < l1; ++i ){
				if( board[x][i] == 1 && dnd == 0 ){
					dnd = 1;
					l1 = i + 1;
					i = y;
				}
				if( dnd == 1)
					board[x][i] = 1;
			}
		}
		if( board[x+1][y] == 2 ){
			int dnd = 0;
			int l1 = 8;
			for( int i = x + 2; i < l1; ++i ){
				if( board[i][y] == 1 && dnd == 0 ){
					dnd = 1;
					l1 = i + 1;
					i = x;
				}
				if( dnd == 1 )
					board[i][y] = 1;
			}
		}
		if( board[x+1][y-1] == 2 ){
			int dnd = 0;
			int l1 = 8, l2 = -1;
			for( int j = y - 2, i = x + 2; i < l1 && j > l2; ++i, --j){
				if( board[i][j] == 1 && dnd == 0 ){
					dnd = 1;
					l1 = i + 1, l2 = j - 1;
					i = x, j = y;
				}
				if( dnd == 1 )
					board[i][j] = 1;
			}
		}
		if( board[x+1][y+1] == 2 ){
			int dnd = 0;
			int l1 = 8, l2 = 8;
			for( int j = y + 2, i = x + 2; i < l1 && j < l2; ++i, ++j ){
				if( board[i][j] == 1 && dnd == 0 ){
					dnd = 1;
					l1 = i + 1, l2 = j + 1;
					i = x, j = y;
				}
				if( dnd == 1 )
					board[i][j] = 1;
			}
		}
	}


	if( board[x][y] == 2){
		if( board[x-1][y] == 1 ){
			int dnd = 0;
			int l1 = -1;
			for( int i = x - 2; i > l1; --i ){
				if( board[i][y] == 2 && dnd == 0 ){
					dnd = 1;
					l1 = i - 1;
					i = x;
				}
				if( dnd == 1 )
					board[i][y] = 2;
					
			}	
		}
		if( board[x-1][y-1] == 1 ){
			int dnd = 0;
			int l1 = -1, l2 = -1;
			for( int j = y - 2, i = x - 2; i > l1 && j > l2; --i, --j ){
				if( board[i][j] == 2 && dnd == 0){
					dnd = 1;
					l1 = i - 1, l2 = j - 1;
					i = x, j = y;
				}
				if( dnd == 1 )
					board[i][j] = 2;
			}	
		}
		if( board[x-1][y+1] == 1 ){
			int dnd = 0;
			int l1 = -1, l2 = 8;
			for( int j = y + 2, i = x - 2; i > l1 && j < l2; --i, ++j ){
				if( board[i][j] == 2 && dnd == 0 ){
					dnd = 1;
					l1 = i - 1, l2 = j + 1;
					i = x, j = y;
				}
				if( dnd == 1 )
					board[i][j] = 2;

			} 
		}
		if( board[x][y-1] == 1 ){
			int dnd = 0;
			int l1 = -1;
			for( int i = y - 2; i > l1; --i ){
				if( board[x][i] == 2 && dnd == 0 ){
					dnd = 1;
					l1 = i - 1;				
					i = y;
				}
				if( dnd == 1 )
					board[x][i] = 2;
			}	
		}
		if( board[x][y+1] == 1 ){
			int dnd = 0;
			int l1 = 8;
			for( int i = y + 2; i < l1; ++i ){
				if( board[x][i] == 2 && dnd == 0 ){
					dnd = 1;
					l1 = i + 1;
					i = y;
				}
				if( dnd == 1)
					board[x][i] = 2;
			}
		}
		if( board[x+1][y] == 1 ){
			int dnd = 0;
			int l1 = 8;
			for( int i = x + 2; i < l1; ++i ){
				if( board[i][y] == 2 && dnd == 0 ){
					dnd = 1;
					l1 = i + 1;
					i = x;
				}
				if( dnd == 1 )
					board[i][y] = 2;
			}
		}
		if( board[x+1][y-1] == 1 ){
			int dnd = 0;
			int l1 = 8, l2 = -1;
			for( int j = y - 2, i = x + 2; i < l1 && j > l2; ++i, --j ){
				if( board[i][j] == 2 && dnd == 0 ){
					dnd = 1;
					l1 = i + 1, l2 = j - 1;
					i = x, j = y;
				}
				if( dnd == 1 )
					board[i][j] = 2;
			}
		}
		if( board[x+1][y+1] == 1 ){
			int dnd = 0;
			int l1 = 8, l2 = 8;
			for( int j = y + 2, i = x + 2; i < l1 && j < l2; ++i, ++j){
				if( board[i][j] == 2 && dnd == 0 ){
					dnd = 1;
					l1 = i + 1, l2 = j + 1;
					i = x, j = y;
				}
				if( dnd == 1 )
					board[i][j] = 2;
			}
		}
	}
}
