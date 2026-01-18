/**
*
* Solution to course project # 8
* Introduction to programming course
* Faculty of Mathematics and Informatics of Sofia University
* Winter semester 2025/2026
*
* @author Dilyan Dimitrov
* @idnumber 7MI0600602
* @compiler VC
*
* Main file for 2048 Game
*
*/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>

const int MIN_BOARD_SIZE = 4;
const int MAX_BOARD_SIZE = 10;
const int CELL_WIDTH = 5;

const int SPAWN_VAL_SMALL = 2;
const int SPAWN_VAL_BIG = 4;
const int PROBABILITY_BASE = 10;

const int MAX_NAME_LEN = 100;
const int MAX_RECORDS_BUFFER = 100;
const int TOP_PLAYERS_COUNT = 5;
const int FILENAME_BUFFER_SIZE = 50;
const int INPUT_IGNORE_LIMIT = 1000;

const char FILE_PREFIX[] = "leaderboard_";
const char FILE_EXT[] = ".txt";

const char KEY_UP = 'w';
const char KEY_DOWN = 's';
const char KEY_LEFT = 'a';
const char KEY_RIGHT = 'd';
const char KEY_QUIT = 'q';

const char MENU_OPT_START = '1';
const char MENU_OPT_LEADERBOARD = '2';
const char MENU_OPT_EXIT = '3';


int myStrlen(const char* str)
{
    if (!str)
    {
        return 0;
    }
    int len = 0;
    while (*str)
    {
        len++;
        str++;
    }
    return len;
}

void myStrcopy(char* dest, const char* source)
{
    if (!source)
    {
        return;
    }
    while (*source)
    {
        *dest = *source;
        dest++;
        source++;
    }
    *dest = '\0';
}

void myStrcat(char* dest, const char* source)
{
    if (!dest || !source)
    {
        return;
    }
    int firstLen = myStrlen(dest);
    dest += firstLen;
    myStrcopy(dest, source);
}

void intToString(int num, char* str)
{
    if (num == 0)
    {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    int i = 0;
    int tempNum = num;
    while (tempNum > 0)
    {
        str[i++] = (tempNum % 10) + '0';
        tempNum /= 10;
    }
    str[i] = '\0';

    for (int j = 0; j < i / 2; j++)
    {
        char temp = str[j];
        str[j] = str[i - 1 - j];
        str[i - 1 - j] = temp;
    }
}

void getFileName(int n, char* buffer)
{
    char numStr[10];
    intToString(n, numStr);

    buffer[0] = '\0';
    myStrcat(buffer, FILE_PREFIX);
    myStrcat(buffer, numStr);
    myStrcat(buffer, "x");
    myStrcat(buffer, numStr);
    myStrcat(buffer, FILE_EXT);
}

void copyBoard(int source[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int dest[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            dest[i][j] = source[i][j];
        }
    }
}

bool areBoardsEqual(int board1[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int board2[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (board1[i][j] != board2[i][j])
            {
                return false;
            }
        }
    }
    return true;
}

void spawnNewNumber(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int n)
{
    int emptyX[MAX_BOARD_SIZE * MAX_BOARD_SIZE];
    int emptyY[MAX_BOARD_SIZE * MAX_BOARD_SIZE];
    int count = 0;

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (board[i][j] == 0)
            {
                emptyX[count] = i;
                emptyY[count] = j;
                count++;
            }
        }
    }

    if (count > 0)
    {
        int idx = rand() % count;
        int val = (rand() % PROBABILITY_BASE == 0) ? SPAWN_VAL_BIG : SPAWN_VAL_SMALL;
        board[emptyX[idx]][emptyY[idx]] = val;
    }
}

int calculateScore(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int n)
{
    int sum = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            sum += board[i][j];
        }
    }
    return sum;
}

void moveLeft(int line[], int n)
{
    int temp[MAX_BOARD_SIZE] = { 0 };
    int count = 0;

    for (int i = 0; i < n; i++)
    {
        if (line[i] != 0)
        {
            temp[count++] = line[i];
        }
    }

    for (int i = 0; i < count - 1; i++)
    {
        if (temp[i] == temp[i + 1])
        {
            temp[i] *= 2;
            temp[i + 1] = 0;
            i++;
        }
    }

    int finalLine[MAX_BOARD_SIZE] = { 0 };
    int finalCount = 0;
    for (int i = 0; i < count; i++)
    {
        if (temp[i] != 0)
        {
            finalLine[finalCount++] = temp[i];
        }
    }

    for (int i = 0; i < n; i++)
    {
        line[i] = finalLine[i];
    }
}

