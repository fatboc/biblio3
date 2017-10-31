#include "header.hpp"

int data_import(vector <Kategoria*> &kategorie, vector <Klient*> &klienci, vector <Ksiazka*> &ksiazki)
{
    ifstream in1("kategorie");
    ifstream in2("ksiazki");
    ifstream in3("klienci");
    string tmp;

    if(!in1.is_open()||!in2.is_open()||!in3.is_open())
        return -1;

    while (!in1.eof())
    {
        Kategoria *new_cat = new Kategoria;

        getline(in1, new_cat->nazwa);
        getline(in1, new_cat->symbol);
        getline(in1, tmp);
        (stringstream)tmp >> new_cat->id;

        in1.ignore(numeric_limits<streamsize>::max(), '\n');

        if(new_cat->symbol!=""&&new_cat->nazwa!=""&&new_cat->id!=0)
            kategorie.push_back(new_cat);
        else delete new_cat;
    }
    in1.close();

    while (!in2.eof())
    {
        Ksiazka *new_book = new Ksiazka;
        int m;

        getline(in2, new_book->tytul);
        getline(in2, new_book->autor);
        getline(in2, tmp);
        (stringstream)tmp >> new_book->id;
        getline(in2, new_book->rok_wydania);
        new_book->dostepnosc = true;
        new_book->pozyczona = 0;
        new_book->wypozyczajacy = NULL;
        getline(in2, tmp);
        (stringstream)tmp >> m;
        if (m>=0)
        {
            m = find_id(kategorie, m);
            new_book->kat = kategorie[m];
            kategorie[m]->nalezace.push_back(new_book);
        }
        else
            new_book->kat = NULL;

        in2.ignore(numeric_limits<streamsize>::max(), '\n');

        if(new_book->tytul!=""&&new_book->autor!=""&&new_book->id!=0&&new_book->kat!=NULL)
            ksiazki.push_back(new_book);
        else delete new_book;
    }
    in2.close();


    while (!in3.eof())
    {
        Klient *new_client  = new Klient;

        getline(in3, new_client->imie);
        getline(in3, new_client->nazwisko);
        getline(in3, tmp);
        (stringstream)tmp >> new_client->id;
        getline(in3, new_client->telefon);
        getline(in3, new_client->adres);
        getline(in3, tmp);
        for(int i=0; i<atoi(tmp.c_str()); i++){
            string n;
            int m = -1;
            getline(in3, n);
            m = find_id(ksiazki, atoi(n.c_str()));
            if(m>=0)
            {
                new_client->pozyczone.push_back(ksiazki[m]);
                ksiazki[m]->dostepnosc = false;
                ksiazki[m]->wypozyczajacy = new_client;
                string czas;
                getline(in3, czas);
                ksiazki[m]->pozyczona = atoi(czas.c_str());
            }
        }

        in3.ignore(numeric_limits<streamsize>::max(), '\n');

        if(new_client->imie!=""&&new_client->nazwisko!=""&&new_client->id!=0)
            klienci.push_back(new_client);
        else
         delete new_client;
    }
    in3.close();

    return 0;
}

void Ksiazka::print()
{
    cout << "Autor: " << autor << endl;
    cout << "Tytul: " << tytul << endl;
    cout << "ID: " << id << endl;
    cout << "Rok wydania: " << rok_wydania << endl;
    cout << "Kategoria: " << kat->nazwa << endl;
    if(dostepnosc)
        cout << "Dostepna." << endl;
    else
    {
                cout << "Wypozyczona przez: " << wypozyczajacy->imie << " " << wypozyczajacy->nazwisko << "(nr " << wypozyczajacy->id << ")." << endl;
                cout << "Data wypozyczenia: " << pozyczona << endl;
    }
}

void Klient::print()
{
    cout << "Imie: " << imie << endl;
    cout << "Nazwisko: " << nazwisko << endl;
    cout << "ID: " << id << endl;
    cout << "Adres: " << adres << endl;
    cout << "Nr telefonu: " << telefon << endl;
    if(pozyczone.size()!=0)
    {
            cout << "Wypozyczone ksiazki: " << endl;
            int j=0;
            for (vector<Ksiazka*>::iterator i=pozyczone.begin(); i!=pozyczone.end(); i++, j++)
                cout << "\t- " << pozyczone[j]->tytul << " (nr " << pozyczone[j]->id << ")." << endl;
    }
    else
        cout << "Brak wypozyczonych ksiazek." << endl;
}

void Kategoria::print()
{
    cout << "Symbol: " << symbol << endl;
    cout << "Nazwa: " << nazwa << endl;
    cout << "ID: " << id << endl;
    if(nalezace.size()!=0)
    {
            cout << "Do kategorii naleza: " << endl;
            int j=0;
            for (vector<Ksiazka*>::iterator i=nalezace.begin(); i!=nalezace.end(); i++, j++)
                cout << "\t- " << nalezace[j]->tytul << " (nr " << nalezace[j]->id << ")." << endl;
    }
    else
        cout << "Brak ksiazek w kategorii." << endl;
}

void Kategoria::modify(vector<string>& data)
{

    symbol = data[0];
    nazwa = data[1];
    (stringstream)data[2] >> id;
}

void Klient::modify(vector<string>& data)
{
    if (data.size()<5)
        exit(EXIT_FAILURE);
    imie = data[0];
    nazwisko = data[1];
    (stringstream)data[2] >> id;
    adres = data[3];
    telefon = data [4];
}

