#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <cairo.h>
#include <gtk/gtk.h>


//Window
  GtkWidget *window;

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;
//game running -> 1 |||| game not running -> 0
int game_running = 0;
//sd will be the socket
  int sd;
  struct sockaddr_in server;
//retain "your turn"
char your_turn[20];

char g_end[10];

//retain which player are you
int turn;

int gboard[8][8], selectbutton; // select - butoane din meniu -> 1 play -> draw board

double coordx, coordy, px, py;
int chk;



void place_piece(cairo_t *cr, int cordx, int cordy);
void draw_prev_state(cairo_t *cr);


void draw_prev_state(cairo_t *cr){
cairo_set_line_width(cr, 0.5);  
cairo_set_source_rgb(cr, 0.69, 0.19, 0);
	for( int i = 0; i < 8; ++i){
		for( int j = 0; j < 8; ++j){
			if( gboard[i][j] != 0 ){
				cairo_save(cr);					
				cairo_translate(cr, i * 60 + 250 + 30, j * 60 + 140 + 30);
				cairo_arc(cr, 0, 0, 25, 0, 2 * G_PI);
				cairo_restore(cr);
				cairo_save(cr);
				cairo_close_path(cr);
				cairo_stroke_preserve(cr);
				if( gboard[i][j] == 1 )
					cairo_set_source_rgb(cr, 0.3, 0.4, 0.6); 
				if(gboard[i][j] == 2 )
					cairo_set_source_rgb(cr, 0, 0, 0);
				cairo_fill(cr);
				cairo_save(cr);
				cairo_restore (cr);
			}
		}
	}
}

//place piece
static void do_drawing(cairo_t *);

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, 
    gpointer user_data)
{
  do_drawing(cr);

  return FALSE;
}

