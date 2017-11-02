#include <vector>

using namespace std;

template<typename T, typename M>
struct item_cmp
{
    M T::*member;

    item_cmp(M T::*nmember) : member(nmember) {}

    bool operator()(T *a, T *b) const
    {
        return a->*member < b->*member;
    }
};

template<typename T, typename M>
item_cmp<T, M> item_sort(M T::*member)
{
    return item_cmp<T, M>(member);
}

template <typename T>
void delete_item(vector<T*>& data, int pos)
{
    typename vector<T*>::iterator i;
    T *tmp = data[pos];
    int j=0;

    for(i=data.begin(); i!=data.end(); i++, j++)
        if(j==pos)
            break;

    data.erase(i);

    delete tmp;
}

template <typename T>
void delete_all(vector <T*>& data)
{
    T* tmp;

    for(typename vector<T*>::iterator i=data.end(); i!=data.begin(); i--)
    {
        if(!data.empty())
        {
            tmp = data.back();
            data.pop_back();
            delete tmp;
        }
    }
    data.clear();
}

template <typename T>
int find_id(vector <T*>& data, int missing)
{
    int found=0;

    for(typename vector<T*>::iterator i=data.begin(); i!=data.end(); i++, found++)
    {
        if(data[found]->id==missing)
            return found;
    }

    return -1;
}

template <typename T>
int item_details(WINDOW * window, T *item, string header, vector<string>& menu_choices)
{
    WINDOW *menu_window;
    MENU* menu;
    ITEM ** items;

    int res=-1, c, n=menu_choices.size();
    int height = LINES-4, width = COLS-2, starty = (LINES-height)/2, startx = (COLS-width)/2;

    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    init_pair(3, COLOR_BLUE, COLOR_WHITE);
    init_pair(4, COLOR_BLACK, COLOR_CYAN);

    wclear(window);
    mvwprintw(window, 1, (width-5)/2,"%s", header.c_str());
    box(window, 0, 0);
    item->print(window);

//gorne menu
    items = (ITEM **)calloc(n+1, sizeof(ITEM *));
    for(int i = 0; i < n; ++i)
    {
        items[i] = new_item(menu_choices[i].c_str(), "");
    }
    items[n] = (ITEM *)NULL;

    menu = new_menu(items);

    menu_window = subwin(window, 1, width, starty, startx);
    set_menu_win(menu, menu_window);
    set_menu_sub(menu, menu_window);

    wbkgd(menu_window, COLOR_PAIR(1));
    set_menu_back(menu, COLOR_PAIR(1));
    set_menu_fore(menu, COLOR_PAIR(2));
    set_menu_format(menu, 1, 4);
    set_menu_mark(menu, "  ");
    set_menu_spacing(menu, 0, 0, 3);

    keypad(window, TRUE);
    refresh();

    post_menu(menu);
    wrefresh(window);
    wrefresh(menu_window);

    //sterowanie

    while((c = wgetch(window)) != KEY_F(1))
    {
        switch(c)
        {
        case KEY_LEFT:
            menu_driver(menu, REQ_PREV_ITEM);;
            break;
        case KEY_RIGHT:
            menu_driver(menu, REQ_NEXT_ITEM);
            break;
        case 10:
            res = item_index(current_item(menu));
            break;
        }
        if (res!=-1) break;
        wrefresh(menu_window);
    }

    for (int i=0; i<n+1; i++)
        free_item(items[i]);

    free_menu(menu);

    return -1*(res+1);
}
