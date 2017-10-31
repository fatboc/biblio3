#include <vector>

using namespace std;

template <typename T>
void delete_item(vector<T*>& data, int pos){
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