//downward_________________________________________-----------------------_____
void turn_around( int x, int y){

	if( gboard[x][y] == 1 ){
		if( gboard[x-1][y] == 2 ){
			int dnd = 0;
			int l1 = -1;
			for( int i = x - 2; i > l1; --i){
				if( gboard[i][y] == 1 && dnd == 0){
					dnd = 1;
					l1 = i - 1;
					i = x;
				}
				if( dnd == 1 )
					gboard[i][y] = 1;
					
			}	
		}
		if( gboard[x-1][y-1] == 2 ){
			int dnd = 0;
			int l1 = -1, l2 = -1;
			for( int j = y - 2, i = x - 2; i > l1 && j > l2; --i, --j){
				if( gboard[i][j] == 1 && dnd == 0){
					dnd = 1;
					l1 = i - 1, l2 = j - 1;
					i = x, j = y;
				}
				if( dnd == 1 )
					gboard[i][j] = 1;
			}	
		}
		if( gboard[x-1][y+1] == 2 ){
			int dnd = 0;
			int l1 = -1, l2 = 8;
			for( int j = y + 2, i = x - 2; i > l1 && j < l2; --i, ++j){
				if( gboard[i][j] == 1 && dnd == 0){
					dnd = 1;
					l1 = i - 1, l2 = j + 1;
					i = x, j = y;
				}
				if( dnd == 1 )
					gboard[i][j] = 1;
			} 
		}
		if( gboard[x][y-1] == 2 ){
			int dnd = 0;
			int l1 = -1;
			for( int i = y - 2; i > l1; --i ){
				if(gboard[x][i] == 1 && dnd == 0 ){
					dnd = 1;
					l1 = i - 1;
					i = y;
				}
				if( dnd == 1 )
					gboard[x][i] = 1;
			}	
		}
		if( gboard[x][y+1] == 2 ){
			int dnd = 0;
			int l1 = 8;
			for( int i = y + 2; i < l1; ++i ){
				if( gboard[x][i] == 1 && dnd == 0 ){
					dnd = 1;
					l1 = i + 1;
					i = y;
				}
				if( dnd == 1)
					gboard[x][i] = 1;
			}
		}
		if( gboard[x+1][y] == 2 ){
			int dnd = 0;
			int l1 = 8;
			for( int i = x + 2; i < l1; ++i ){
				if( gboard[i][y] == 1 && dnd == 0 ){
					dnd = 1;
					l1 = i + 1;
					i = x;
				}
				if( dnd == 1 )
					gboard[i][y] = 1;
			}
		}
		if( gboard[x+1][y-1] == 2 ){
			int dnd = 0;
			int l1 = 8, l2 = -1;
			for( int j = y - 2, i = x + 2; i < l1 && j > l2; ++i, --j){
				if( gboard[i][j] == 1 && dnd == 0 ){
					dnd = 1;
					l1 = i + 1, l2 = j - 1;
					i = x, j = y;
				}
				if( dnd == 1 )
					gboard[i][j] = 1;
			}
		}
		if(gboard[x+1][y+1] == 2 ){
			int dnd = 0;
			int l1 = 8, l2 = 8;
			for( int j = y + 2, i = x + 2; i < l1 && j < l2; ++i, ++j ){
				if( gboard[i][j] == 1 && dnd == 0 ){
					dnd = 1;
					l1 = i + 1, l2 = j + 1;
					i = x, j = y;
				}
				if( dnd == 1 )
					gboard[i][j] = 1;
			}
		}
	}


	if( gboard[x][y] == 2){
		if( gboard[x-1][y] == 1 ){
			int dnd = 0;
			int l1 = -1;
			for( int i = x - 2; i > l1; --i ){
				if( gboard[i][y] == 2 && dnd == 0 ){
					dnd = 1;
					l1 = i - 1;
					i = x;
				}
				if( dnd == 1 )
					gboard[i][y] = 2;
					
			}	
		}
		if( gboard[x-1][y-1] == 1 ){
			int dnd = 0;
			int l1 = -1, l2 = -1;
			for( int j = y - 2, i = x - 2; i > l1 && j > l2; --i, --j ){
				if( gboard[i][j] == 2 && dnd == 0){
					dnd = 1;
					l1 = i - 1, l2 = j - 1;
					i = x, j = y;
				}
				if( dnd == 1 )
					gboard[i][j] = 2;
			}	
		}
		if( gboard[x-1][y+1] == 1 ){
			int dnd = 0;
			int l1 = -1, l2 = 8;
			for( int j = y + 2, i = x - 2; i > l1 && j < l2; --i, ++j ){
				if( gboard[i][j] == 2 && dnd == 0 ){
					dnd = 1;
					l1 = i - 1, l2 = j + 1;
					i = x, j = y;
				}
				if( dnd == 1 )
					gboard[i][j] = 2;

			} 
		}
		if( gboard[x][y-1] == 1 ){
			int dnd = 0;
			int l1 = -1;
			for( int i = y - 2; i > l1; --i ){
				if( gboard[x][i] == 2 && dnd == 0 ){
					dnd = 1;
					l1 = i - 1;				
					i = y;
				}
				if( dnd == 1 )
					gboard[x][i] = 2;
			}	
		}
		if( gboard[x][y+1] == 1 ){
			int dnd = 0;
			int l1 = 8;
			for( int i = y + 2; i < l1; ++i ){
				if(gboard[x][i] == 2 && dnd == 0 ){
					dnd = 1;
					l1 = i + 1;
					i = y;
				}
				if( dnd == 1)
					gboard[x][i] = 2;
			}
		}
		if( gboard[x+1][y] == 1 ){
			int dnd = 0;
			int l1 = 8;
			for( int i = x + 2; i < l1; ++i ){
				if( gboard[i][y] == 2 && dnd == 0 ){
					dnd = 1;
					l1 = i + 1;
					i = x;
				}
				if( dnd == 1 )
					gboard[i][y] = 2;
			}
		}
		if( gboard[x+1][y-1] == 1 ){
			int dnd = 0;
			int l1 = 8, l2 = -1;
			for( int j = y - 2, i = x + 2; i < l1 && j > l2; ++i, --j ){
				if( gboard[i][j] == 2 && dnd == 0 ){
					dnd = 1;
					l1 = i + 1, l2 = j - 1;
					i = x, j = y;
				}
				if( dnd == 1 )
					gboard[i][j] = 2;
			}
		}
		if( gboard[x+1][y+1] == 1 ){
			int dnd = 0;
			int l1 = 8, l2 = 8;
			for( int j = y + 2, i = x + 2; i < l1 && j < l2; ++i, ++j){
				if( gboard[i][j] == 2 && dnd == 0 ){
					dnd = 1;
					l1 = i + 1, l2 = j + 1;
					i = x, j = y;
				}
				if( dnd == 1 )
					gboard[i][j] = 2;
			}
		}
	}
}

