#include "header.hpp"

int create_menu(WINDOW * window, vector<string> &list_choices, string header, bool upper_present)
//generowanie dowolnego menu
{
	WINDOW *menu_window, *list_window, *current_window;
	ITEM **menu_items, **list_items;
	MENU *menu, *lista, *current_menu;
	bool upper_active = false;
	int n = list_choices.size(), res = -1, c, shifty, shiftx=1;
	int height = LINES-4, width = COLS-10, starty = (LINES-height)/2, startx = (COLS-width)/2;
	char tmp[n][256];

	init_pair(1, COLOR_WHITE, COLOR_BLUE);
	init_pair(2, COLOR_WHITE, COLOR_RED);
	init_pair(3, COLOR_BLUE, COLOR_WHITE);
	init_pair(4, COLOR_BLACK, COLOR_CYAN);
	init_pair(5, COLOR_BLACK, COLOR_WHITE);

    wclear(window);
//	wbkgd(stdscr, COLOR_PAIR(3));
	wbkgd(window, COLOR_PAIR(1));
	box(window, 0, 0);
	keypad(window, TRUE);

    int len = header.length();
    wattron(window, A_BOLD);
    mvwprintw(window, 1, (COLS-len)/2,"%s", header.c_str());
    wattroff(window, A_BOLD);

    mvwaddch(window, 2, 0, ACS_LTEE);
    mvwhline(window, 2, 1, ACS_HLINE, COLS-2);
    mvwaddch(window, 2, COLS - 1, ACS_RTEE);

//menu listy
	list_items = (ITEM**)calloc(n+1, sizeof(ITEM*));
	for (int i=0; i<n; i++)
	{
		strcpy(tmp[i], list_choices[i].c_str());
		list_items[i] = new_item(tmp[i], "");
	}
	list_items[n] = (ITEM*)NULL;

	lista = new_menu(list_items);

	if(upper_present)
		shifty=3;
	else
		shifty=2;
	list_window = subwin(window, height-shifty, width-shiftx, starty+shifty, startx+(shiftx/2));
	set_menu_win(lista, list_window);
	set_menu_sub(lista, list_window);

	set_menu_back(lista, COLOR_PAIR(1));
	set_menu_fore(lista, COLOR_PAIR(4));
	set_menu_mark(lista, " ");

	refresh();
	post_menu(lista);

//gorne menu
	if(upper_present){
		vector<string> menu_choices {"Szukaj", "Sortuj", "Filtruj", "Nowy", "Wroc"};

		menu_items = (ITEM**)calloc(6, sizeof(ITEM*));
		for (int i=0; i<5; i++)
		{
			menu_items[i] = new_item(menu_choices[i].c_str(), "");
		}
		menu_items[5] = (ITEM*)NULL;

		menu = new_menu(menu_items);
		menu_window = subwin(window, 1, width, starty+1, startx);
		set_menu_win(menu, menu_window);
		set_menu_sub(menu, menu_window);

		wbkgd(menu_window, COLOR_PAIR(2));
		set_menu_back(menu, COLOR_PAIR(2));
		set_menu_fore(menu, COLOR_PAIR(2));
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
	while((c = wgetch(window))!=KEY_F(1)&&res==-1)
	{
		if(upper_active)
		{
			current_menu = menu;
			current_window = menu_window;
		}
		else if(upper_present)
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
                        set_menu_fore(menu, COLOR_PAIR(2));
                        wrefresh(menu_window);
                        set_menu_fore(lista, COLOR_PAIR(4));
                        wrefresh(list_window);
                    }
                    else
                    {
                        set_menu_fore(menu, COLOR_PAIR(5));
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
	}
	if (upper_present)
	{
        for (int i=0; i<5; i++)
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
        {
            x = item_index(current_item(menu));
            break;
        }
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
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    return 0;
}

int menu_main(WINDOW* window, vector<Kategoria*> &kategorie, vector<Ksiazka*>& ksiazki, vector<Klient*> &klienci)
{
    vector<string> opcje {"Ksiazki", "Klienci", "Kategorie"};
    int res = create_menu(window, opcje, "MENU", false);

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
       // zapisz(kategorie, ksiazki, klienci);
        break;

    case 5:
        break;

    case -1:
        break;

    default:
        menu_main(window, kategorie, ksiazki, klienci);
        break;
    }
//    delwin(menu_window);

    return res;
}

int menu_kategorie(WINDOW * window, vector <Kategoria*>& kategorie)
{
    /*char ** list_choices = cat_choices(kategorie);
    char * menu_choices[] = {"Edytuj", "Usun", "Wroc"};
    char * fields[] = {"Symbol", "Nazwa"};*/

    vector<string> list_choices;
    for (int i=0; i<kategorie.size(); i++)
        list_choices.push_back(kategorie[i]->nazwa);

    vector<string> menu_choices {"Edytuj", "Usun", "Wroc"};

    int res, x=-1, check;

    res = create_menu(window, list_choices, "Kategorie", true);
   /* do
    {
        check = 0;
        if(result>0)
        {
            do
            {
                x = item_details<kategoria>(window, kategorie[result-1], "KATEGORIA", KATEGORIA, menu_choices);
                switch (x)
                {
                case 0:
                {
                    const char * data[] = {kategorie[result-1]->symbol.c_str(), kategorie[result-1]->nazwa.c_str()};
                    item_form(kategorie[result-1], 2, "EDYTUJ", fields, data);
                    x=-1;
                    break;
                }
                case 1:
                {
                    if(usun(kategorie, result-1)==1)
                        x = -1;
                    else
                        menu_kategorie(window, kategorie);
                    break;
                }
                case 2:
                    menu_kategorie(window, kategorie);
                    break;
                }
            }
            while (x<0);
        }

        else
            switch (result)
            {
            case -1:
            {
                int x;
                char * choices[] = {"Symbolu", "Nazwy"};
                x = dialog(choices, 2, "SORTUJ", "Sortuj wedlug:");
                if(x==0)
                {
                    sort_symbol(kategorie);
                }
                else if(x==1)
                {
                    sort_nazwa(kategorie);
                }
                menu_kategorie(window, kategorie);
            }
            break;
            case -2:
            {
                char * choices[] = {"OK"};
                dialog(choices, 1, "FILTRUJ", "Brak dostepnych filtrow.");
                menu_kategorie(window, kategorie);
            }
            case -3:
            {
                result = cat_search(kategorie);
                check++;
                break;
            }
            case -4:
                add_cat(kategorie);
                menu_kategorie(window, kategorie);
                break;
            }
    }
    while (check!=0);*/

    return 0;
}

int menu_klienci(WINDOW * window, vector <Klient*> &klienci)
{

    vector<string> list_choices;
    for (int i=0; i<klienci.size(); i++)
        list_choices.push_back(klienci[i]->imie);

    vector<string> menu_choices {"Edytuj", "Usun", "Wroc"};

    int res, x=-1, check;

    res = create_menu(window, list_choices, "Klienci", true);
    /*char ** list_choices= client_choices(klienci);
    char * menu_choices[] = {"Edytuj", "Usun", "Wroc"};
    char * fields[] = {"Imie", "Nazwisko", "Adres", "Telefon"};
    int result, x=-1, check, found=0;

    result = list_view("KLIENCI", list_choices, klienci.size(), "#  Nr karty  Imie         Nazwisko");

    do
    {
        check=0;
        if(result>0||found==1)
        {
            do
            {
                x = item_details<Klient>(window, klienci[result-1], "KLIENCI", KLIENT, menu_choices);
                switch (x)
                {
                case 0:
                {
                    const char * data[] = {klienci[result-1]->imie.c_str(),
                                           klienci[result-1]->nazwisko.c_str(),
                                           klienci[result-1]->adres.c_str(),
                                           klienci[result-1]->telefon.c_str()
                                          };
                    item_form(klienci[result-1], 4, "EDYTUJ", fields, data);
                    x=-1;
                    break;
                }
                case 1:
                {
                    if(usun(klienci, result-1)==1)
                        x = -1;
                    else
                        menu_klienci(window, klienci);
                    break;
                }
                case 2:
                    menu_klienci(window, klienci);
                    break;
                }
            }
            while (x<0);
        }

        else
            check=0;
        switch (result)
        {
        case -1:
        {
            char * choices[] = {"Nr karty", "Imienia", "Nazwiska"};
            int x;
            x = dialog(choices, 3, "SORTUJ", "Sortuj wedlug:");
            switch (x)
            {
            case 0:
                sort_cid(klienci);
                break;
            case 1:
                sort_imie(klienci);
                break;
            case 2:
                sort_nazwisko(klienci);
                break;
            }
            menu_klienci(window, klienci);
        }
        break;
        case -2:
        {
            char * choices[] = {"Tylko wypozyczajacy", "Tylko zadluzeni"};
            int y = dialog(choices, 2, "FILTRUJ", "Filtruj:");
            vector<klient*> results;
            if(client_search(results, klienci, y+1)!=-5)
                menu_klienci(window, results);
            break;
        }
        case -3:
        {
            vector<klient*> results;
            result = client_search(results, klienci, 0);
            check++;
            break;
        }
        case -4:
        {
            add_client(klienci);
            char * ok[] = {"OK"};
            char text[4];
            sprintf(text, "%d.", klienci.size());
            dialog(ok, 1, "ELO", text);
            menu_klienci(window, klienci);
            break;
        }

        case -5:
            check =0;
            break;
        }
    }
    while(check!=0);

    for (int i=0; i<klienci.size(); i++)
        delete list_choices[i];*/

    return 0;
}

int menu_ksiazki(WINDOW * window, vector <Ksiazka*> &ksiazki, vector <Kategoria*> &kategorie, vector <Klient*> klienci){

    vector<string> list_choices;
    for (int i=0; i<ksiazki.size(); i++)
        list_choices.push_back(kategorie[i]->nazwa);

    vector<string> menu_choices {"Edytuj", "Usun", "Wroc"};

    int res, x=-1, check;

    res = create_menu(window, list_choices, "Kategorie", true);
    /*char ** list_choices = book_choices(ksiazki);
    char * fields[] = {"Autor", "Tytul", "Rok wydania"};
    int result, x=-1, check;

    result = list_view("Ksiazki", list_choices, ksiazki.size(), "#  Autor          Tytul");

    do
    {
        check = 0;
        if(result>0)
        {
            do
            {
                if(ksiazki[result-1]->dostepnosc)
                {
                    char * menu_choices[] = {"Edytuj", "Usun", "Wypozycz", "Wroc"};
                    x = item_details<ksiazka>(window, ksiazki[result-1], "KSIAZKI", KSIAZKA, menu_choices);
                }
                else
                {
                    char * menu_choices[] = {"Edytuj", "Usun", "Zwroc", "Wroc"};
                    x = item_details<ksiazka>(window, ksiazki[result-1], "KSIAZKI", KSIAZKA, menu_choices);
                }

                switch (x)
                {
                case 0:
                {
                    const char * data[] = {ksiazki[result-1]->autor.c_str(),
                                           ksiazki[result-1]->tytul.c_str(),
                                           ksiazki[result-1]->rok_wydania.c_str()
                                          };
                    item_form(ksiazki[result-1], 3, "EDYTUJ", fields, data);
                    x=-1;
                    break;
                }
                case 1:
                {
                    if(usun(ksiazki, result-1)==1)
                        x = -1;
                    else
                        menu_ksiazki(window, ksiazki, kategorie, klienci);
                    break;
                }
                case 2:
                {
                    if(ksiazki[result-1]->dostepnosc)
                    {
                        vector<klient*> results;
                        int res = client_search(results, klienci, 0);
                        if(res>0)
                        {
                            ksiazki[result-1]->wypozyczajacy = klienci[res-1];
                            ksiazki[result-1]->pozyczona = time(0);
                            ksiazki[result-1]->dostepnosc = false;
                            klienci[res-1]->pozyczone.push_back(ksiazki[result-1]);
                            char * ok[] = {"OK"};
                            dialog(ok, 1, "WYPOZYCZ", "Pozycja wypozyczona.");
                        }
                    }

                    else
                    {
                        vector<ksiazka*>::iterator i;
                        i = find(ksiazki[result-1]->wypozyczajacy->pozyczone.begin(), ksiazki[result-1]->wypozyczajacy->pozyczone.end(), ksiazki[result-1]);
                        if (i!= ksiazki[result-1]->wypozyczajacy->pozyczone.end())
                        {
                            ksiazki[result-1]->wypozyczajacy->pozyczone.erase(i);
                            ksiazki[result-1]->wypozyczajacy = NULL;
                            ksiazki[result-1]->pozyczona = 0;
                            ksiazki[result-1]->dostepnosc = true;
                            char * ok[] = {"OK"};
                            dialog(ok, 1, "ZWROC", "Pozycja zwrocona.");

                        }
                    }
                }
                case 3:
                    menu_ksiazki(window, ksiazki, kategorie, klienci);
                    break;
                }
            }
            while (x<0);
        }

        else
            switch (result)
            {
            case -1:
            {
                int x;
                char * choices[] = {"Numeru", "Autora", "Tytulu"};
                x = dialog(choices, 3, "SORTUJ", "Sortuj wedlug:");
                switch (x)
                {
                case 0:
                    sort_bid(ksiazki);
                    break;
                case 1:
                    sort_autor(ksiazki);
                    break;
                case 2:
                    sort_tytul(ksiazki);
                    break;
                }
                menu_ksiazki(window, ksiazki, kategorie, klienci);
            }
            break;
            case -2:
            {
                char * choices[] = {"Tylko wypozyczone", "Tylko przetrzymane", "Tylko dostepne"};
                int y = dialog(choices, 3, "FILTRUJ", "Filtruj:");
                vector<ksiazka*> results;
                if(book_search(results, ksiazki, y+1)!=-5)
                    menu_ksiazki(window, results, kategorie, klienci);
                break;
            }
            case -3:
            {
                vector<ksiazka*> results;
                result = book_search(results, ksiazki, 0);
                check++;
                break;
            }
            case -4:
                add_book(ksiazki, kategorie);
                menu_ksiazki(window, ksiazki, kategorie, klienci);
                break;
            }
    }
    while(check!=0);

    for (int i=0; i<ksiazki.size(); i++)
        delete list_choices[i];*/

    return 0;
}