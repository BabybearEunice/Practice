#include <iostream>
using namespace std;
  
void shortest_path(int originalPath[][500], int shortestPath[][500], int n);
int cp_lowest_village(int n, int t1, int t2, int shortestPath[ ][500], bool cover[], int cost[],int satisfy[], int population[]);
int satisfy_most_people(int n, int t2, int cost[], int shortestPath[][500], int satisfy[], int villagePopu[],bool cover[]);
  
int main()
{
    int n = 0, budget = 0, t1 = 0, t2 = 0;
    int population[500] = {0}, cost[500] = {0}, originalPath[500][500] = {0}, shortestPath[500][500] = {0};
    bool cover[500] = {0};
    int satisfy[500] = {0}; //0未被覆蓋或滿足
    int bestVillage = 0; //每一輪選村落時最佳選擇
    int sum1 = 0, sum2 = 0; //sum1計算有多少村落被覆蓋，sum2計算有多少村落被滿足
  
    cin >> n >> budget >> t1 >> t2; //輸入村落數、總預算、覆蓋時間、滿足時間
    for(int i = 0 ; i < n ; i++) //輸入各村人口數
    {
        cin >> population[i];
    }
    for(int i = 0 ; i < n ; i++) //輸入在各村建診所的成本
    {
        cin >> cost[i];
    }
    for(int i = 0 ; i < n ; i++) //輸入村落之間的車程
    {
        for(int j = i+1 ; j < n ;j++)
            cin >> originalPath[i][j];
    }
    for(int i = 0 ; i < n ; i++) //對稱
    {
        for(int j = 0 ; j < n ; j++)
        {
            originalPath[j][i] = originalPath[i][j];
        }
  
    }
  
    shortest_path(originalPath, shortestPath, n); //求出各村落到各村落最短的時間
  
    for(int i = 0 ; i < n ; i++) //選出能覆蓋所有村落的組合
    {
        for(int j = 0 ; j < n ; j++) //計算有多少村落被覆蓋
        {
            if(cover[j] == 1)
             sum1 = sum1 + 1;
        }
        if(sum1 == n)
            break;
        if(sum1 != n) //如果還有未被覆蓋的村落，繼續run
        {
            bestVillage = cp_lowest_village(n, t1, t2, shortestPath, cover, cost, satisfy, population);
  
            if((budget - cost[bestVillage]) >= 0)
            {
             if(i == 0)
                 cout << bestVillage + 1;
             else
                 cout << " " << bestVillage + 1;
  
             cover[bestVillage] =  1; //有覆蓋的、有滿足的變成1
             satisfy[bestVillage] = 1;
             for(int j = 0 ; j < n ; j++)
             {
                 if(shortestPath[bestVillage][j] <= t1)
                     cover[j] = 1;
                 if(shortestPath[bestVillage][j] <= t2)
                     satisfy[j] = 1;
             }
             budget = budget - cost[bestVillage]; //更新當前預算
            }
            else
                cover[bestVillage] = 0;
        }
        sum1 = 0;
    }
    for(int i = 0 ; i < n ; i++) //用剩餘的預算選出能滿足最多人的組合
    {
        for(int j = 0 ; j < n ; j++) //計算有多少村落被滿足
        {
            if(satisfy[j] == 1)
            sum2 = sum2 + 1;
        }
        if((budget <= 0) || (sum2 == n))
            break;
        if((budget > 0) && (sum2 != n)) //如果還有預算且還有村落沒被滿足
        {
            bestVillage = satisfy_most_people(n, t2, cost, shortestPath, satisfy, population, cover);
  
            if((budget - cost[bestVillage]) >= 0)//有沒有可能繼選到超出預算的？
            {
                cout << " " << bestVillage + 1;
  
                for(int j = 0 ; j < n ;j++)
                {
                    if(shortestPath[bestVillage][j] <= t2)
                        satisfy[j] = 1;
                }
  
                budget = budget - cost[bestVillage]; //更新當前預算
            }
  
            else
                satisfy[bestVillage] = 0;
        }
        sum2 = 0;
    }
  
    return 0;
}
  
  
  