int piece_count = 30;
static gboolean on_draw_help(GtkWidget *widget, cairo_t *cr, gpointer user_data);
void display_result(int result){
  GList *children, *iter;

  children = gtk_container_get_children(GTK_CONTAINER(window));
  for(iter = children; iter != NULL; iter = g_list_next(iter))
	gtk_widget_destroy(GTK_WIDGET(iter->data));
  g_list_free(children);


  GtkWidget *darea;  
  GtkWidget *contain_box;
  GtkWidget *align;
  GtkWidget *ret_menu;


  contain_box = gtk_box_new(FALSE, 0);
  align = gtk_alignment_new(0.5, 0.5, 0, 0);	
if( result == 1 ) 
  darea = gtk_label_new("Win");
else if ( result == 2 )
  darea = gtk_label_new("Loss");
else if (result == 3 )
  darea = gtk_label_new("Draw"); 
  gtk_widget_set_size_request(ret_menu, 200, 100);
  gtk_label_set_justify(GTK_LABEL(darea), GTK_JUSTIFY_CENTER);
  

  ret_menu = gtk_button_new_with_label("Menu");
  gtk_widget_set_size_request(ret_menu, 70, 30);
  



  gtk_container_add(GTK_CONTAINER(contain_box), darea);
  gtk_container_add(GTK_CONTAINER(contain_box), ret_menu);
  gtk_container_add(GTK_CONTAINER(align), contain_box);
  gtk_container_add(GTK_CONTAINER(window), align);


  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  
  g_signal_connect(G_OBJECT(ret_menu),"clicked",G_CALLBACK(on_draw_help), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);




  gtk_widget_show_all(window);
}


