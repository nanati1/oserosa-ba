#include "Board.h"

// =====================
// コンストラクタ
// =====================
Board::Board() {
    SetFromString(
        "........"
        "........"
        "........"
        "...WB..."
        "...BW..."
        "........"
        "........"
        "........"
    );

    turn = BLACK;
    UpdatePuttable();
}

// =====================
// 文字列 → 盤面
// =====================
void Board::SetFromString(const std::string& s)
{
    if (s.size() < 64) return;

    int i = 0;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            char c = s[i++];

            if (c == 'B') board[x][y] = BLACK;
            else if (c == 'W') board[x][y] = WHITE;
            else board[x][y] = EMPTY;
        }
    }

    // ★これ超重要
    UpdatePuttable();
}

// =====================
// 相手
// =====================
Stone Board::Opponent(Stone s) const {
    return s == BLACK ? WHITE : BLACK;
}

// =====================
// 範囲チェック
// =====================
bool Board::In(int x, int y) const {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

// =====================
// ひっくり返せるか（方向）
// =====================
bool Board::CanFlipDir(int x, int y, int dx, int dy, Stone s) const {
    int nx = x + dx, ny = y + dy;
    if (!In(nx, ny) || board[nx][ny] != Opponent(s)) return false;

    while (true) {
        nx += dx;
        ny += dy;

        if (!In(nx, ny) || board[nx][ny] == EMPTY) return false;
        if (board[nx][ny] == s) return true;
    }
}

// =====================
// 置けるか
// =====================
bool Board::CanPut(int x, int y) const {
    if (!In(x, y) || board[x][y] != EMPTY) return false;

    for (int dx = -1; dx <= 1; dx++)
        for (int dy = -1; dy <= 1; dy++)
            if (dx || dy)
                if (CanFlipDir(x, y, dx, dy, turn))
                    return true;

    return false;
}

// =====================
// 石を置く
// =====================
void Board::Put(int x, int y) {

    // ★ここ重要（安全）
    if (!CanPut(x, y)) return;

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

    // 手番交代
    turn = Opponent(turn);

    // 次の置ける場所更新
    UpdatePuttable();
}

// =====================
// 置ける場所更新
// =====================
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

// =====================
// 置けるか（キャッシュ）
// =====================
bool Board::IsPuttable(int x, int y) const
{
    return puttable[x][y];
}

// =====================
// 取得
// =====================
Stone Board::Get(int x, int y) const {
    return board[x][y];
}

Stone Board::GetTurn() const {
    return turn;
}

// =====================
// 置ける場所ある？
// =====================
bool Board::HasAnyPuttable() const
{
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
            if (puttable[x][y])
                return true;
    return false;
}

// =====================
// 石数カウント
// =====================
int Board::CountStone(Stone s) const
{
    int cnt = 0;
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
            if (board[x][y] == s)
                cnt++;
    return cnt;
}

// =====================
// 終了判定
// =====================
bool Board::IsGameEnd() const
{
    if (HasAnyPuttable())
        return false;

    Stone backup = turn;

    // 相手確認
    const_cast<Board*>(this)->turn = Opponent(turn);
    const_cast<Board*>(this)->UpdatePuttable();

    bool opponentCan = HasAnyPuttable();

    // 戻す
    const_cast<Board*>(this)->turn = backup;
    const_cast<Board*>(this)->UpdatePuttable();

    return !opponentCan;
}

// =====================
// パス
// =====================
void Board::Pass()
{
    turn = Opponent(turn);
    UpdatePuttable();
}