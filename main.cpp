#include <iostream>
#include <ncursesw/curses.h>
#define XMAX 33
#define YMAX 18

static const char chip_X[] = {1, 1, 1, 0, 0, 1, 1, 1,
                              0, 0, 1, 1, 1, 1, 0, 0,
                              0, 0, 0, 1, 1, 0, 0, 0,
                              0, 0, 1, 1, 1, 1, 0, 0,
                              1, 1, 1, 0, 0, 1, 1, 1
                             };

static const char chip_O[] = {0, 0, 1, 1, 1, 1, 0, 0,
                              1, 1, 1, 0, 0, 1, 1, 1,
                              1, 1, 0, 0, 0, 0, 1, 1,
                              1, 1, 1, 0, 0, 1, 1, 1,
                              0, 0, 1, 1, 1, 1, 0, 0
                             };
//Zrob cos z tym boardem bo wyglada jak shit *<*
struct board {
	int x;
	int y;
	int previousChip[2];
	int chipsInPlay[2];
	char chips[3][3];
	int player;
	int movements;
	int move;
};
class Map {
	public:
		void draw_chip(int *dest, char type, board *Tboard) {
			int y,x;

			for (int i = 0; i < 5; i++)     // Chip wysokosc
				for (int j = 0; j < 8; j++) {   // Chip szerokosc
					y = YMAX / 3 * dest[0] + i + Tboard->y + 1;
					x = XMAX / 3 * dest[1] + j + Tboard->x + 2;
					if (chip_X[j + 8 * i] == 1 && type == 'x') {        // If X chip...
						attron(COLOR_PAIR(1));
						mvaddch(y, x, '#');
						attroff(COLOR_PAIR(1));
					} else if (chip_O[j + 8 * i] == 1 && type == 'o') {   // If O chip...
						attron(COLOR_PAIR(2));
						mvaddch(y, x, '#');
						attroff(COLOR_PAIR(2));
					} else {  // drukuj spacje
						mvaddch(y, x, ' ');
					}
				}
		}
		void render_chip(board *Tboard) {

			for(int i=0; i<=YMAX; i++)
				for(int j=0; j<=XMAX; j++) {
					if (i == 0) { // Pierwszy wierwsz na board
						if (j == 0)              //  Lewy - Górny róg.
							mvaddch(Tboard->y + i, Tboard->x + j, ACS_ULCORNER);
						else if (j == XMAX)     // Prawy - Górny róg.
							mvaddch(Tboard->y + i, Tboard->x + j, ACS_URCORNER);
						else if (j % (XMAX / 3) == 0)   // Środkowa kolumna.
							mvaddch(Tboard->y + i, Tboard->x + j, ACS_TTEE);
						else                    // LINIE.
							mvaddch(Tboard->y + i, Tboard->x + j, ACS_HLINE);
					} else if (i % (YMAX / 3) == 0 && i != YMAX) { // Dividery.
						if (j == 0)             // Pierwsza Kolumna.
							mvaddch(Tboard->y + i, Tboard->x + j, ACS_LTEE);
						else if (j == XMAX)     // Ostatnia Kolumna.
							mvaddch(Tboard->y + i, Tboard->x + j, ACS_RTEE);
						else if (j % (XMAX / 3) == 0)   // Środkowa kolumna.
							mvaddch(Tboard->y + i, Tboard->x + j, ACS_PLUS);
						else                    // LINIE.
							mvaddch(Tboard->y + i, Tboard->x + j, ACS_HLINE);
					} else if (i == YMAX) {           // Ostatni wiersz
						if (j == 0)              // Lewy - Dolny róg.
							mvaddch(Tboard->y + i, Tboard->x + j, ACS_LLCORNER);
						else if (j == XMAX)     // Prawy - Dolny róg.
							mvaddch(Tboard->y + i, Tboard->x + j, ACS_LRCORNER);
						else if (j % (XMAX / 3) == 0)   // Środkowa kolumna.
							mvaddch(Tboard->y + i, Tboard->x + j, ACS_BTEE);
						else                    // LINIE.
							mvaddch(Tboard->y + i, Tboard->x + j, ACS_HLINE);
					} else if (j % (XMAX / 3) == 0) { // Środkowe linie.
						mvaddch(Tboard->y + i, Tboard->x + j, ACS_VLINE);
					}
				}
		}
};
class Game : public Map {
	public:
		void clean_chips(board *Tboard) {
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
					Tboard->chips[i][j] = 0;
		}