void display_menu();
void place_piece(cairo_t *cr, int cordx, int cordy){
	if( piece_count > 0 ){
		if( turn == 1 ){		
			int x = ( cordx - 250 ) / 60, y = ( cordy - 140 ) / 60;
			read(sd, &your_turn, sizeof(char) * 9 );
			if( strncmp(your_turn, "your turn", 9) == 0 ){
				write(sd, "ready", sizeof(char) * 5);
				x = ntohl(x); y = ntohl(y);	
				if( write(sd, &x, sizeof(int)) < 0 ) {
					fprintf(stdout,"\n-------------------doing this again\n\n");
				}
				write(sd, &y, sizeof(int));
				x = htonl(x), y = htonl(y);
				fprintf(stdout, " i'm sending %d and %d\n", x, y);
				read(sd, &chk, sizeof(int));
				chk = htonl(chk);
				fprintf(stdout,"i received chk = %d\n", chk);
				if( chk == 1 ){
					gboard[x][y] = 1;
					fprintf(stdout,"---inside if: i receive chk=1\n");
					--piece_count;
					//check pieces placement
					turn_around( x, y);

				}
				fprintf(stdout,"is gboard[%d][%d] equat with 1?\n", x, y);
				fprintf(stdout,"gboard[%d][%d] = %d \n", x, y, gboard[x][y]);
				for( int i = 0; i < 8; ++i ){
					for( int j = 0; j < 8; ++j){
						fprintf(stdout, "%d   ",gboard[i][j]);
					}
					fprintf(stdout,"\n\n");
				}
				cairo_set_line_width(cr, 0.5);  
				cairo_set_source_rgb(cr, 0.69, 0.19, 0);
				for( int i = 0; i < 8; ++i){
					for( int j = 0; j < 8; ++j){
						if( gboard[i][j] != 0 ){
							cairo_save(cr);					
							cairo_translate(cr, i * 60 + 250 + 30, j * 60 + 140 + 30);
							cairo_arc(cr, 0, 0, 25, 0, 2 * G_PI);
							cairo_restore(cr);
							cairo_save(cr);
							cairo_close_path(cr);
							cairo_stroke_preserve(cr);
							if( gboard[i][j] == 1 )
								cairo_set_source_rgb(cr, 0.3, 0.4, 0.6); 
							if( gboard[i][j] == 2 )
								cairo_set_source_rgb(cr, 0, 0, 0);
							cairo_fill(cr);
							cairo_save(cr);
							cairo_restore (cr);
						}
					}
				}

				fprintf(stdout,"\nwhen do i pass this for, reall?\n\n");
				if( chk == 1 ){
					read(sd, &x, sizeof(int));
					read(sd, &y, sizeof(int));
					fprintf(stdout,"----before translating they are %d and %d\n", x, y);
					x = htonl(x); y = htonl(y);

					if( x == - 5 )
						goto game_win;

					fprintf(stdout,"----other player placed %d and %d\n", x, y);
					gboard[x][y] = 2;
					//check pieces placement
					turn_around( x, y);
				}
				cairo_set_line_width(cr, 0.5);  
				cairo_set_source_rgb(cr, 0.69, 0.19, 0);
				for( int i = 0; i < 8; ++i){
					for( int j = 0; j < 8; ++j){
						if( gboard[i][j] != 0 ){
							cairo_save(cr);					
							cairo_translate(cr, i * 60 + 250 + 30, j * 60 + 140 + 30);
							cairo_arc(cr, 0, 0, 25, 0, 2 * G_PI);
							cairo_restore(cr);
							cairo_save(cr);
							cairo_close_path(cr);
							cairo_stroke_preserve(cr);
							if( gboard[i][j] == 1 )
								cairo_set_source_rgb(cr, 0.3, 0.4, 0.6); 
							if( gboard[i][j] == 2 )
								cairo_set_source_rgb(cr, 0, 0, 0);
							cairo_fill(cr);
							cairo_save(cr);
							cairo_restore (cr);
						}
					}
				}


			}
			for( int i = 0; i < 8; ++i ){
				for( int j = 0; j < 8; ++j){
					fprintf(stdout, "%d   ",gboard[i][j]);
				}
				fprintf(stdout,"\n\n");
			}
		}//turn if
		else{
			int xo, yo;
			if( chk == -1 ){
				read(sd, &chk, sizeof(int));
				chk = htonl(chk);
				fprintf(stdout,"\n\n %d \n\n", chk);
				if( chk == -5 )
					goto game_win;
			}
			if( chk == 1 ){		
				read(sd, &xo, sizeof(int));
				read(sd, &yo, sizeof(int));
				fprintf(stdout,"----before translating %d and %d\n", xo, yo);
				xo = htonl(xo), yo = htonl(yo);
				fprintf(stdout,"----other player placed %d and %d\n", xo, yo);
				gboard[xo][yo] = 2;
				// check pieces placement
				turn_around( xo, yo);
			}
			cairo_set_line_width(cr, 0.5);  
			cairo_set_source_rgb(cr, 0.69, 0.19, 0);
			for( int i = 0; i < 8; ++i){
				for( int j = 0; j < 8; ++j){
					if( gboard[i][j] != 0 ){
						cairo_save(cr);					
						cairo_translate(cr, i * 60 + 250 + 30, j * 60 + 140 + 30);
						cairo_arc(cr, 0, 0, 25, 0, 2 * G_PI);
						cairo_restore(cr);
						cairo_save(cr);
						cairo_close_path(cr);
						cairo_stroke_preserve(cr);
						if( gboard[i][j] == 1 )
							cairo_set_source_rgb(cr, 0.3, 0.4, 0.6); 
						if( gboard[i][j] == 2 )
							cairo_set_source_rgb(cr, 0, 0, 0);
						cairo_fill(cr);
						cairo_save(cr);
						cairo_restore (cr);
					}
				}
			}


			read(sd, &your_turn, sizeof(char) * 9 );
			if( strncmp(your_turn, "your turn", 9) == 0){
				write(sd, "ready", sizeof(char) * 5);
				int x = ( cordx - 250 ) / 60, y = ( cordy - 140 ) / 60;
				x = ntohl(x), y = ntohl(y);	
				write(sd, &x, sizeof(int));
				write(sd, &y, sizeof(int));
				x = htonl(x), y = htonl(y);
				fprintf(stdout, " i'm sending %d and %d\n", x, y);

				chk = 0;

				read(sd, &chk, sizeof(int));
				chk = htonl(chk);
				if( chk == 1 ){
					gboard[x][y] = 1;
					--piece_count;
					chk = -1;
					//check pieces placement
					turn_around( x, y);
				}
				cairo_set_line_width(cr, 0.5);  
				cairo_set_source_rgb(cr, 0.69, 0.19, 0);
				for( int i = 0; i < 8; ++i){
					for( int j = 0; j < 8; ++j){
						if( gboard[i][j] != 0 ){
							cairo_save(cr);					
							cairo_translate(cr, i * 60 + 250 + 30, j * 60 + 140 + 30);
							cairo_arc(cr, 0, 0, 25, 0, 2 * G_PI);
							cairo_restore(cr);
							cairo_save(cr);
							cairo_close_path(cr);
							cairo_stroke_preserve(cr);
							if( gboard[i][j] == 1 )
								cairo_set_source_rgb(cr, 0.3, 0.4, 0.6); 
							if( gboard[i][j] == 2 )
								cairo_set_source_rgb(cr, 0, 0, 0);
							cairo_fill(cr);
							cairo_save(cr);
							cairo_restore (cr);
						}
					}
				}

			}
			for( int i = 0; i < 8; ++i ){
				for( int j = 0; j < 8; ++j){
					fprintf(stdout, "%d   ",gboard[i][j]);
				}
				fprintf(stdout,"\n\n");
			}
		} // turn else
	}
	else{
		game_win:
		read( sd, &g_end, sizeof(char) * 3 );
		if( strncmp(g_end, "win", 3) == 0 ){
			display_result(1);
		}
		else if ( strncmp( g_end, "los", 3 ) == 0 ){
			display_result(2);

		}
		else if ( strncmp( g_end, "drw", 3 ) == 3 ){
			display_result(3);
		}
		write(sd,"get out", sizeof(char) * 7);
	}
}