void mySwap(int& a, int& b)
{
    a += b;
    b = a - b;
    a -= b;
}

void reverseLine(int line[], int n)
{
    int mid = n / 2;

    for (int i = 0; i < mid; i++)
    {
        mySwap(line[i], line[n - 1 - i]);
    }
}

void applyMove(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int n, char move)
{
    if (move == KEY_LEFT)
    {
        for (int i = 0; i < n; i++)
        {
            moveLeft(board[i], n);
        }
    }
    else if (move == KEY_RIGHT)
    {
        for (int i = 0; i < n; i++)
        {
            reverseLine(board[i], n);
            moveLeft(board[i], n);
            reverseLine(board[i], n);
        }
    }
    else if (move == KEY_UP)
    {
        for (int j = 0; j < n; j++)
        {
            int col[MAX_BOARD_SIZE];

            for (int i = 0; i < n; i++)
            {
                col[i] = board[i][j];;
            }

            moveLeft(col, n);
            for (int i = 0; i < n; i++)
            {
                board[i][j] = col[i];
            }
        }
    }
    else if (move == KEY_DOWN)
    {
        for (int j = 0; j < n; j++)
        {
            int col[MAX_BOARD_SIZE];
            for (int i = 0; i < n; i++)
            {
                col[i] = board[i][j];
            }
            reverseLine(col, n);
            moveLeft(col, n);
            reverseLine(col, n);
            for (int i = 0; i < n; i++)
            {
                board[i][j] = col[i];
            }
        }
    }
}

bool isGameOver(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (board[i][j] == 0) 
                return false;

            if (j + 1 < n && board[i][j] == board[i][j + 1])
                return false;

            if (i + 1 < n && board[i][j] == board[i + 1][j])
                return false;
        }
    }
    return true;
}

void sortLeaderboard(char names[][MAX_NAME_LEN], int scores[], int count)
{
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            if (scores[j] < scores[j + 1])
            {
                mySwap(scores[j], scores[j + 1]);
                char tempName[MAX_NAME_LEN];
                myStrcopy(tempName, names[j]);
                myStrcopy(names[j], names[j + 1]);  
                myStrcopy(names[j + 1], tempName);
            }
        }
    }
}

void updateLeaderboard(int n, const char* playerName, int score)
{
    char filename[FILENAME_BUFFER_SIZE];
    getFileName(n, filename);

    char allNames[MAX_RECORDS_BUFFER][MAX_NAME_LEN];
    int allScores[MAX_RECORDS_BUFFER];
    int count = 0;

    std::ifstream inFile(filename);
    if (inFile.is_open())
    {
        while (count < MAX_RECORDS_BUFFER && inFile >> allNames[count] >> allScores[count])
        {
            count++;
        }
        inFile.close();
    }

    if (count < MAX_RECORDS_BUFFER)
    {
        myStrcopy(allNames[count], playerName);
        allScores[count] = score;
        count++;
    }

    sortLeaderboard(allNames, allScores, count);
    std::ofstream outFile(filename);
    if (outFile.is_open())
    {
        int limit = (count < TOP_PLAYERS_COUNT) ? count : TOP_PLAYERS_COUNT;
        for (int i = 0; i < limit; i++)
        {
            outFile << allNames[i] << " " << allScores[i] << std::endl;
        }
        outFile.close();
    }
}

