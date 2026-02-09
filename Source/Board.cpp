#include "Board.h"

Board::Board() {
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
            board[x][y] = EMPTY;

    board[3][3] = board[4][4] = WHITE;
    board[3][4] = board[4][3] = BLACK;
    turn = BLACK;

    UpdatePuttable();   // ★ 最初のハイライト生成
}

Stone Board::Opponent(Stone s) const {
    return s == BLACK ? WHITE : BLACK;
}

bool Board::In(int x, int y) const {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

bool Board::CanFlipDir(int x, int y, int dx, int dy, Stone s) const {
    int nx = x + dx, ny = y + dy;
    if (!In(nx, ny) || board[nx][ny] != Opponent(s)) return false;

    while (true) {
        nx += dx; ny += dy;
        if (!In(nx, ny) || board[nx][ny] == EMPTY) return false;
        if (board[nx][ny] == s) return true;
    }
}

bool Board::CanPut(int x, int y) const {
    if (!In(x, y) || board[x][y] != EMPTY) return false;

    for (int dx = -1; dx <= 1; dx++)
        for (int dy = -1; dy <= 1; dy++)
            if (dx || dy)
                if (CanFlipDir(x, y, dx, dy, turn))
                    return true;

    return false;
}

void Board::Put(int x, int y) {

    if (!puttable[x][y]) return;

    board[x][y] = turn;

    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if ((dx != 0 || dy != 0) &&
                CanFlipDir(x, y, dx, dy, turn)) {

                int nx = x + dx;
                int ny = y + dy;

                while (board[nx][ny] == Opponent(turn)) {
                    board[nx][ny] = turn;
                    nx += dx;
                    ny += dy;
                }
            }
        }
    }

    turn = Opponent(turn);
    UpdatePuttable();   // ★ 次の手番用
}

void Board::UpdatePuttable()
{
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            puttable[x][y] = false;

            if (board[x][y] != EMPTY) continue;

            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if ((dx != 0 || dy != 0) &&
                        CanFlipDir(x, y, dx, dy, turn)) {
                        puttable[x][y] = true;
                        goto FOUND;
                    }
                }
            }
        FOUND:;
        }
    }
}

bool Board::IsPuttable(int x, int y) const
{
    return puttable[x][y];
}

Stone Board::Get(int x, int y) const {
    return board[x][y];
}

Stone Board::GetTurn() const {
    return turn;
}

bool Board::HasAnyPuttable() const
{
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
            if (puttable[x][y])
                return true;
    return false;
}

int Board::CountStone(Stone s) const
{
    int cnt = 0;
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
            if (board[x][y] == s)
                cnt++;
    return cnt;
}

bool Board::IsGameEnd() const
{
    if (HasAnyPuttable())
        return false;

    // 相手番も確認
    Stone backup = turn;
    const_cast<Board*>(this)->turn = Opponent(turn);
    const_cast<Board*>(this)->UpdatePuttable();

    bool opponentCan = HasAnyPuttable();

    // 元に戻す
    const_cast<Board*>(this)->turn = backup;
    const_cast<Board*>(this)->UpdatePuttable();

    return !opponentCan;
}

void Board::Pass()
{
    turn = Opponent(turn);
    UpdatePuttable();
}