static void do_drawing(cairo_t *cr)
{
 
  int cordx = coordx, cordy = coordy;
  if( cordx >= 830 && coordx <= 920 )
	if( cordy >= 675 && cordy <= 705 ){
		write(sd, "withd", sizeof(char) * 5);
		display_result(2);
	}
  if( cordx >= 250 && cordx <= 730 ){
	while( ( cordx - 250 ) %60 != 0 ){
		--cordx;
	}
	if( cordy >= 140 && cordy <= 620 ){
		while( ( cordy - 140 ) % 60 != 0 ){
			--cordy;
		}
		place_piece(cr,cordx, cordy);

  	}
	else
		draw_prev_state(cr); 
  }
  else
	draw_prev_state(cr);  
}

static gboolean clicked(GtkWidget *widget, GdkEventButton *event,
    gpointer user_data)
{
    if (event->button == 1) {
	coordx = event->x;
	coordy = event->y;
	fprintf(stdout,"coords are x %f and y %f\n", coordx, coordy);
	gtk_widget_queue_draw(widget);
    }

    return TRUE;
}




//board
static void do_board(cairo_t *);

static gboolean on_draw_board(GtkWidget *widget, cairo_t *cr,gpointer user_data)
{      
  do_board(cr);

  return FALSE;
}

static void do_board(cairo_t *cr)
{
  cairo_set_line_width (cr, 0.7);
  cairo_set_source_rgb (cr, 0, 0, 0);

  for( int i = 140; i <= 620; i = i+60){
	cairo_move_to (cr, 250, i);
	cairo_line_to (cr, 730, i);
	cairo_stroke (cr);
  }
  for(int j = 250; j <= 730; j = j+60){
	cairo_move_to (cr, j, 140);
	cairo_line_to (cr, j, 620);
	cairo_stroke (cr);
  } 
//feint button

  cairo_set_line_width(cr, 30.0 );
  cairo_move_to( cr, 830, 690 );
  cairo_line_to( cr, 920, 690 );
  cairo_stroke (cr);
/*
cairo_close_path(cr);

char text[10] = "Concede";
  cairo_scale (cr, 30, 30);
  cairo_set_font_size (cr, 0.8);

  /* Drawing code goes here */
 /* cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
  cairo_select_font_face (cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_translate (cr, 850, 690);
	cairo_show_text (cr, "Concede");
  cairo_close_path(cr);
*/
}




void display_menu();

static gboolean on_draw_help(GtkWidget *widget, cairo_t *cr, 
    gpointer user_data)
{
  display_menu();

  return FALSE;
}







//rank
static void do_rank(cairo_t *);

static gboolean on_draw_rank(GtkWidget *widget, cairo_t *cr, 
    gpointer user_data)
{
  

  return FALSE;
}


