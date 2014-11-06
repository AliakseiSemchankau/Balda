#include "bot.h"
#include <algorithm>


Bot::Bot(QObject* parent) :
Player(parent)
{
    MOVES.push_back(std::make_pair(0, 1));
    MOVES.push_back(std::make_pair(0, -1));
    MOVES.push_back(std::make_pair(1, 0));
    MOVES.push_back(std::make_pair(-1, 0));
}


void Bot::connectToDictionary(QObject* dictionary) {
    connect(this, SIGNAL(getDictionary()), dictionary, SLOT(sendDictionary()));
    emit getDictionary();
}
/*
*  This slot realizes the move of Bot and interacts with error signals from other classes
*
*  Strategy
*      @author Aliaksei Semchankau
*  Interface
*      @author Pavel Akhtyamov
*/
void Bot::runProcess() {

    std::vector<std::string> notAllowedWords;
    //std::vector<std::vector<char> > table;

    emit Player::showBoard();

    // takeTable(Table);
    // takeListOfNotAllowedWords(notAllowedWords);


    //ofstream out("output.txt");

    std::vector<Word> Variants = possibleVariants(std::vector<QString>(0));

    /*out << Variants.size() << "\n";
    for (int i = 0; i < Variants.size(); ++i, out << "\n")
        out << Variants[i].possible_word;*/

    //sendAnswer(possibleVariants);

}

bool Bot::notBelong(std::vector<QString> notAllowedWords, QString &checkIn)
{
    for (int i = 0; i<notAllowedWords.size(); ++i)
        if (notAllowedWords[i] == checkIn)
            return false;

    return true;
}

void Bot::dfs(std::vector<Word> &words,
              int x, int y, int curPosition, std::vector<std::vector<bool> > curUsed,
              QString &curString,
              std::vector<std::pair<int, int> > &curCoords, bool usedEmpty)
{
    curUsed[x][y] = true;

    if (borVocabulary.borVertices[curPosition].isLeaf() && usedEmpty)  // Если мы пришли в такую позицию, что слово уже готово. Проверяется использованность хоть какой-то пустой клетки.
    {
        curCoords.push_back(std::make_pair(x, y));
        curString += board[x][y];

        words.push_back(Word(curString, curCoords));

        curString = curString.left((int)curString.size() - 1);
        //curString.pop_back();
        curCoords.pop_back();
    }

    if (board[x][y] == QChar('-') && !usedEmpty)  // Если мы пришли в пустую клетку во время работы дфса, то ставим туда всевозможные буквы и пытаемся продлить слово.
    {
        for (int i = 0; i<32; ++i)
        {
            board[x][y] = getChar(i);
            if (borVocabulary.borVertices[curPosition].findChildren(getChar(i)) != -1)
                dfs(words, x, y, borVocabulary.borVertices[curPosition].findChildren(getChar(i)), curUsed, curString, curCoords, true);
        }
        board[x][y] = QChar('-');
    }
    else
        for (size_t i = 0; i<MOVES.size(); ++i)  // Иначе мы пытаемся пройти в одном из четырех направлений и продлить слово.
        {
        int dx = MOVES[i].first;
        int dy = MOVES[i].second;

        int xx = x + dx;
        int yy = y + dy;

        if (board[xx][yy] == QChar('#'))
            continue;

        if (board[xx][yy] == QChar('-') && usedEmpty)
            continue;

        if (curUsed[xx][yy])
            continue;

        curCoords.push_back(std::make_pair(x, y));
        curString += board[x][y];

        if (board[xx][yy] != QChar('-'))
        {
            if (borVocabulary.borVertices[curPosition].findChildren(board[xx][yy]) != NOT_FOUND)
                dfs(words, xx, yy, borVocabulary.borVertices[curPosition].findChildren(board[xx][yy]), curUsed, curString, curCoords, usedEmpty);
        }
        else
        {
            dfs(words, xx, yy, curPosition, curUsed, curString, curCoords, usedEmpty);
        }

        curString = curString.left((int)curString.size() - 1);
        //curString.pop_back();
        curCoords.pop_back();
    }

    curUsed[x][y] = false;
}

std::vector<Word> Bot::possibleVariants (std::vector<QString> notAllowedWords)
{
    int N, M;
    N = board.size() - 2;
    M = board[0].size() - 2;
    //cerr << N << " " << M << "\n";

    //std::vector<std::string> possibleStrings;
    //std::vector<std::vector<std::pair<int, int> > > coordinates;
    std::vector <Word> words;

    for (int i = 1; i <= N; ++i)
        for (int j = 1; j <= M; ++j)
        {														// Пытаемся запустить дфс из всех клеток таблицы.
        std::vector<std::vector<bool> > curUsed;
        std::vector<std::pair<int, int> > curCoords;
        QString curString;
        curUsed.assign(N + 2, std::vector<bool>(M + 2, false));

        int curPosition = 0;

        if (board[i][j] != QChar('-') && borVocabulary.borVertices[curPosition].findChildren(board[i][j]) != -1)
            dfs(words, i, j, borVocabulary.borVertices[curPosition].findChildren(board[i][j]), curUsed, curString, curCoords, false);
        else
            if (board[i][j] == QChar('-'))
                dfs(words, i, j, PRE_VERTEX, curUsed, curString, curCoords, false);

        }

    std::vector<Word> corrects;

    for (int i = 0; i < words.size(); ++i)
        if (notBelong(notAllowedWords, words[i].possibleWord))
            corrects.push_back(words[i]);



    return corrects;
}


void Bot::setupDictionary(QVector<QString> words) {
    for (int i = 0; i < words.size(); ++i) {
        borVocabulary.addWord(words[i]);
    }
}