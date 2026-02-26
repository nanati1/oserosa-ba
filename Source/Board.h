#pragma once
#include <string>

enum Stone
{
    EMPTY,
    BLACK,
    WHITE
};

class Board
{
public:
    Board();

    // 文字列から盤面設定
    void SetFromString(const std::string& s);

    // 石を置く
    void Put(int x, int y);

    // 置けるか
    bool CanPut(int x, int y) const;

    // 置ける場所更新
    void UpdatePuttable();

    // 置ける場所か
    bool IsPuttable(int x, int y) const;

    // 石取得
    Stone Get(int x, int y) const;

    // 手番取得
    Stone GetTurn() const;

    // 置ける場所ある？
    bool HasAnyPuttable() const;

    // ゲーム終了？
    bool IsGameEnd() const;

    // 石カウント
    int CountStone(Stone s) const;

    // パス
    void Pass();

private:
    Stone board[8][8];
    bool puttable[8][8];
    Stone turn;

    // 相手取得
    Stone Opponent(Stone s) const;

    // 範囲内？
    bool In(int x, int y) const;

    // 方向チェック
    bool CanFlipDir(int x, int y, int dx, int dy, Stone s) const;
};