static void do_rank(cairo_t *cr)
{
	cairo_text_extents_t te;
	char alphabet[] = "AbCdEfGhIjKlMnOpQrStUvWxYz";
	char letter[2];
  	cairo_scale (cr, 30, 30);
	cairo_set_font_size (cr, 0.8);

	/* Drawing code goes here */
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_select_font_face (cr, "Georgia",
			CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

	for (int i=0; i < strlen(alphabet); i++) {
		/* get each letter separately
		 * (from comp.lang.c FAQ, Question 13.2) */
		*letter = '\0';
		strncat (letter, alphabet + i, 1);

		cairo_text_extents (cr, letter, &te);
		cairo_move_to (cr, i + 0.5 - te.x_bearing - te.width / 2,
				0.5 - te.y_bearing - te.height / 2);
		cairo_show_text (cr, letter);
	}
}






//pressed play -> make game board
static gboolean pressed_play(GtkWidget *widget, cairo_t *cr,gpointer user_data)
{   
  GList *children, *iter;

  children = gtk_container_get_children(GTK_CONTAINER(window));
  for(iter = children; iter != NULL; iter = g_list_next(iter))
	gtk_widget_destroy(GTK_WIDGET(iter->data));
  g_list_free(children);


//connect with an opponent
  char comm[20];
  strcpy(comm, "find match");
  int llg = strlen(comm);
  write(sd, &llg, sizeof(int));
  if (write (sd, &comm, sizeof(char)*10) <= 0)
    {
      perror ("Eroare la write() spre server.\n");
      return errno;
    }
  //do i make it wait until it receives a response?
  // most likely yes
  // maybe make some queue screen
  read(sd, &comm, sizeof(char) * 11);
  read(sd, &turn, sizeof(int));
  turn = htonl(turn);
  fprintf(stdout, "i am player %d\n", turn);
  if( turn == 2 ){
	chk = -1;
  }
  for( int i = 0; i < 8; ++i){
	for(int j = 0; j < 8; ++j){
		gboard[i][j] = 0;
	}
  }
  if( turn == 1 ){
  	gboard[3][3] = 1, gboard[3][4] = 2, gboard[4][3] = 2, gboard[4][4] = 1;
  }
  if( turn == 2 ){
	  gboard[3][3] = 2, gboard[3][4] = 1, gboard[4][3] = 1, gboard[4][4] = 2;
  }
  game_running = 1;
//game board   
  GtkWidget *darea;  

  darea = gtk_drawing_area_new();
  gtk_container_add(GTK_CONTAINER(window), darea);

  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  g_signal_connect(G_OBJECT(darea), "draw", 
      G_CALLBACK(on_draw_board), NULL);




//place piece
  g_signal_connect(G_OBJECT(darea), "draw", 
      G_CALLBACK(on_draw_event), NULL);


  g_signal_connect(window, "button-press-event", 
      G_CALLBACK(clicked), NULL);

  gtk_widget_show_all(window);
}




//pressed rankings -> display rankings
static gboolean pressed_rank(GtkWidget *widget, cairo_t *cr,gpointer user_data)
{   
  GList *children, *iter;

  children = gtk_container_get_children(GTK_CONTAINER(window));
  for(iter = children; iter != NULL; iter = g_list_next(iter))
	gtk_widget_destroy(GTK_WIDGET(iter->data));
  g_list_free(children);

  int szrn = 8;
  char ranks[200];
  szrn = ntohl(szrn);
  write(sd, &szrn, sizeof(int) );
  write(sd, "get rank", sizeof(char) * 8 );

  read(sd, &szrn, sizeof(int) );
  szrn = htonl(szrn);
  read(sd, &ranks, sizeof(char) * szrn );
  ranks[strlen(ranks)] = '\0';
  fprintf(stdout, "%s, %d\n", ranks, szrn);

  GtkWidget *darea;  
  GtkWidget *contain_box;
  GtkWidget *align;
  GtkWidget *ret_menu;


  contain_box = gtk_list_box_new();
  align = gtk_alignment_new(0.5, 0.5, 0.5, 0.5);	
  
  darea = gtk_label_new(NULL);
  char * markup;
  markup = g_markup_printf_escaped("<span font_desc=\"20.0\">%s</span>", ranks);
  gtk_label_set_markup(darea, markup); 
  gtk_widget_set_size_request(darea, 100, 700);
  gtk_label_set_justify(GTK_LABEL(darea), GTK_JUSTIFY_CENTER);
  

  ret_menu = gtk_button_new_with_label("Menu");
  gtk_widget_set_size_request(ret_menu, 70, 30);
  


  gtk_list_box_insert(contain_box, darea, -1);
  gtk_list_box_insert(contain_box, ret_menu, -1);
  gtk_container_add(GTK_CONTAINER(align), contain_box);
  gtk_container_add(GTK_CONTAINER(window), align);


  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  
  g_signal_connect(G_OBJECT(ret_menu),"clicked",
	G_CALLBACK(on_draw_help), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);



  gtk_widget_show_all(window);
}





//pressed help -> display instructions
static gboolean pressed_help(GtkWidget *widget, cairo_t *cr,gpointer user_data)
{   
  GList *children, *iter;

  children = gtk_container_get_children(GTK_CONTAINER(window));
  for(iter = children; iter != NULL; iter = g_list_next(iter))
	gtk_widget_destroy(GTK_WIDGET(iter->data));
  g_list_free(children);


  GtkWidget *darea;  
  GtkWidget *contain_box;
  GtkWidget *align;
  GtkWidget *ret_menu;


  contain_box = gtk_list_box_new();
  align = gtk_alignment_new(0.5, 0.5, 0, 0);	
  darea = gtk_label_new("Each reversi piece has a black side and a white side\n\n\
	On your turn, you place a piece on the board with your color facing up. You must place \n\n\
	the piece such that an opponent's piece or a row of the opponent's pieces is flanked by your pieces. \n\n When this happens, the opponent's pieces between your pieces are turned over to become your color.\n\n\
	The objective of the game is to have more pieces than your opponent when the game is over. This\n\n\
	happens when neither player can make another move, usually meaning that the board is full.\n\n\
	The game is started as is shown below and a move consists of placing a piece on an empty square."); 
  gtk_label_set_use_markup (GTK_LABEL (darea), TRUE);
  
  gtk_widget_set_size_request(darea, 600, 600);	
  gtk_label_set_justify(GTK_LABEL(darea), GTK_JUSTIFY_CENTER);
  

  ret_menu = gtk_button_new_with_label("Menu");
  gtk_widget_set_size_request(ret_menu, 70, 30);
  



  gtk_container_add(GTK_CONTAINER(contain_box), darea);
  gtk_container_add(GTK_CONTAINER(contain_box), ret_menu);
  gtk_container_add(GTK_CONTAINER(align), contain_box);
  gtk_container_add(GTK_CONTAINER(window), align);


  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  
  g_signal_connect(G_OBJECT(ret_menu),"clicked",G_CALLBACK(on_draw_help), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);




  gtk_widget_show_all(window);
}


	GtkWidget *u_name;
	GtkWidget *pass;
void create_window(GtkWidget *button, gpointer window) {

	GtkWidget *win, *label;

	const gchar * username, * password;
	username = gtk_entry_get_text(u_name);
	password = gtk_entry_get_text(pass);
	char usnm[30], passwd[30];
	strcpy(usnm, username);
	strcpy(passwd, password);
	int uuu = strlen(usnm), ppp = strlen(passwd);
	fprintf(stdout,"username is %s, %d, %d, pass is %s\n", usnm, uuu, sizeof(username), password);
	uuu = ntohl(uuu), ppp = ntohl(ppp);
	write(sd, &uuu, sizeof(int));
	uuu = htonl(uuu);
	fprintf(stdout,"%d\n", uuu);
	write(sd, &usnm, sizeof(char) * uuu);
	write(sd, &ppp, sizeof(int));
	ppp = htonl(ppp);
	write(sd, &passwd, sizeof(char) * ppp);
	int logcheck;
	read( sd, &logcheck, sizeof(int) );
	logcheck = htonl(logcheck);
	if( logcheck )	
	{
	    display_menu();
	}

	else 

	{
	    label = gtk_label_new("Username and Password is incorrect.");
	    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	    gtk_container_add(GTK_CONTAINER(win), label);
	    gtk_widget_show_all(win);

	}

	}

	static void destroy(GtkWidget *widget, gpointer data){
	gtk_main_quit();
}


void display_login(){
	GList *children, *iter;

	children = gtk_container_get_children(GTK_CONTAINER(window));
	for(iter = children; iter != NULL; iter = g_list_next(iter))
	gtk_widget_destroy(GTK_WIDGET(iter->data));
	g_list_free(children);
	  

	GtkWidget *grid;
	GtkWidget *Login_button, *Quit_button;

	GtkWidget *label_user;
	GtkWidget *label_pass;
	GtkWidget  *button_container;




	grid = gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(grid), 3);
	gtk_container_add(GTK_CONTAINER(window), grid);

	label_user = gtk_label_new("Username  ");
	label_pass = gtk_label_new("Password  ");

	u_name = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(u_name), "Username");
	gtk_grid_attach(GTK_GRID(grid), label_user, 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), u_name, 1, 1, 2, 1);


	pass = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(pass), "Password");
	gtk_grid_attach(GTK_GRID(grid), label_pass, 0, 2, 1, 1);
	gtk_entry_set_visibility(GTK_ENTRY(pass), 0);
	gtk_grid_attach(GTK_GRID(grid), pass, 1, 2, 1, 1);


	Login_button = gtk_button_new_with_label("Log in");
	g_signal_connect(Login_button, "clicked", G_CALLBACK(create_window), NULL);
	gtk_grid_attach(GTK_GRID(grid), Login_button, 0, 3, 2, 1);

	Quit_button = gtk_button_new_with_label("Quit");
	g_signal_connect(Quit_button, "clicked", G_CALLBACK(gtk_main_quit), NULL);
	gtk_grid_attach(GTK_GRID(grid), Quit_button, 0, 4, 2, 1);

}


