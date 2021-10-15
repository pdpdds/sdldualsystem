#pragma once

enum player { X_PLAYER = 1, O_PLAYER = -1 };
enum play  { NO_WIN = 0, X_WIN = 1, O_WIN = 2, DRAW = 3 };
enum cell { O = -1, NUL = 0, X = 1 };

class Board
{
public:
	static const int FIELD_SIZE = 10;					// размер по? для игры
private:
	static const int WIN_SIZE = 5;
	cell board[FIELD_SIZE][FIELD_SIZE];				// пото?буде?размер по? для игры ?10 ячеек

	bool searchWin3(player player) const;
	bool searchWin(player player) const;
	bool searchDraw() const;
public:
	Board();
	~Board();

	bool makeMove(int x, int y, player player);
	play isWin() const;
	cell getCell(int x, int y) const;
};

