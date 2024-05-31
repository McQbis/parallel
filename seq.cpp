#include <iostream>
#include <cstdlib>
#include <vector>

struct Item
{
    bool s = 0;
    int w, v;
};

int W, n;

Item* items;

void generateInstance()
{
    items = new Item[n];
    for(int i = 0; i < n; i++)
    {
        items[i].v = 1 + (rand()%W);
        items[i].w = 1 + (rand()%W);
    }
}

int F;

void dp()
{
    std::vector<std::vector<int>> V(n+1, std::vector<int>(W+1, 0));
    for(int i = 1; i < n+1; i++)
    {  
        for(int j = 1; j < W+1; j++)
        {
            if(items[i-1].w > j) V[i][j] = V[i-1][j];
            else
            {
                if(V[i-1][j] > V[i-1][j-items[i-1].w]+items[i-1].v) V[i][j] = V[i-1][j];
                else V[i][j] = V[i-1][j-items[i-1].w]+items[i-1].v;
            }
        }
    }
    F = V[n][W];
    int i = n;
    int j = W;
    while(i > 0 && j > 0)
    {
        if(V[i][j] > V[i-1][j]) 
        {
            i--;
            j -= items[i].w;
        }
        else i--;
    }
}

int main(){
    printf("Podaj liczbe przedmiotow:\n");
    scanf("%d", &n);
    printf("Podaj pojemnosc plecaka:\n");
    scanf("%d", &W);
    generateInstance();
    // for(int i = 0; i < n; i++)
    // {
    //     printf("Przedmiot %d wartosc %d waga %d\n", i, items[i].v, items[i].w);
    // }
    dp();
    printf("\nfunkcja celu: %d\n", F);
    // for(int i = 0; i < n; i++)
    // {
    //     if(items[i].s) printf("Przedmiot %d wartosc %d waga %d\n", i, items[i].v, items[i].w);
    // }
    return 0;
}