void showLeaderboard()
{
    int n;
    std::cout << "\n--- LEADERBOARD ---\n";
    std::cout << "Enter board size (" << MIN_BOARD_SIZE << "-" << MAX_BOARD_SIZE << "): ";

    while (!(std::cin >> n) || n < MIN_BOARD_SIZE || n > MAX_BOARD_SIZE)
    {
        std::cout << "Invalid size! Try again: ";
        std::cin.clear();
        std::cin.ignore(INPUT_IGNORE_LIMIT, '\n');
    }

    char filename[FILENAME_BUFFER_SIZE];
    getFileName(n, filename);

    std::ifstream inFile(filename);
    if (!inFile.is_open())
    {
        std::cout << "No data found for size " << n << "x" << n << ".\n";
    }
    else
    {
        std::cout << "\nTOP " << TOP_PLAYERS_COUNT << " (" << n << "x" << n << "):\n";
        std::cout << "----------------------\n";
        char tName[MAX_NAME_LEN];
        int tScore;
        int rank = 1;
        while (inFile >> tName >> tScore)
        {
            std::cout << rank << ". " << std::left << std::setw(15) << tName << " - " << tScore << "\n";
            rank++;
        }
        std::cout << "----------------------\n";
        inFile.close();
    }
    std::cout << "\nPress Enter to return...";
    std::cin.ignore();
    std::cin.get();
}

void clearScreen()
{
    system("cls");
}

void printBoard(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int n, int score)
{
    clearScreen();
    std::cout << "2048 GAME | Score: " << score << "\n\n";

    for (int i = 0; i < n; ++i)
    {
        std::cout << "|";
        for (int j = 0; j < n; ++j)
        {
            if (board[i][j] == 0)
            {
                std::cout << std::setw(CELL_WIDTH) << ".";
            }

            else
            {
                std::cout << std::setw(CELL_WIDTH) << board[i][j];
            }
        }
        std::cout << " |\n\n";
    }
    std::cout << "Controls: " << KEY_UP << "," << KEY_LEFT << "," << KEY_DOWN << "," << KEY_RIGHT << ", " << KEY_QUIT << "\n";
}

int getBoardSize()
{
    int n;
    do
    {
        std::cout << "Enter board size (" << MIN_BOARD_SIZE << "-" << MAX_BOARD_SIZE << "): ";
        if (!(std::cin >> n))
        {
            std::cin.clear();
            std::cin.ignore(INPUT_IGNORE_LIMIT, '\n');
            n = 0;
        }
    } while (n < MIN_BOARD_SIZE || n > MAX_BOARD_SIZE);

    std::cin.ignore(INPUT_IGNORE_LIMIT, '\n');

    return n;
}

void playGame()
{
    char playerName[MAX_NAME_LEN];
    std::cout << "Enter nickname: ";
    std::cin >> playerName;

    int n = getBoardSize();
    int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE] = { 0 };

    spawnNewNumber(board, n);
    spawnNewNumber(board, n);

    while (true)
    {
        int currentScore = calculateScore(board, n);
        printBoard(board, n, currentScore);

        if (isGameOver(board, n))
        {
            std::cout << "\nGAME OVER!\nFinal Score: " << currentScore << std::endl;
            updateLeaderboard(n, playerName, currentScore);
            break;
        }

        char inputBuffer[100];
        char move;

        while (true)
        {
            std::cout << "Move: ";
            std::cin.getline(inputBuffer, 100);

            if (myStrlen(inputBuffer) != 1)
            {
                std::cout << "Error: Please enter exactly one character!\n";
            }
            else
            {
                move = inputBuffer[0];
                break;
            }
        }

        if (move == KEY_QUIT)
            break;

        int oldBoard[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
        copyBoard(board, oldBoard, n);

        applyMove(board, n, move);

        if (!areBoardsEqual(board, oldBoard, n))
        {
            spawnNewNumber(board, n);
        }
    }
    std::cout << "\nPress Enter to return...";
    std::cin.get();
}

void showMenu()
{
    while (true)
    {
        clearScreen();
        std::cout << "=== 2048 GAME ===\n";
        std::cout << MENU_OPT_START << ". Start Game\n";
        std::cout << MENU_OPT_LEADERBOARD << ". Leaderboard\n";
        std::cout << MENU_OPT_EXIT << ". Exit\n";
        std::cout << "Select option: ";

        char choice;
        std::cin >> choice;

        if (choice == MENU_OPT_START)
        {
            playGame();
        }

        else if (choice == MENU_OPT_LEADERBOARD)
        {
            showLeaderboard();
        }

        else if (choice == MENU_OPT_EXIT)
            return;
    }
}

int main()
{
    srand((unsigned)time(0));
    showMenu();
    return 0;
}