		int place_chip(int *dest, board *Tboard) {
			if (Tboard->chips[dest[0]][dest[1]] != 0) {  // If cell jest zajety przez jakis chip

				// jesli player postawił juz wszystkie chipy i chce je ruszyc
				if (Tboard -> move == 0) {
					if (Tboard->chipsInPlay[Tboard->player - 1] == 3 && Tboard->chips[dest[0]][dest[1]] == Tboard->player) {
						Tboard->previousChip[0] = dest[0];
						Tboard->previousChip[1] = dest[1];
						draw_chip(dest, ' ', Tboard);
						Tboard->chips[dest[0]][dest[1]] = 0;
						Tboard->chipsInPlay[Tboard->player - 1]--;
						Tboard->move = 1;
						return 6; // Nie rób nic
					} else {
						if (Tboard->chipsInPlay[Tboard->player - 1] == 3)
							return 1;
						else
							return 2;
					}
				} else {
					if (Tboard->chips[dest[0]][dest[1]] == Tboard->player) {
						return 3;
					} else {
						return 2;
					}
				}
			}
			// Jesli player chce polozyc chip
			else if (dest[0] != Tboard->previousChip[0] || dest[1] != Tboard->previousChip[1]) {
				if (Tboard->chipsInPlay[Tboard->player - 1] <3) {   // Jesli player nie postawi wszystkich chipow w grze.
					if (Tboard->player == 1) {

						draw_chip(dest, 'x', Tboard);                          // Rysuj chip.

						Tboard->chips[dest[0]][dest[1]] = Tboard->player;     // Wstawia chip na game board.
						Tboard->chipsInPlay[Tboard->player - 1]++;             // Dodaje chip do player variable
						Tboard->player = 2;                                      // Zmienia gracza.
					} else if (Tboard->player == 2) {

						draw_chip(dest, 'o', Tboard);

						Tboard->chips[dest[0]][dest[1]] = Tboard->player;
						Tboard->chipsInPlay[Tboard->player - 1]++;
						Tboard->player = 1;
					}

					Tboard->move = 0;
					Tboard->previousChip[0] = 3;
					Tboard->previousChip[1] = 3;
				} else {
					return 4;
				}
			} else {
				return 5;
			}

			return 0;  // All good ¯\_('<')_/¯
		}


