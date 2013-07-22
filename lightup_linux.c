#include <stdio.h>
#include <stdlib.h>

#define BOARD_SIZE 1200

int row, col, size, step, guess;
char board[BOARD_SIZE];
char tempBoard[BOARD_SIZE];

void printBoard()
{
    puts("---------------------LIGHT-UP---------------------");
    printf("step: %d\n", step);
    int i;
    for (i = 0; i < size; i++) {
        if (board[i] == '!')
            printf("%2c", 'x');
        else if (board[i] == '+')
            printf("%2c", '!');
        else if (board[i] == ' ')
            printf("%2c", '.');
        else if (board[i] == '/')
            printf("%2c", '#');
        else if (board[i] == '*')
            printf("%2c", ' ');
        else
            printf("%2c", board[i]);
        if (i % col == col-1)
            puts("");
    }
}

void copyBoard()
{
    int i;
    for (i = 0; i < size; i++)
        tempBoard[i] = board[i];
}

void restoreBoard()
{
    int i;
    for (i = 0; i < size; i++)
        board[i] = tempBoard[i];
}

int stillNeed(char *board, int pos, int offset_row, int offset_col)
{
    int have = 0;
    if (pos+offset_row >= 0 && pos+offset_row < size && board[pos+offset_row] == '+')
        have++;
    if ((((pos%col) && offset_col < 0) || ((pos+1)%col && offset_col > 0)) && board[pos+offset_col] == '+')
        have++;
    return board[pos]-'0'-have;
}

int enough(char *board, int pos, int offset_row, int offset_col)
{
    int have = 0, space = 0, temp[2] = {0};
    if (pos+offset_row >= 0 && pos+offset_row < size) {
        if (board[pos+offset_row] == '+')
            have++;
        else if (board[pos+offset_row] == ' ')
            temp[space++] = pos+offset_row;
    }
    if (((pos%col) && offset_col < 0) || ((pos+1)%col && offset_col > 0)) {
        if (board[pos+offset_col] == '+')
            have++;
        else if (board[pos+offset_col] == ' ')
            temp[space++] = pos+offset_col;
    }
    if ((board[pos]-'0'-have) == 1 && space) {
        int i;
        for (i = 0; i < space; i++)
            board[temp[i]] = '!';
        return 1;
    }
    return 0;
}

void lit(char *board, int pos)
{
    board[pos] = '+';
    int i;
    for (i = pos-col; i >= 0 && board[i] < '/'; i-=col)
        board[i] = '*';
    for (i = pos-1; (i+1) % col && board[i] < '/'; i--)
        board[i] = '*';
    for (i = pos+1; i % col && board[i] < '/'; i++)
        board[i] = '*';
    for (i = pos+col; i < size && board[i] < '/'; i+=col)
        board[i] = '*';
}

int check(char *board)
{
    int i, j, k, changed = 0, completed = 1, temp[4] = {0};
    for (i = 0; i < size; i++) {
        if (board[i] < '#') {
            completed = 0;
            int source = 0;
            for (j = i; source < 3 && j >= 0 && board[j] < '/'; j-=col) // scan up (including itself)
                if (board[j] == ' ')
                    temp[source++] = j;
            for (j = i+col; source < 3 && j < size && board[j] < '/'; j+=col) // scan down
                if (board[j] == ' ')
                    temp[source++] = j;
            for (j = i-1; source < 3 && (j+1) % col && board[j] < '/'; j--) // scan left
                if (board[j] == ' ')
                    temp[source++] = j;
            for (j = i+1; source < 3 && j % col && board[j] < '/'; j++) // scan right
                if (board[j] == ' ')
                    temp[source++] = j;
            if (!source)
                return -1;
            else if (source == 1) { // if can be illuminate form only one bulb, then place a bulb
                lit(board, temp[0]);
                changed = 1;
            }
            else if (board[i] == '!' && source == 2 && abs(i-temp[0]) == col && abs(i-temp[1]) == 1 && board[temp[0]+temp[1]-i] == ' ') {
                board[temp[0]+temp[1]-i] = '!';
                changed = 1;
            }
        }
        else if (board[i] > '/') {
            int space = 0, have = 0, need = board[i]-'0';
            if (i/col) {
                if (board[i-col] == ' ')
                    temp[space++] = i-col;
                else if (board[i-col] == '+')
                    have++;
            }
            if (i%col) {
                if (board[i-1] == ' ')
                    temp[space++] = i-1;
                else if (board[i-1] == '+')
                    have++;
            }
            if ((i+1) % col) {
                if (board[i+1] == ' ')
                    temp[space++] = i+1;
                else if (board[i+1] == '+')
                    have++;
            }
            if ((i/col) < row-1) {
                if (board[i+col] == ' ')
                    temp[space++] = i+col;
                else if (board[i+col] == '+')
                    have++;
            }
            if (have > need || space+have < need)
                return -1;
            else if (space) {
                if (space+have == need)
                    for (changed = 1, j = 0; j < space; j++)
                        lit(board, temp[j]);
                else if (have == need)
                    for (changed = 1, j = 0; j < space; j++)
                        board[temp[j]] = '!';
                else if (space+have == need+1)
                    for (j = 0; j < space-1; j++)
                        for (k = j+1; k < space; k++) {
                            if (board[temp[j]+temp[k]-i] == ' ') {
                                changed = 1;
                                board[temp[j]+temp[k]-i] = '!';
                            }
                            else if ((temp[j]+temp[k]-i) != i && board[temp[j]+temp[k]-i] > '0') {
                                if (space == 2 && board[temp[j]+temp[k]-i] < '3')
                                    changed |= enough(board, temp[j]+temp[k]-i, temp[j]-i, temp[k]-i); // spaces are enough to choose
                                else if (space > 2 && stillNeed(board, temp[j]+temp[k]-i, temp[j]-i, temp[k]-i) == 1) {
                                    int l;
                                    for (l = 0; l < space; l++) {
                                        if (l == j || l == k)
                                                continue;
                                        if (board[temp[l]] == ' ') {
                                            changed = 1;
                                            lit(board, temp[l]);
                                        }
                                    }
                                }
                            }
                        }
            }
        }
    }
    if (!changed)
        return completed<<1;
    return 1;
}

int findNext(int now)
{
    int i;
    for (i = now+1; i < size; i++)
        if (board[i] == ' ')
            return i;
}

int main(int argc, char *argv[])
{
    int i;
    if (argc < 2)
        exit(1);
    FILE *input = fopen(argv[1], "r");
    if (!input)
        exit(1);
    fscanf(input, "%d%d", &col, &row);
    size = row*col;
    for (i = 0; i < size; i++) {
        if ((board[i] = fgetc(input)) > 10)
            continue;
        if (board[i] == '\n')
            board[i] = fgetc(input);
        else if (board[i] == EOF)
            exit(1);
    }
    fclose(input);

    int result;
    while (1) {
        step++;
        if ((result = check(board)) == 0) {
            guess++;
            int pos = -1;
            while (1) {
                copyBoard();
                pos = findNext(pos);
                lit(tempBoard, pos);
                while((result = check(tempBoard)) == 1);
                if (result == -1) {
                    board[pos] = '!';
                    break;
                }
                else if (result == 2) {
                    restoreBoard();
                    break;
                }
            }
        }
        if (result == 2)
            break;
    }

    printBoard();
    printf("Total guesses: %d\n", guess);
    return 0;
}