//diplay menu, duh
void display_menu(){
  GList *children, *iter;

  children = gtk_container_get_children(GTK_CONTAINER(window));
  for(iter = children; iter != NULL; iter = g_list_next(iter))
	gtk_widget_destroy(GTK_WIDGET(iter->data));
  g_list_free(children);


  GtkWidget *vbox;

  GtkWidget *menubar;
  GtkWidget *playMenu;
  GtkWidget *playMi;
  GtkWidget *rankMi;
  GtkWidget *helpMi;
  GtkWidget *quitMi;
  GtkWidget *align;

  

 // g_signal_connect(window, "button-press-event", 
 //     G_CALLBACK(clicked), NULL);



  align = gtk_alignment_new(0.5, 0.5, 0, 0);
  vbox = gtk_box_new(FALSE, 0);
  //gtk_container_add(GTK_CONTAINER(window), vbox);
  gtk_container_add(GTK_CONTAINER(align), vbox);
  gtk_container_add(GTK_CONTAINER(window), align);



//create menu
  menubar = gtk_menu_bar_new();
  gtk_menu_bar_set_pack_direction(menubar, GTK_PACK_DIRECTION_TTB);
  playMenu = gtk_menu_new();

  playMi = gtk_menu_item_new_with_label("Play");
  rankMi = gtk_menu_item_new_with_label("Rankings");
  helpMi = gtk_menu_item_new_with_label("Help");
  quitMi = gtk_menu_item_new_with_label("Quit");

  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), playMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), rankMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), helpMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), quitMi);
  gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);