		void place_cursor(int *orig, int *dest, board *Tboard) {
			int y, x;

			for (int i = 0; i < 5; i++) {
				// Czyszczenie ostatniego kursora
				y = YMAX / 3 * orig[0] + i + Tboard->y + 1;
				x = XMAX / 3 * orig[1] + Tboard->x + 1;
				mvaddch(y, x, ' ');
				x = x + 9;
				mvaddch(y, x, ' ');

				// Setowanie nowego cursora
				y = YMAX / 3 * dest[0] + i + Tboard->y + 1;
				x = XMAX / 3 * dest[1] + Tboard->x + 1;

				attron(COLOR_PAIR(3) | A_BLINK);
				mvaddch(y, x, ACS_DIAMOND);
				x = x + 9;
				mvaddch(y, x, ACS_DIAMOND);
				attroff(COLOR_PAIR(3) | A_BLINK);
			}
		}
		int check_winner(board *Tboard) {
			int temp;

			// Góra-Lewy do Dół-Prawy.
			temp = Tboard->chips[0][0];
			for (int i=1; i<3; i++)
				if (Tboard->chips[i][i] != temp || Tboard->chips[i][i] == 0)     // If nastepny chip sie nie równa.
					temp = 4; // Aby nie overlapowac z empty cells albo players chips.
				else if (i == 2)
					return Tboard->chips[i][i];  // Zwraca zwycięzce.

			// Dół-Lewy do Góra-Prawy.
			temp = Tboard->chips[2][0];
			for (int i=1; i<3; i++)
				if (Tboard->chips[2-i][i] != temp || Tboard->chips[2-i][i] == 0)
					temp = 4;
				else if (i == 2)
					return Tboard->chips[2-i][i];

			// Pierwszy rząd.
			temp = Tboard->chips[0][0];
			for (int i=1; i<3; i++)
				if (Tboard->chips[0][i] != temp || Tboard->chips[0][i] == 0)
					temp = 4;
				else if ( i == 2)
					return Tboard->chips[0][i];

			// Drugi rząd.
			temp = Tboard->chips[1][0];
			for (int i=1; i<3; i++)
				if (Tboard->chips[1][i] != temp || Tboard->chips[1][i] == 0)
					temp = 4;
				else if (i == 2)
					return Tboard->chips[1][i];

			// Trzeci rząd.
			temp = Tboard->chips[2][0];
			for (int i=1; i<3; i++)
				if (Tboard->chips[2][i] != temp || Tboard->chips[2][i] == 0)
					temp = 4;
				else if (i == 2)
					return Tboard->chips[2][i];

			// Pierwsza kolumna.
			temp = Tboard->chips[0][0];
			for (int i=1; i<3; i++)
				if (Tboard->chips[i][0] != temp || Tboard->chips[i][0] == 0)
					temp = 4;
				else if (i == 2)
					return Tboard->chips[i][0];

			// Druga kolumna.
			temp = Tboard->chips[0][1];
			for (int i=1; i<3; i++)
				if (Tboard->chips[i][1] != temp || Tboard->chips[i][1] == 0)
					temp = 4;
				else if (i == 2)
					return Tboard->chips[i][1];

			// Trzecia kolumna.
			temp = Tboard->chips[0][2];
			for (int i=1; i<3; i++)
				if (Tboard->chips[i][2] != temp || Tboard->chips[i][2] == 0)
					temp = 4;
				else if (i == 2)
					return Tboard->chips[i][2];

			return 0; // Nie ma wygranego
		}
		void print_game_messages(board *Tboard) {
			mvprintw(5, 4, "      PLAYER");
			mvprintw(6, 10, "%d TURN", Tboard->player);

			mvprintw(5, 63, "NUMBER OF");
			mvprintw(6, 61, "  MOVES: %d", Tboard->movements);
		}
		int main_game(board *Tboard) {
			int key = 0;
			int winner = 0;
			int check_move = 0;
			int cursorig[] = {0, 0};
			int cursdest[] = {0, 0};


			while (key != '0' && winner == 0) {      // Loop until klawsz 0 albo game end
				print_game_messages(Tboard);
				key = getch();
				switch (key) {
					case KEY_LEFT:
						if (cursorig[1] > 0) {
							cursdest[1] = cursorig[1] - 1;
							place_cursor(cursorig, cursdest, Tboard);
							cursorig[1] = cursdest[1];
						}
						break;

					case KEY_RIGHT:
						if (cursorig[1] < 2) {
							cursdest[1] = cursorig[1] + 1;
							place_cursor(cursorig, cursdest, Tboard);
							cursorig[1] = cursdest[1];
						}
						break;

					case KEY_UP:
						if (cursorig[0] > 0) {
							cursdest[0] = cursorig[0] - 1;
							place_cursor(cursorig, cursdest, Tboard);
							cursorig[0] = cursdest[0];
						}
						break;

					case KEY_DOWN:
						if (cursorig[0] < 2) {
							cursdest[0] = cursorig[0] + 1;
							place_cursor(cursorig, cursdest, Tboard);
							cursorig[0] = cursdest[0];
						}
						break;

					case ' ':
						move(23, 0);
						deleteln();
						check_move = place_chip(cursdest, Tboard);
						switch (check_move) {
							case 0:
								winner = check_winner(Tboard);
								Tboard->movements++;
								break;
							case 1:

								mvprintw(23, (80 - 41) / 2, "YOU CANNOT TAKE A CHIP FROM YOUR OPPONENT");
								break;
							case 2:
								mvprintw(23, (80 - 48) / 2, "YOU CAN'T PLACE A CHIP ON AN OCCUPIED FIELD");
								break;
							case 3:
								mvprintw(23, (80 - 55) / 2, "YOU HAVE ALREADY PICKED UP THE ELEMENT. PLACE IT ON A EMPTY FIELD");
								break;
							case 4:
								mvprintw(23, (80 - 56) / 2, "YOU HAVE ALREADY PLACED ALL YOUR CHIPS ON THE BOARD. MOVE YOUR CHIP");
								break;
							case 5:
								mvprintw(23, (80 - 47) / 2, "SOMETHING WENT WRONG :(");
								break;
						}
						break;
				}
			}

			return winner;
		}
};
int main () {
	int key;
	int exit = 0;
	int winner;
	board Tboard;

	Tboard.x = (80 - XMAX) / 2;
	Tboard.y = 3;

	initscr();
	start_color();
	noecho();
	cbreak();
	curs_set(FALSE);
	keypad(stdscr, TRUE);

	// WDRAŻANIE KOLORÓW I INNE TAKIE TAKIE

	init_pair(0, COLOR_WHITE, COLOR_BLACK);
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);
	init_pair(3, COLOR_BLUE, COLOR_BLACK);
	init_pair(4, COLOR_BLACK, COLOR_CYAN);

	while (!exit) {
		key = 0;
		winner = 0;
		Tboard.player = 1;
		Tboard.previousChip[0] = 3;
		Tboard.previousChip[1] = 3;
		Tboard.chipsInPlay[0] = 0;
		Tboard.chipsInPlay[1] = 0;
		Tboard.movements = 0;

		clear();
		mvprintw(1, (80 - 9) / 2, "");

		Game game;
		game.clean_chips(&Tboard);
		game.render_chip(&Tboard);
		winner = game.main_game(&Tboard);

		if (winner != 0) {
			attron(COLOR_PAIR(4));
			mvprintw(10, (80 - 21) / 2, "CONGRATULATIONS PLAYER %d", winner);
			attron(A_BLINK);
			mvprintw(11, (80 - 12) / 2, "YOU WON!");
			attroff(A_BLINK);
			mvprintw(12, (80 - 29) / 2, "WANNA PLAY AGAIN? (y/n)");
			attroff(COLOR_PAIR(4));

			while (key != 'n' && key != 'y') { /// Dopisz zeby tez szukało z capslockiem bo zapomniales !!!!!
				key = getch();
				if (key == 'n')
					exit = 1;
			}
		} else {
			exit = 1;
		}
	}

	resetty();
	endwin();

	return 0;
}