void Ksiazka::modify(vector<string>& data)
{
    if (data.size()<4)
        exit(EXIT_FAILURE);
    autor = data[0];
    tytul = data[1];
    (stringstream)data[2] >> id;
    rok_wydania = data[3];
}

void add_book(vector<Ksiazka*> &ksiazki, vector<Kategoria*> &kategorie, vector<string> &data)
{
    int kat_no = find_id(kategorie, stoi(data[4]));
    try
    {
        if (data.size()<5) throw "Za malo danych.";
        if (find_id(ksiazki, atoi(data[2].c_str()))!=-1) throw "ID juz istnieje";
        if (kat_no==-1) throw "Bledna kateogria";
    }
    catch(const char * ex){
        cout << ex << endl;
        exit(EXIT_FAILURE);
    }
    Ksiazka * new_book = new Ksiazka;


    new_book->dostepnosc = true;
    new_book->pozyczona = 0;
    new_book->wypozyczajacy = NULL;
    new_book->kat = kategorie[kat_no];
    kategorie[kat_no]->nalezace.push_back(new_book);
    new_book->modify(data);
    ksiazki.push_back(new_book);

}

void add_client(vector<Klient*> &klienci, vector<string> &data)
{
    try
    {
        if (find_id(klienci, atoi(data[2].c_str()))!=-1) throw "ID juz istnieje";
    }
    catch(const char * ex){
        cout << ex << endl;
        exit(EXIT_FAILURE);
    }

    Klient * new_client = new Klient;

    new_client->modify(data);

    klienci.push_back(new_client);
}

void add_category(vector<Kategoria*> &kategorie, vector<string> &data)
{
    try
    {
        if (find_id(kategorie, atoi(data[2].c_str()))!=-1) throw "ID juz istnieje";
    }
    catch(const char * ex){
        cout << ex << endl;
        exit(EXIT_FAILURE);
    }

    Kategoria * new_category  = new Kategoria;

    new_category->modify(data);

    kategorie.push_back(new_category);
}

void Kategoria::item_export(ostream& out)
{
    out << nazwa << endl;
    out << symbol << endl;
    out << id << endl;
    out << endl;
}

void Klient::item_export(ostream &out)
{
    out << imie << endl;
    out << nazwisko << endl;
    out << id << endl;
    out << telefon << endl;
    out << adres << endl;
    out << pozyczone.size() << endl;
    int j=0;
    for (vector<Ksiazka*>::iterator i=pozyczone.begin(); i!=pozyczone.end(); i++, j++)
    {
        out << pozyczone[j]->id << endl;
        out << pozyczone[j]->pozyczona << endl;
    }

    out << endl;
}

void Ksiazka::item_export(ostream &out)
{
    out << tytul << endl;
    out << autor << endl;
    out << id << endl;
    out << rok_wydania << endl;
    out << kat->id << endl;
    out << endl;
}

int data_export(vector<Kategoria*> &kategorie, vector<Ksiazka*> &ksiazki, vector<Klient*> &klienci)
{
    ofstream out1("kategorie");
    ofstream out2("ksiazki");
    ofstream out3("klienci");

    int j=0;

    try
    {
        if (!out1.is_open()) throw -1;
        if (!out2.is_open()) throw -2;
        if (!out3.is_open()) throw -3;
    }
    catch (int ex)
    {
        return ex;
    }

    for (vector<Kategoria*>::iterator i=kategorie.begin(); i!=kategorie.end(); i++, j++)
    {
        kategorie[j]->item_export(out1);
    }
    out1.close();

    j=0;
    for (vector<Ksiazka*>::iterator i=ksiazki.begin(); i!=ksiazki.end(); i++, j++)
    {
        ksiazki[j]->item_export(out2);
    }
    out2.close();

    j=0;
    for (vector<Klient*>::iterator i=klienci.begin(); i!=klienci.end(); i++, j++)
    {
        klienci[j]->item_export(out3);
    }
    out3.close();

    return 0;
}

string Kategoria::new_choice()
{
    string choice = "", tmp;

    tmp = to_string(id);
    tmp.resize(4, ' ');
    choice+=tmp;
    choice+=" ";

    tmp = symbol;
    tmp.resize(7, ' ');
    choice+=tmp;
    choice+=" ";

    tmp = nazwa;
    tmp.resize(COLS-choice.length()-4, ' ');
    choice+=tmp;

    return(choice);
}

string Ksiazka::new_choice()
{
    string choice = "", tmp;

    tmp = to_string(id);
    tmp.resize(4, ' ');
    choice+=tmp;
    choice+=" ";

    tmp = autor;
    tmp.resize(20, ' ');
    choice+=tmp;
    choice+=" ";

    tmp = tytul;
    tmp.resize(COLS-choice.length()-4, ' ');
    choice+=tmp;

    return(choice);
}

string Klient::new_choice()
{
    string choice = "", tmp;

    tmp = to_string(id);
    tmp.resize(4, ' ');
    choice+=tmp;
    choice+=" ";

    tmp = imie;
    tmp.resize(20, ' ');
    choice+=tmp;
    choice+=" ";

    tmp = nazwisko;
    tmp.resize(COLS-choice.length()-4, ' ');
    choice+=tmp;

    return(choice);
}

void cleanup(WINDOW *window, vector<Kategoria*> &kategorie, vector<Klient*> &klienci, vector<Ksiazka*> &ksiazki)
{
    delwin(window);

    delete_all(kategorie);
    delete_all(klienci);
    delete_all(ksiazki);

    endwin();
}