//pressed quit
  g_signal_connect(G_OBJECT(quitMi), "activate",
        G_CALLBACK(gtk_main_quit), NULL);
  

//pressed play
  g_signal_connect(G_OBJECT(playMi), "activate",
        G_CALLBACK(pressed_play), NULL);   


//pressed rank
  g_signal_connect(G_OBJECT(rankMi), "activate",
        G_CALLBACK(pressed_rank), NULL);


//pressed help
  g_signal_connect(G_OBJECT(helpMi), "activate",
        G_CALLBACK(pressed_help), NULL);      
  
 

  gtk_widget_show_all(window);
}



int main (int argc, char *argv[])
{		
  //struct sockaddr_in server;	// structura folosita pentru conectare 

  /* exista toate argumentele in linia de comanda? */
  if (argc != 3)
    {
      printf ("[client] Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

  /* stabilim portul */
  port = atoi (argv[2]);

  /* cream socketul */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[client] Eroare la socket().\n");
      return errno;
    }
  

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr(argv[1]);
  /* portul de conectare */
  server.sin_port = htons (port);
  
  /* ne conectam la server */
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }



  GtkWidget *vbox;

  GtkWidget *menubar;
  GtkWidget *playMenu;
  GtkWidget *playMi;
  GtkWidget *rankMi;
  GtkWidget *helpMi;
  GtkWidget *quitMi;
  GtkWidget *align;
  gtk_init(&argc, &argv);


  window = gtk_window_new(GTK_WINDOW_TOPLEVEL); 
  

  g_signal_connect(window, "button-press-event", 
      G_CALLBACK(clicked), NULL);

  g_signal_connect(window, "button-press-event", 
      G_CALLBACK(clicked), NULL);


//create window
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 1024, 720);
  gtk_window_set_title(GTK_WINDOW(window), "Reversi");
   
  g_signal_connect(G_OBJECT(window), "destroy",
        G_CALLBACK(gtk_main_quit), NULL);
 
 
//login screen 
  display_login();
//din login ma duc in menu
//menu screen
//  display_menu();  
 



  gtk_widget_show_all(window);

  gtk_main();



}
