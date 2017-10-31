#include "header.hpp"

int create_menu(WINDOW * window, vector<string> &list_choices, string header, string guide, bool upper_present)
//generowanie dowolnego menu
{
    WINDOW *menu_window, *list_window, *current_window;
    ITEM **menu_items, **list_items;
    MENU *menu, *lista, *current_menu;
    bool upper_active = false;
    int n = list_choices.size(), res = -1, c, j;
    int height = LINES-4, width = COLS-2, starty = (LINES-height)/2, startx = (COLS-width)/2, shifty, shiftx=5;
    char tmp[n][256];

    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    init_pair(3, COLOR_BLUE, COLOR_WHITE);
    init_pair(4, COLOR_BLACK, COLOR_CYAN);
    init_pair(5, COLOR_BLACK, COLOR_WHITE);

    wclear(window);
    wbkgd(window, COLOR_PAIR(4));
    box(window, 0, 0);
    keypad(window, TRUE);

    int len = header.length();
    wattron(window, A_BOLD);
    mvwprintw(window, 1, (COLS-len)/2,"%s", header.c_str());
    wattroff(window, A_BOLD);

    if(!upper_present)
    {
        mvwaddch(window, 2, 0, ACS_LTEE);
        mvwhline(window, 2, 1, ACS_HLINE, COLS-2);
        mvwaddch(window, 2, COLS - 1, ACS_RTEE);
    }

    if (guide.length()!=0)
        mvwprintw(window, 4, 4, "%s", guide.c_str());

//menu listy
    list_items = (ITEM**)calloc(n+1, sizeof(ITEM*));
    for (int i=0; i<n; i++)
    {
        strcpy(tmp[i], list_choices[i].c_str());
        list_items[i] = new_item(tmp[i], "");
    }
    list_items[n] = (ITEM*)NULL;

    lista = new_menu(list_items);

    if(guide.length()==0)
        shifty = 2;
    else
        shifty = 4;

    list_window = subwin(window, height-shifty, width-shiftx, starty+shifty, startx+(shiftx/2));
    set_menu_win(lista, list_window);
    set_menu_sub(lista, list_window);

    set_menu_back(lista, COLOR_PAIR(4));
    set_menu_fore(lista, COLOR_PAIR(2));
    set_menu_mark(lista, " ");

    refresh();
    post_menu(lista);

//gorne menu
    if(upper_present)
    {
        vector<string> menu_choices {"Szukaj", "Sortuj", "Filtruj", "Nowy", "Wroc"};
        j = menu_choices.size();

        menu_items = (ITEM**)calloc(j+1, sizeof(ITEM*));
        for (int i=0; i<j; i++)
        {
            menu_items[i] = new_item(menu_choices[i].c_str(), "");
        }
        menu_items[j] = (ITEM*)NULL;

        menu = new_menu(menu_items);

        menu_window = subwin(window, 1, width, starty, startx);
        //keypad(menu_window, TRUE);
        set_menu_win(menu, menu_window);
        set_menu_sub(menu, menu_window);

        wbkgd(menu_window, COLOR_PAIR(1));
        set_menu_back(menu, COLOR_PAIR(1));
        set_menu_fore(menu, COLOR_PAIR(1));
        set_menu_format(menu, 1, 5);
        set_menu_mark(menu, "  ");
        set_menu_spacing(menu, 0, 0, 3);

        refresh();
        post_menu(menu);
    }
    wrefresh(window);

    current_menu = lista;
    current_window = list_window;

//sterowanie
    while((c = wgetch(window))!=KEY_F(1))
    {
        if(upper_active)
        {
            current_menu = menu;
            current_window = menu_window;
        }
        else if(upper_present&&!upper_active)
        {
            current_menu = lista;
            current_window = list_window;
        }

        switch(c)
        {
        case KEY_UP:
            if(!upper_active)
                menu_driver(current_menu, REQ_PREV_ITEM);
            break;
        case KEY_LEFT:
            if(upper_active)
                menu_driver(current_menu, REQ_PREV_ITEM);
            break;
        case KEY_DOWN:
            if(!upper_active)
                menu_driver(current_menu, REQ_NEXT_ITEM);
            break;
        case KEY_RIGHT:
            if(upper_active)
                menu_driver(current_menu, REQ_NEXT_ITEM);
            break;
        case '\t':
        {
            if(upper_present)
            {
                if(upper_active)
                {
                    set_menu_fore(menu, COLOR_PAIR(1));
                    wrefresh(menu_window);
                    set_menu_fore(lista, COLOR_PAIR(2));
                    wrefresh(list_window);
                }
                else
                {
                    set_menu_fore(menu, COLOR_PAIR(2));
                    wrefresh(menu_window);
                    set_menu_fore(lista, COLOR_PAIR(5));
                    wrefresh(list_window);
                }
                upper_active = !upper_active;
            }
            break;
        }
        case 10:
            res = item_index(current_item(current_menu))+1;
            break;
        }
        wrefresh(current_window);
        if (res!=-1) break;
    }
    if (upper_present)
    {
        for (int i=0; i<j+1; i++)
            free_item(menu_items[i]);

        free_menu(menu);
    }

    for (int i=0; i<n+1; i++)
        free_item(list_items[i]);

    free_menu(lista);

    if(upper_active)
        return (-1*res);
    else
        return res;
}

