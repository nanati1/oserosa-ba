#pragma once
enum Stone { EMPTY, BLACK, WHITE };

class Board {
public:
    Board();

    bool CanPut(int x, int y) const;
    void Put(int x, int y);

    void UpdatePuttable();
    bool IsPuttable(int x, int y) const;


    Stone Get(int x, int y) const;
    Stone GetTurn() const;

    bool HasAnyPuttable() const;
    int CountStone(Stone s) const;
    bool IsGameEnd() const;

    //パス判定
    void Pass();


private:
    Stone board[8][8];
    Stone turn;

    bool puttable[8][8];   // ★ ハイライト情報

    bool In(int x, int y) const;
    Stone Opponent(Stone s) const;
    bool CanFlipDir(int x, int y, int dx, int dy, Stone s) const;
};