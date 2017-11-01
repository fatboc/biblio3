#include "header.hpp"

void elo()
{
    vector<string> ok{"OK"};
    dialog(ok, "ELO", "elo.");
}

int create_menu(WINDOW * window, vector<string> &list_choices, string header, string guide, bool upper_present)
//generowanie dowolnego menu
{
    WINDOW *menu_window, *list_window, *current_window;
    ITEM **menu_items, **list_items;
    MENU *menu, *lista, *current_menu;
    bool upper_active = false;
    int n = list_choices.size(), res = -1, c, j;
    int height = LINES-4, width = COLS-2, starty = (LINES-height)/2, startx = (COLS-width)/2, shifty, shiftx=5;

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
        list_items[i] = new_item(list_choices[i].c_str(), "");
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
    int res;

    do
    {
        res = create_menu(window, opcje, "MENU", "", false);

        switch (res)
        {
        case 1:
            menu_ksiazki(window, ksiazki, kategorie, klienci);
            break;

        case 2:
            menu_klienci(window, klienci);
            break;

        case 3:
            menu_kategorie(window, kategorie);
            break;

        case 4:
            zapisz(kategorie, ksiazki, klienci);
            break;

        case 5:
            break;

        case -1:
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

    vector<string> menu_choices {"Edytuj", "Usun", "Wroc"}, ok {"OK"};

    int res=0, skip=0;

    do
    {
        if (skip==0)
            res = create_menu(window, list_choices, "KATEGORIE", "ID   Symbol  Nazwa", true);
        else
            skip=0;

        if (res>0)
            item_details(window, kategorie[res-1], "KATEGORIE", menu_choices);

        else
            switch(res)
            {
            case -1:
            {
                string text = search_form("Wprowadz wyszukiwana fraze:");
                if (text.size()!=0)
                {
                    int j=0, x=-1;
                    vector<Kategoria*> result;
                    vector<string> result_choices;
                    for (vector<Kategoria*>::iterator i=kategorie.begin(); i!=kategorie.end(); i++, j++)
                        if(kategorie[j]->find_text(text)!=string::npos)
                            result.push_back(kategorie[j]);
                    for (int i=0; i<result.size(); i++)
                        result_choices.push_back(result[i]->new_choice());
                    res = create_menu(window, result_choices, "WYNIKI", "ID   Symbol  Nazwa", true);
                    skip++;
                }
                else
                    dialog(ok, "SZUKAJ", "Brak wynikow.");
                break;
            }
            case -2:
            {
                vector<string> choices {"Wg numeru", "Wg symbolu", "Wg nazwy"};
                int how = dialog(choices, "SORTUJ", "Wybierz sposob sortowania:");
                switch (how)
                {
                case 0:
                    sort(kategorie.begin(), kategorie.end(), item_sort(&Kategoria::id));
                    break;
                case 1:
                    sort(kategorie.begin(), kategorie.end(), item_sort(&Kategoria::symbol));
                    break;
                case 2:
                    sort(kategorie.begin(), kategorie.end(), item_sort(&Kategoria::nazwa));
                    break;
                }
                list_choices.clear();
                for (int i=0; i<kategorie.size(); i++)
                    list_choices.push_back(kategorie[i]->new_choice());
                break;
            }
            case -3:
                dialog(ok, "FILTRUJ", "Brak dostepnych filtrow");
                break;
            case -4:
            {
                vector<string> result, data, fields {"Symbol:", "Nazwa:", "ID:"};
                for(int i=0; i<3; i++)
                    data.push_back("Podaj wartosc");
                    string ex1 = "Operacja przerwana.";
                    try
                    {
                        if(item_form(window, "NOWA KATEGORIA", fields, data, result)!=1) throw ex1;
                        add_category(kategorie, result);
                    }
                    catch(string ex)
                    {
                        dialog(ok, "NOWY", ex);
                        break;
                    }
                    catch(...)
                    {
                        dialog(ok, "NOWY", "Cos poszlo nie tak.");
                        break;
                    }
                list_choices.push_back(kategorie.back()->new_choice());
                dialog(ok, "NOWY", "Zapisano pomyslnie.");
                break;
            }
            default:
                break;
            }
    } while (res!=-5);


    return res;
}

int menu_klienci(WINDOW * window, vector <Klient*> &klienci)
{

    vector<string> list_choices;
    for (int i=0; i<klienci.size(); i++)
        list_choices.push_back(klienci[i]->new_choice());

    vector<string> menu_choices {"Edytuj", "Usun", "Wroc"}, ok{"OK"};

    int res, skip=0;

    do
    {
        if(skip==0)
            res = create_menu(window, list_choices, "Klienci", "ID   Imie                 Nazwisko", true);
        else
            skip = 0;

        if(res>0)
            item_details(window, klienci[res-1], "KLIENCI", menu_choices);

        else
            switch(res)
            {
            case -1:
            {
                string text = search_form("Wprowadz wyszukiwana fraze:");
                if (text.size()!=0)
                {
                    int j=0, x=-1;
                    vector<Klient*> result;
                    vector<string> result_choices;
                    for (vector<Klient*>::iterator i=klienci.begin(); i!=klienci.end(); i++, j++)
                        if(klienci[j]->find_text(text)!=string::npos)
                            result.push_back(klienci[j]);
                    for (int i=0; i<result.size(); i++)
                        result_choices.push_back(result[i]->new_choice());
                    res = create_menu(window, result_choices, "WYNIKI", "ID   Imie                 Nazwisko", true);
                    skip++;
                }
                else
                    dialog(ok, "SZUKAJ", "Brak wynikow.");
                break;
            }
            case -2:
            {
                vector<string> choices {"Wg numeru", "Wg imienia", "Wg nazwiska"};
                int how = dialog(choices, "SORTUJ", "Wybierz sposob sortowania:");
                switch (how)
                {
                case 0:
                    sort(klienci.begin(), klienci.end(), item_sort(&Klient::id));
                    break;
                case 1:
                    sort(klienci.begin(), klienci.end(), item_sort(&Klient::imie));
                    break;
                case 2:
                    sort(klienci.begin(), klienci.end(), item_sort(&Klient::nazwisko));
                    break;
                }
                list_choices.clear();
                for (int i=0; i<klienci.size(); i++)
                    list_choices.push_back(klienci[i]->new_choice());
                break;
            }
            case -3:
            {
                int j=0, x=-1;
                vector<Klient*> result;
                vector<string> choices {"Tylko wypozyczjacy", "Tylko zadluzeni"}, result_choices;
                x = dialog(choices, "FILTRUJ", "Wybierz filtr:");
                for (vector<Klient*>::iterator i=klienci.begin(); i!=klienci.end(); i++, j++)
                    if(klienci[j]->check(x+1))
                        result.push_back(klienci[j]);
                for (int i=0; i<result.size(); i++)
                    result_choices.push_back(result[i]->new_choice());
                res = create_menu(window, result_choices, "WYNIKI", "ID   Imie                 Nazwisko", true);\
                skip++;
                break;
            }
            case -4:
            {
                vector<string> result, data, fields {"Imie:", "Nazwisko:", "ID:", "Adres:", "Telefon:"};
                for(int i=0; i<5; i++)
                    data.push_back("Podaj wartosc");
                    string ex1 = "Operacja przerwana.";
                    try
                    {
                        if(item_form(window, "NOWY KLIENT", fields, data, result)!=1) throw ex1;
                        add_client(klienci, result);
                    }
                    catch(string ex)
                    {
                        dialog(ok, "NOWY", ex);
                        break;
                    }
                    catch(...)
                    {
                        dialog(ok, "NOWY", "Cos poszlo nie tak.");
                        break;
                    }
                list_choices.push_back(klienci.back()->new_choice());
                dialog(ok, "NOWY", "Zapisano pomyslnie.");
                break;
            }
            default:
                break;
            }
    } while (res!=-5);

    return 0;
}

int menu_ksiazki(WINDOW * window, vector <Ksiazka*> &ksiazki, vector <Kategoria*> &kategorie, vector <Klient*> klienci)
{
    vector<string> list_choices;

    for (int i=0; i<ksiazki.size(); i++)
        list_choices.push_back(ksiazki[i]->new_choice());

    vector<string> menu_choices {"Edytuj", "Usun", "Wroc"}, ok{"OK"};

    int res=0, skip=0;

    do
    {
        if (skip==0)
            res = create_menu(window, list_choices, "KSIAZKI", "ID   Autor                Tytul", true);
        else
            skip = 0;

        if(res>0)
            item_details(window, ksiazki[res-1], "KSIAZKI", menu_choices);

        else
            switch(res)
            {
            case -1:
            {
                string text = search_form("Wprowadz wyszukiwana fraze:");
                if (text.size()!=0)
                {
                    int j=0, x=-1;
                    vector<Ksiazka*> result;
                    vector<string> result_choices;
                    for (vector<Ksiazka*>::iterator i=ksiazki.begin(); i!=ksiazki.end(); i++, j++)
                        if(ksiazki[j]->find_text(text)!=string::npos)
                            result.push_back(ksiazki[j]);
                    for (int i=0; i<result.size(); i++)
                        result_choices.push_back(result[i]->new_choice());
                    res = create_menu(window, result_choices, "WYNIKI", "ID   Autor                Tytul", true);
                    skip++;
                }
                else
                    dialog(ok, "SZUKAJ", "Brak wynikow.");
                break;
            }
            case -2:
            {
                vector<string> choices {"Wg numeru", "Wg autora", "Wg tytulu"};
                int how = dialog(choices, "SORTUJ", "Wybierz sposob sortowania:");
                switch (how)
                {
                case 0:
                    sort(ksiazki.begin(), ksiazki.end(), item_sort(&Ksiazka::id));
                    break;
                case 1:
                    sort(ksiazki.begin(), ksiazki.end(), item_sort(&Ksiazka::autor));
                    break;
                case 2:
                    sort(ksiazki.begin(), ksiazki.end(), item_sort(&Ksiazka::tytul));
                    break;
                }
                list_choices.clear();
                for (int i=0; i<ksiazki.size(); i++)
                    list_choices.push_back(ksiazki[i]->new_choice());
                break;
            }
            case -3:
            {
                int j=0, x=-1;
                vector<Ksiazka*> result;
                vector<string> choices {"Tylko dostepne", "Tylko wypozyczone", "Tylko przetrzymane"}, result_choices;
                x = dialog(choices, "FILTRUJ", "Wybierz filtr:");
                for (vector<Ksiazka*>::iterator i=ksiazki.begin(); i!=ksiazki.end(); i++, j++)
                    if(ksiazki[j]->check(x+1))
                        result.push_back(ksiazki[j]);
                for (int i=0; i<result.size(); i++)
                    result_choices.push_back(result[i]->new_choice());
                res = create_menu(window, result_choices, "WYNIKI", "ID   Autor                Tytul", true);
                skip++;
                break;
            }
            case -4:
            {
                vector<string> result, data, fields {"Autor:", "Tytul:", "ID:", "Rok wydania:"};
                for(int i=0; i<4; i++)
                    data.push_back("Podaj wartosc");
                    string ex1 = "Operacja przerwana.", kat;
                    try
                    {
                        if(item_form(window, "NOWA KSIAZKA", fields, data, result)!=1) throw ex1;
                        kat = search_form("Podaj numer kategorii:");
                        if(kat=="") throw ex1;
                        result.push_back(kat);
                        if(add_book(ksiazki, kategorie, result)!=0) throw 0;

                    }
                    catch(string ex)
                    {
                        dialog(ok, "NOWY", ex);
                        break;
                    }
                    catch(...)
                    {
                        dialog(ok, "NOWY", "Cos poszlo nie tak.");
                        break;
                    }
                list_choices.push_back(ksiazki.back()->new_choice());
                dialog(ok, "NOWY", "Zapisano pomyslnie.");
                break;
            }
            default:
                break;
            }
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

string search_form(string text)
{
    WINDOW *pop_up, *menu_window, *form_window;
    MENU* menu;
    ITEM ** items;
    FIELD *field[2];
    FORM * form;

    char * choices[] = {"Szukaj", "Wroc"};

    items = (ITEM **)calloc(2, sizeof(ITEM *));

    for (int i=0; i<2; i++)
        items[i]=new_item(choices[i], "");
    items[2] = (ITEM *)NULL;

    menu = new_menu(items);
    int height = 10, width = 50, starty = (LINES-height)/2, startx = (COLS-width)/2;

    init_pair(1, COLOR_WHITE, COLOR_RED);
    init_pair(2, COLOR_BLUE, COLOR_WHITE);
    init_pair(3, COLOR_BLACK, COLOR_WHITE);

    pop_up = newwin(height, width, starty, startx);
    keypad(pop_up, TRUE);
    menu_window = subwin(pop_up, 1, 15, starty+7, startx+16);
    set_menu_win(menu, menu_window);
    set_menu_sub(menu, menu_window);
    keypad(menu_window, TRUE);
    refresh();

    wbkgd(pop_up, COLOR_PAIR(1));
    box(pop_up, 0, 0);
    mvwaddch(pop_up, 2, 0, ACS_LTEE);
    mvwhline(pop_up, 2, 1, ACS_HLINE, width-2);
    mvwaddch(pop_up, 2, width-1, ACS_RTEE);
    set_menu_back(menu, COLOR_PAIR(1));
    set_menu_fore(menu, A_REVERSE | COLOR_PAIR(1));

    set_menu_mark(menu, "  ");
    set_menu_format(menu, 1, 2);

    post_menu(menu);

    mvwprintw(pop_up, 1, (width-6)/2,"%s", "SZUKAJ");
    mvwprintw(pop_up, 3, 2, "%s", text.c_str());


    field[0] = new_field(1, 40, 0, 0, 0, 0);
    field[1] = NULL;
    field_opts_off(field[0], O_AUTOSKIP);

    /* Create the form and post it */
    form = new_form(field);
    form_window = subwin(pop_up, 1, 40, starty+5, startx+5);
    wbkgd(form_window, COLOR_PAIR(2));
    set_form_win(form, form_window);
    set_form_sub (form, form_window);
    keypad(form_window, TRUE);

    post_form(form);
    refresh();


    wrefresh(menu_window);

    wrefresh(pop_up);
    wrefresh(form_window);


    bool lower_active = false;
    WINDOW * current_window;
    int x=-1, c;

    while((c = wgetch(pop_up)) != KEY_F(1))
    {
        if (lower_active)
        {
            current_window = menu_window;
            curs_set(0);
        }
        else
        {
            current_window = form_window;
            curs_set(1);
        }
        switch(c)
        {
        case KEY_LEFT:
            if(lower_active)
                menu_driver(menu, REQ_PREV_ITEM);
            else
                form_driver(form, REQ_PREV_CHAR);
            break;
        case KEY_RIGHT:
            if(lower_active)
                menu_driver(menu, REQ_NEXT_ITEM);
            else
                form_driver(form, REQ_NEXT_CHAR);
            break;
        case '\t':
        {
            if (lower_active)
                set_menu_fore(menu, A_REVERSE | COLOR_PAIR(1));
            else
                set_menu_fore(menu, COLOR_PAIR(3));
            form_driver(form, REQ_NEXT_FIELD);
            lower_active = !lower_active;
            wrefresh(menu_window);
            break;
        }
        case 10:
        {
            if (!lower_active)
                 form_driver(form, REQ_NEXT_FIELD);
            x = item_index(current_item(menu))+1;
            /*else
            {
                form_driver(form, REQ_NEXT_FIELD);
                lower_active = true;
                if (lower_active)
                    set_menu_fore(menu, A_REVERSE | COLOR_PAIR(1));
                wrefresh(menu_window);
            }*/
            break;
        }
        case KEY_BACKSPACE:
        {
            if(!lower_active)
                form_driver(form, REQ_DEL_PREV);
            break;
        }
        default:
            if(!lower_active)
                form_driver(form, c);
            break;
        }
        if (x!=-1) break;
        wrefresh(current_window);
    }


    for (int i=0; i<2; i++) free_item(items[i]);
    string result = "";
    if(x==1)
    {
        result = field_buffer(field[0], 0);
        char tmp[result.length()];
        strcpy(tmp, result.c_str());
        result = trim(tmp);
    }
    free_field(field[0]);
    free_field(field[1]);
    free_form(form);
    free_menu(menu);
    delwin(pop_up);
    curs_set(0);

    return result;
}

int item_form(WINDOW * window, string header, vector<string> &fields, vector<string> &data, vector<string> &result)
{
    WINDOW *menu_window, *form_window, *current_window;
    MENU * menu;
    FORM * form;
    int n = fields.size();
    FIELD *field[n+1];
    ITEM ** items;
    bool upper_active = false;
    vector<string> menu_choices {"Zapisz", "Wroc"};
    int res=-1, c, m=menu_choices.size(), j=0;
    int height=LINES, width=COLS, starty=(LINES-height)/2, startx=(COLS-width)/2;

    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    init_pair(3, COLOR_BLUE, COLOR_WHITE);
    init_pair(4, COLOR_BLACK, COLOR_CYAN);
    init_pair(5, COLOR_BLACK, COLOR_WHITE);

    wclear(window);
    box(window, 0, 0);
    mvwprintw(window, 1, (width-header.length())/2,"%s", header.c_str());

//form
    for (int i=0; i<n; i++)
    {
        field[i] = new_field(1, 35, i*2, startx+20, 0, 0);
    }
    field[n] = NULL;

    form = new_form(field);
    form_window = subwin(window, height-6, width-2, starty+4, startx+1);
    set_form_win(form, form_window);
    set_form_sub(form, form_window);
    box(form_window, 0, 0);

    form_opts_off(form, O_NL_OVERLOAD);
    form_opts_off(form, O_BS_OVERLOAD);

    for (int i=0; i<n; i++)
    {
        set_field_buffer(field[i], 0, data[i].c_str());
        field_opts_on(field[i], O_PASSOK);
        field_opts_off(field[i], O_AUTOSKIP);
    }

    post_form(form);

    for(vector<string>::iterator i=fields.begin(); i!=fields.end(); i++, j++)
        mvwprintw(form_window, j*2, startx+4, "%s", fields[j].c_str());

    curs_set(1);
    refresh();
    form_driver(form, REQ_END_LINE);
    wrefresh(form_window);

//gorne menu
    items = (ITEM**)calloc(m+1, sizeof(ITEM*));
    j=0;
    for(vector<string>::iterator i=menu_choices.begin(); i!=menu_choices.end(); i++, j++)
        items[j] = new_item(menu_choices[j].c_str(), "");
    items[m] = (ITEM*)NULL;

    menu = new_menu(items);

    menu_window = subwin(window, 1, width-2, starty+2, startx+1);
    set_menu_win(menu, menu_window);
    set_menu_sub(menu, menu_window);

    wbkgd(menu_window, COLOR_PAIR(3));
    set_menu_back(menu, COLOR_PAIR(3));
    set_menu_fore(menu, COLOR_PAIR(5));
    set_menu_format(menu, 1, menu_choices.size());
    set_menu_mark(menu, " ");
    set_menu_spacing(menu, 0, 0, 3);

    post_menu(menu);
    refresh();
    wrefresh(menu_window);

//sterowanie
    while((c=wgetch(window)) != KEY_F(1))
    {
        if(upper_active)
        {
            current_window = menu_window;
        }
        else
        {
            current_window = form_window;
        }

        switch(c)
        {
        case KEY_UP:
            if(!upper_active)
            {
                form_driver(form, REQ_PREV_FIELD);
                form_driver(form, REQ_END_FIELD);
                break;
            }
        case KEY_LEFT:
            if(upper_active)
                menu_driver(menu, REQ_PREV_ITEM);
            else
                form_driver(form, REQ_PREV_CHAR);
            break;
        case KEY_DOWN:
            if(!upper_active)
            {
                form_driver(form, REQ_NEXT_FIELD);
                form_driver(form, REQ_END_FIELD);
                break;
            }
        case KEY_RIGHT:
            if(upper_active)
                menu_driver(menu, REQ_NEXT_ITEM);
            else
                form_driver(form, REQ_NEXT_CHAR);
            break;
        case '\t':
        {
            if (upper_active)
            {
                form_driver(form, REQ_PREV_FIELD);
                set_menu_fore(menu, COLOR_PAIR(5));
                curs_set(1);
                wrefresh(menu_window);
            }
            else
            {
                form_driver(form, REQ_NEXT_FIELD);
                set_menu_fore(menu, COLOR_PAIR(2));
                curs_set(0);
                wrefresh(menu_window);
            }
            upper_active = !upper_active;
            break;
        }
        case 10:
        {
            if (upper_active)
                res = item_index(current_item(menu))+1;
            else
            {
                form_driver(form, REQ_NEXT_FIELD);
                form_driver(form, REQ_END_FIELD);
            }
            break;
        }
        case KEY_BACKSPACE:
        {
            if(!upper_active)
                form_driver(form, REQ_DEL_PREV);
            break;
        }
        default:
            if(!upper_active)
                form_driver(form, c);
            break;
        }
        if (res!=-1)
        {
            curs_set(0);
            break;
        }
        wrefresh(current_window);
    }

    for(int i=0; i<m; i++)
        free_item(items[i]);
    free_menu(menu);

    unpost_form(form);
    free_form(form);

    if(res==1)
        for(int i=0; i<n; i++)
        {
            char * tmp = field_buffer(field[i], 0);
            tmp = trim(tmp);
            result.push_back(tmp);
        }

    for (int i=0; i<n; i++)
        free_field(field[i]);

    return res;
}