int dialog(vector<string> &choices, string header, string text)
//tworzy dowolne okno dialogu
{
    WINDOW *pop_up;
    MENU* menu;
    ITEM ** items;

    int len = find_longest(choices);
    int n = choices.size();

    items = (ITEM **)calloc(n+1, sizeof(ITEM *));

    for (int i=0; i<n; i++)
        items[i]=new_item(choices[i].c_str(), "");
    items[n] = (ITEM *)NULL;

    menu = new_menu(items);
    int height = 7+n, width = 25+len, starty = (LINES-height)/2, startx = (COLS-width)/2;

    init_pair(1, COLOR_WHITE, COLOR_RED);
    init_pair(2, COLOR_BLUE, COLOR_WHITE);

    pop_up = newwin(height, width, starty, startx);
    set_menu_win(menu, pop_up);
    set_menu_sub(menu, derwin(pop_up, 2+n, 5+len, height-(2+n), width/2-len));
    keypad(pop_up, TRUE);
    refresh();

    wbkgd(stdscr, COLOR_PAIR(2));
    wbkgd(pop_up, COLOR_PAIR(1));
    box(pop_up, 0, 0);
    mvwaddch(pop_up, 2, 0, ACS_LTEE);
    mvwhline(pop_up, 2, 1, ACS_HLINE, width-2);
    mvwaddch(pop_up, 2, width-1, ACS_RTEE);
    set_menu_back(menu, COLOR_PAIR(1));

    mvwprintw(pop_up, 1, (width-5)/2,"%s", header.c_str());
    mvwprintw(pop_up, 3, 2, "%s", text.c_str());

    post_menu(menu);
    wrefresh(pop_up);

    int x=-1, c;

    while((c = wgetch(pop_up)) != KEY_F(1))
    {
        switch(c)
        {
        case KEY_UP:
            menu_driver(menu, REQ_PREV_ITEM);
            break;
        case KEY_DOWN:
            menu_driver(menu, REQ_NEXT_ITEM);
            break;
        case 10:
            x = item_index(current_item(menu));
            break;
        }
        if (x!=-1) break;
        wrefresh(pop_up);
    }

    for (int i=0; i<n+1; i++) free_item(items[i]);
    free_menu(menu);
    delwin(pop_up);

    return x;
}

int find_longest(vector<string> &data)
{
    unsigned int len=0, j=0;

    for (vector<string>::iterator i=data.begin(); i!=data.end(); i++, j++)
        if (data[j].length()>len)
            len = data[j].length();

    return len;
}

int init()
{
//inicjalizacja curses

    if(!initscr())
        return -1;
    start_color();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    return 0;
}

int menu_main(WINDOW* window, vector<Kategoria*> &kategorie, vector<Ksiazka*>& ksiazki, vector<Klient*> &klienci)
{
    vector<string> opcje {"Przegladaj ksiazki", "Przegladaj klientow", "Przegladaj kategorie", "Zapisz", "Wyjdz"};
    int res, x=0;

    do
    {
        res = create_menu(window, opcje, "MENU", "", false), x;
        x = 0;

        switch (res)
        {
        case 1:
            x = menu_ksiazki(window, ksiazki, kategorie, klienci);
            break;

        case 2:
            x = menu_klienci(window, klienci);
            break;

        case 3:
            x = menu_kategorie(window, kategorie);
            break;

        case 4:
            zapisz(kategorie, ksiazki, klienci);
            break;

        case 5:
            break;

        case -1:
            x = 3;
            break;

        default:
            menu_main(window, kategorie, ksiazki, klienci);
            break;
        }
    }while (res!=-1&&res!=5);

    return 0;
}

int menu_kategorie(WINDOW * window, vector <Kategoria*>& kategorie)
{
    vector<string> list_choices;

    for (int i=0; i<kategorie.size(); i++)
        list_choices.push_back(kategorie[i]->new_choice());

    vector<string> menu_choices {"Edytuj", "Usun", "Wroc"};

    int res=0, x=0, check;

    do
    {
        res = create_menu(window, list_choices, "KATEGORIE", "ID   Symbol  Nazwa", true);

        if (res>0)
            x = item_details(window, kategorie[res-1], "KATEGORIE", menu_choices);
    } while (res!=-5);


    return res;
}

int menu_klienci(WINDOW * window, vector <Klient*> &klienci)
{

    vector<string> list_choices;
    for (int i=0; i<klienci.size(); i++)
        list_choices.push_back(klienci[i]->new_choice());

    vector<string> menu_choices {"Edytuj", "Usun", "Wroc"};

    int res, x=0, check;

    do
    {
        res = create_menu(window, list_choices, "Klienci", "ID   Imie                 Nazwisko", true);

        if(res>0)
            x = item_details(window, klienci[res-1], "KLIENCI", menu_choices);
    } while (res!=-5);

    return 0;
}

int menu_ksiazki(WINDOW * window, vector <Ksiazka*> &ksiazki, vector <Kategoria*> &kategorie, vector <Klient*> klienci)
{
    vector<string> list_choices;

    for (int i=0; i<ksiazki.size(); i++)
        list_choices.push_back(ksiazki[i]->new_choice());

    vector<string> menu_choices {"Edytuj", "Usun", "Wroc"};

    int res=0, x=0;

    do
    {
        res = create_menu(window, list_choices, "KSIAZKI", "ID   Autor                Tytul", true);

        if(res>0)
            x = item_details(window, ksiazki[res-1], "KSIAZKI", menu_choices);
    } while (res!=-5);

    return 0;
}

void zapisz(vector<Kategoria*> &kategorie, vector<Ksiazka*> &ksiazki, vector<Klient*> &klienci)
{
    int res = data_export(kategorie, ksiazki, klienci);
    vector<string> ok {"OK"};
    string info;

    switch(res)
    {
    case 0:
        info = "Zapisano pomyslnie";
        break;
    case -1:
        info = "Blad pliku \"kategorie\"";
        break;
    case -2:
        info = "Blad pilku \"ksiazki\"";
        break;
    case -3:
        info = "Blad pliku \"klienci\"";
        break;
    default:
        info = "Cos sie stalo";
        break;
    }

    dialog(ok, "ZAPISZ", info);
}
