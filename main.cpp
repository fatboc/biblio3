#include "header.hpp"


int main()
{
    vector<Ksiazka*> ksiazki;
    vector<Klient*> klienci;
    vector<Kategoria*> kategorie;

    try
    {
        if (data_import(kategorie, klienci, ksiazki)!=0) throw "Blad eksportu.\n";
        if (init()!=0) throw "Nie mozna uruchomic curses.\n";
    }

    catch (const char * ex)
    {
        cout << ex;
        return -1;
    }

    WINDOW * window = newwin(LINES-1, COLS, 0, 0);

    menu_main(window, kategorie, ksiazki, klienci);

    delwin(window);
    endwin();

    return 0;
}