void shortest_path(int originalPath[][500], int shortestPath[][500], int n)
{
    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++)
        {
            if(i == j)
            {
                shortestPath[i][j] = 0;
                continue;
            }
            shortestPath[i][j] = 20000;
        } //initialize array shortestPath
    for(int i = 0; i < n; i++) //i refers as the root
    {
        bool selected[500] = {0};
        selected[i] = 1;
        for(int j = 0; j < n; j++)
        {
            if(originalPath[i][j] != -1)
                shortestPath[i][j] = originalPath[i][j];
        }
        for(int j = 1; j < n; j++) //j + 1 refers as the number of selected nodes
        {
            int current = 0;
            int currentDis = 20000;
            for(int k = 0; k < n; k++)
            {
                if(selected[k] == 1)
                    continue;
                if(shortestPath[i][k] < currentDis)
                {
                    current = k;
                    currentDis = shortestPath[i][k];
                }
            } //find the unselected node with the shortest distance
            for(int k = 0; k < n; k++) //find the nodes that are connected o the current one
            {
                if((originalPath[current][k] != -1) && (shortestPath[i][current] + originalPath[current][k] < shortestPath[i][k]))
                    shortestPath[i][k] = shortestPath[i][current] + originalPath[current][k];
            }
            selected[current] = 1; //mark the current node as been selected
        }
    }
}
  
int cp_lowest_village(int n, int t1, int t2, int shortestPath[][500], bool cover[], int cost[], int satisfy[], int population[])
{
    float bestCp = 10000000000000.0;
    int bestVillage = 0;
    for(int i = 0; i < n; i++)//檢測到每一個村落時間小於t1的複蓋數量
    {
            int times = 0;
            float cp = 10000000000.0;
            for(int j = 0; j < n; j++)
            {
                if((shortestPath[i][j] <= t1) && (cover[j] == 0))
                    times++;
            }
            if(times == 0)
                cp = 100000000000.0;
            else
                cp = static_cast<float> (cost[i]) / times;
             
            if(cp < bestCp)
            {
             bestCp = cp;
             bestVillage = i;
            }
            if(cp == bestCp)//當兩個CP值一樣小,將此覆蓋cp的村落與覆蓋bestCp的村落比較兩者的滿足人數(不用管滿足的cp值)
            {
                int sum2 = 0, sum3 = 0;
                for(int j = 0 ; j < n ; j++)
                {
                    if((shortestPath[bestVillage][j] <= t2) && (satisfy[j] == 0))
  
                        sum2 += population[j];
  
                    if((shortestPath[i][j] <= t2) && (satisfy[j] == 0))
  
                        sum3 += population[j];
                }
                if(sum3 > sum2)
                {
                    bestCp = cp;
                    bestVillage = i;
                }
        }
    }
    return bestVillage;
}
int satisfy_most_people(int n, int t2, int cost[], int shortestPath[][500], int satisfy[], int villagePopu[], bool cover[])
{
    float bestCp = 100000000000.0;
    int bestVillage = 0;
    for(int i = 0; i < n; i++)//檢測到每一個村落時間小於t2滿足的數量
    {
        if(satisfy[i] == 0)
        {
            int population = 0;
            float cp = 100000000.0;
            for(int j = 0; j < n; j++)
            {
                if((shortestPath[i][j] <= t2) && (satisfy[j] == 0))
                    population += villagePopu[j];
                if(population == 0)
                    cp = 100000000000.0;
                else
                    cp = static_cast<float> (cost[i]) / population;
            }
            if(cp < bestCp)
            {
                bestCp = cp;
                bestVillage = i;
            }
        }
    }
    return bestVillage;
 }
