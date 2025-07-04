// TwoPartsGraph.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <chrono>

using namespace std;

class Exception : public exception
{
private:
    string str;
public:
    Exception(string s)
    {
        str = s;
    }
    void print()
    {
        cout << "\nExeption: " << str;
    }
};

int** transpose(int** matrix, int n)
{
    int** res = new int* [n];
    for (int i = 0; i < n; i++)
        res[i] = new int[n];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            res[i][j] = matrix[j][i];
    return res;
}

enum Color //Цвет вершины
{
    uncolor,
    blue,
    pink
};

struct Edge //Ребро
{
    int weight;
    int vertex1;
    int vertex2;
    Edge(int w = 0, int v1 = 0, int v2 = 0)
    {
        weight = w;
        vertex1 = v1;
        vertex2 = v2;
    }
    bool operator==(Edge e)
    {
        return weight == e.weight and vertex1 == e.vertex1 and vertex2 == e.vertex2;
    }

    Edge operator=(Edge e)
    {
        weight = e.weight;
        vertex1 = e.vertex1;
        vertex2 = e.vertex2;
        return *this;
    }
};

ostream& operator<<(ostream& s, Edge e)
{
    s << '\n' << "[" << e.vertex1 << ", " << e.vertex2 << "], " << "weight = " << e.weight;
    return s;
}

istream& operator>>(istream& stream, Edge& e)
{
    stream >> e.vertex1;
    stream >> e.vertex2;
    stream >> e.weight;
    return stream;
}

class BipartiteGraph //Класс двудольный граф
{
private:
    vector<Edge> edges; //Массив рёбер
    int CountVertices; //Количество вершин
    vector<Color> colors; //Цвета вершин

    bool fBFS(int vertex1, int vertex2, queue<int>& q) //
    {
        if (colors[vertex2] == uncolor)
        {
            q.push(vertex2);
            colors[vertex2] = colors[vertex1] == blue ? pink : blue;
            return true;
        }
        if (colors[vertex1] == colors[vertex2])
        {
            for (int j = 0; j < CountVertices; j++)
                colors[j] = uncolor;
            return false;
        }
        return true;
    }

    bool BFS() //Раскраска с помощью поиска в ширину
    {
        for (int j = 0; j < CountVertices; j++)
            colors[j] = uncolor;
        queue<int> q;
        q.push(edges[0].vertex1);
        colors[edges[0].vertex1] = blue;
        bool r = false;
        while (true)
        {
            r = true;
            int s;
            for (int j = 0; j < CountVertices; j++)
            {
                if (colors[j] == uncolor)
                {
                    r = false;
                    s = j;
                    break;
                }
            }
            if (r and q.empty())
                return true;
            int cur_vertex;
            if (!q.empty())
            {
                cur_vertex = q.front();
                q.pop();
            }
            else
            {
                cur_vertex = s;
                colors[cur_vertex] = blue;
            }
            for (int i = 0; i < edges.size(); i++)
            {
                if (edges[i].vertex1 == cur_vertex)
                {
                    if (!fBFS(cur_vertex, edges[i].vertex2, q))
                        return false;
                }
                else if (edges[i].vertex2 == cur_vertex)
                {
                    if (!fBFS(cur_vertex, edges[i].vertex1, q))
                        return false;
                }
            }
        }
        return true;
    }

    bool DFS(Color p, int vertex = 0) //Раскраска с помощью поиска в глубину
    {
        colors[vertex] = p == blue ? pink : blue;
        for (int i = 0; i < edges.size(); i++)
        {
            if (edges[i].vertex1 == vertex)
            {
                if (colors[edges[i].vertex2] == colors[vertex])
                {
                    for (int i = 0; i < CountVertices; i++)
                        colors[i] = uncolor;
                    return false;
                }
                else
                    if (colors[edges[i].vertex2] == uncolor)
                        return DFS(colors[vertex], edges[i].vertex2);
            }
            else if (edges[i].vertex2 == vertex)
            {
                if (colors[edges[i].vertex1] == colors[vertex])
                {
                    for (int i = 0; i < CountVertices; i++)
                        colors[i] = uncolor;
                    return false;
                }
                else
                    if (colors[edges[i].vertex1] == uncolor)
                        return DFS(colors[vertex], edges[i].vertex1);
            }
        }
        bool r = true;
        for (int i = 0; i < CountVertices; i++)
            if (colors[i] == uncolor)
                r = false;
        if (r)
            return true;
    }

    bool FindMaxMatch(vector<Edge>& res, vector<Edge> M, vector<int> busy_V, vector<int>& busy_V2, int vertex)
    {
        if (busy_V[vertex] == 0)
        {
            for (int i = 0; i < edges.size(); i++)
            {
                if (edges[i].vertex1 == vertex)
                {
                    if (busy_V[edges[i].vertex2] == 0)
                    {
                        res.push_back(edges[i]);
                        busy_V2[edges[i].vertex2] = 1;
                        busy_V2[vertex] = 1;
                        return true;
                    }
                }
                if (edges[i].vertex2 == vertex)
                {
                    if (busy_V[edges[i].vertex1] == 0)
                    {
                        res.push_back(edges[i]);
                        busy_V2[edges[i].vertex1] = 1;
                        busy_V2[vertex] = 1;
                        return true;
                    }
                }
            }
        }

        busy_V2[vertex] = 1;
        bool a = false;
        for (int i = 0; i < edges.size(); i++)
        {
            if (edges[i].vertex1 == vertex)
                if (busy_V2[edges[i].vertex2] == 0)
                    a = true;
            if (edges[i].vertex2 == vertex)
                if (busy_V2[edges[i].vertex1] == 0)
                    a = true;
        }
        if (!a)
        {
            if (busy_V[vertex] == 0)
                return true;
            int ind = res.size() - 1;
            busy_V2[res[ind].vertex1] = 0;
            busy_V2[res[ind].vertex2] = 0;
            res.pop_back();
            res.pop_back();
            return false;
        }

        for (int i = 0; i < edges.size(); i++)
        {
            if (edges[i].vertex1 == vertex and busy_V2[edges[i].vertex2] == 0)
            {
                int ver = vertex;
                vertex = edges[i].vertex2;
                res.push_back(edges[i]);
                busy_V2[vertex] = 1;
                bool norm = false;
                for (int j = 0; j < M.size(); j++)
                {
                    if (M[j].vertex1 == vertex)
                    {
                        vertex = M[j].vertex2;
                        res.push_back(M[j]);
                        norm = true;
                    }
                    else if (M[j].vertex2 == vertex)
                    {
                        vertex = M[j].vertex1;
                        res.push_back(M[j]);
                        norm = true;
                    }
                }
                if (!norm)
                    return true;
                busy_V2[vertex] = 1;

                bool lucky = FindMaxMatch(res, M, busy_V, busy_V2, vertex);
                if (!lucky)
                {
                    vertex = ver;
                    continue;
                }
                else
                    return true;
            }

            if (edges[i].vertex2 == vertex and busy_V2[edges[i].vertex1] == 0)
            {
                int ver = vertex;
                vertex = edges[i].vertex1;
                res.push_back(edges[i]);
                busy_V2[vertex] = 1;
                bool norm = false;
                for (int j = 0; j < M.size(); j++)
                {
                    if (M[j].vertex1 == vertex)
                    {
                        vertex = M[j].vertex2;
                        res.push_back(M[j]);
                        norm = true;
                    }
                    else if (M[j].vertex2 == vertex)
                    {
                        vertex = M[j].vertex1;
                        res.push_back(M[j]);
                        norm = true;
                    }
                }
                if (!norm)
                    return true;
                busy_V2[vertex] = 1;

                bool lucky = FindMaxMatch(res, M, busy_V, busy_V2, vertex);
                if (!lucky)
                {
                    vertex = ver;
                    continue;
                }
                else
                    return true;
            }
        }
        return false;
    }

    BipartiteGraph NewGraph(int countB, int**& matrix, vector<int>& temp)
    {
        for (int i = 1; i < countB + 1; i++)
        {
            int min = matrix[i][1];
            for (int j = 2; j < countB + 1; j++)
                if (matrix[i][j] < min) //Находим минимум в каждой строке
                    min = matrix[i][j];
            for (int j = 1; j < countB + 1; j++)
                matrix[i][j] = matrix[i][j] - min; //Вычитаем минимум из каждого элемента этой строки
        }
        BipartiteGraph graph;
        //vector<int> temp(CountVertices);
        vector<bool> change(CountVertices, false);
        int w = 0;
        int w2, w3;
        for (int i = 1; i < countB + 1; i++)
        {
            for (int j = 1; j < countB + 1; j++)
            {
                if (matrix[i][j] == 0)
                {
                    if (change[matrix[i][0]] == false)
                    {
                        temp[matrix[i][0]] = w;
                        w2 = w;
                        w++;
                        change[matrix[i][0]] = true;
                    }
                    else
                    {
                        w2 = temp[matrix[i][0]];
                    }
                    if (change[matrix[0][j]] == false)
                    {
                        temp[matrix[0][j]] = w;
                        w3 = w;
                        w++;
                        change[matrix[0][j]] = true;
                    }
                    else
                    {
                        w3 = temp[matrix[0][j]];
                    }
                    graph.push(w2, w3);
                }
            }
        }
        return graph;
    }

public:
    BipartiteGraph()
    {
        CountVertices = 0;
        colors.resize(100, uncolor);
    }

    void push(int v1, int v2, int w = 0)
    {
        if (v1 == v2)
        {
            //Исключение
            throw Exception("You have violated the bipartiteness of the graph!");
        }
        if (v1 == CountVertices and v2 == CountVertices + 1 or v2 == CountVertices and v1 == CountVertices + 1)
        {
            if (CountVertices + 2 <= 100)
                CountVertices = CountVertices + 2;
            else
            {
                //Исключение
                throw Exception("The graph cannot have more than 100 vertices!");
            }
            Edge newEdge(w, v1, v2);
            edges.push_back(newEdge);
            BFS();
            return;
        }
        if (v1 == CountVertices and v2 < CountVertices or v2 == CountVertices and v1 < CountVertices)
        {
            if (CountVertices + 1 <= 100)
                CountVertices = CountVertices + 1;
            else
            {
                //Исключение
                throw Exception("The graph cannot have more than 100 vertices!");
            }
            Edge newEdge(w, v1, v2);
            edges.push_back(newEdge);
            BFS();
            return;
        }
        if (v1 < CountVertices and v2 < CountVertices)
        {
            Edge newEdge(w, v1, v2);
            edges.push_back(newEdge);
            bool r = BFS();
            if (!r)
            {
                //Исключение
                throw Exception("You have violated the bipartiteness of the graph!");
            }
        }
    }

    BipartiteGraph operator=(BipartiteGraph graph)
    {
        edges = graph.edges;
        CountVertices = graph.CountVertices;
        colors = graph.colors;
        for (int i = 0; i < CountVertices; i++)
            colors[i] = graph.colors[i];
        return *this;
    }

    vector<Edge> MaxMatch()
    {
        vector<Edge> res;
        vector<Edge> M;
        vector<Edge> M2;
        vector<int> busy_V(CountVertices);
        vector<int> busy_V2(CountVertices);
        for (int i = 0; i < CountVertices; i++)
        {
            bool r;
            if (busy_V[i] == 0)
            {
                r = FindMaxMatch(res, M, busy_V, busy_V2, i);
                if (!r)
                {
                    for (int j = 0; j < CountVertices; j++)
                        busy_V2[j] = 0;
                    res.clear();
                    continue;
                }
                for (int j = 0; j < M.size(); j++)
                {
                    bool p = true;
                    for (int k = 0; k < res.size(); k++)
                    {
                        if (M[j] == res[k])
                            p = false;
                    }
                    if (p)
                        M2.push_back(M[j]);
                }

                for (int j = 0; j < res.size(); j++)
                {
                    bool p = true;
                    for (int k = 0; k < M.size(); k++)
                    {
                        if (res[j] == M[k])
                            p = false;
                    }
                    if (p)
                        M2.push_back(res[j]);
                }

                M = M2;

                for (int j = 0; j < CountVertices; j++)
                    busy_V2[j] = 0;

                for (int j = 0; j < CountVertices; j++)
                    busy_V[j] = 0;

                for (int j = 0; j < M.size(); j++)
                {
                    busy_V[M[j].vertex1] = 1;
                    busy_V[M[j].vertex2] = 1;
                }

                res.clear();
                M2.clear();
            }
        }
        return M;
    }

    bool perfect(vector<Edge> p)
    {
        bool res = true;
        for (int i = 0; i < CountVertices; i++)
        {
            bool a = false;
            if (colors[i] == blue)
            {
                for (int j = 0; j < p.size(); j++)
                {
                    if (p[j].vertex1 == i or p[j].vertex2 == i)
                        a = true;
                }
                if (!a)
                {
                    res = false;
                    break;
                }
            }
        }
        return res;
    }

    vector<Edge> appointment() //Задача о назначениях
    {
        int countB = 0; //Количество вершин первой доли
        int countP = 0; //Количество вершин второй доли
        for (int i = 0; i < CountVertices; i++)
            if (colors[i] == blue)
                countB++;

        for (int i = 0; i < CountVertices; i++)
            if (colors[i] == pink)
                countP++;

        int c = CountVertices;

        if (countP > countB)
        {
            for (int i = c; i < c + countP - countB; i++)
                for (int j = 0; j < c; j++)
                    if (colors[j] == pink)
                        push(i, j);
            countB = countP;
        }

        if (countB > countP)
        {
            for (int i = c; i < c + countB - countP; i++)
                for (int j = 0; j < c; j++)
                    if (colors[j] == blue)
                        push(i, j);
            countP = countB;
        }

        int** matrix = new int* [countB + 1];
        for (int i = 0; i < countB + 1; i++)
            matrix[i] = new int[countB + 1];

        int k = 1;
        for (int i = 0; i < CountVertices; i++) //Заполнение нулевого столбца матрицы номерами голубых вершин
        {
            if (colors[i] == blue)
            {
                matrix[k][0] = i;
                k++;
            }
        }

        int j = 1;
        for (int i = 0; i < CountVertices; i++) //Заполнение нулевой строки матрицы номерами розовых вершин
        {
            if (colors[i] == pink)
            {
                matrix[0][j] = i;
                j++;
            }
        }

        matrix[0][0] = -1;
        for (int i = 0; i < edges.size(); i++) //Заполение матрицы
        {
            int z = 0;
            int z2 = 0;
            for (int j = 1; j < countB + 1; j++)
                if (matrix[0][j] == edges[i].vertex1 or matrix[0][j] == edges[i].vertex2)
                    z = j;
            for (int j = 1; j < countB + 1; j++)
                if (matrix[j][0] == edges[i].vertex1 or matrix[j][0] == edges[i].vertex2)
                    z2 = j;
            matrix[z2][z] = edges[i].weight;
        }

        while (true)
        {
            vector<int> temp(CountVertices);
            BipartiteGraph graph = NewGraph(countB, matrix, temp);

            vector<Edge> max_m = graph.MaxMatch();
            vector<Edge> max_m_2(max_m.size());
            if (graph.perfect(max_m))
            {
                for (int i = 0; i < max_m.size(); i++)
                {
                    for (int j = 0; j < CountVertices; j++)
                    {
                        if (temp[j] == max_m[i].vertex1)
                            max_m_2[i].vertex1 = j;
                        else if (temp[j] == max_m[i].vertex2)
                            max_m_2[i].vertex2 = j;
                    }
                }
                for (int i = 0; i < edges.size(); i++)
                    for (int j = 0; j < max_m_2.size(); j++)
                        if (edges[i].vertex1 == max_m_2[j].vertex1 and edges[i].vertex2 == max_m_2[j].vertex2)
                            max_m_2[j].weight = edges[i].weight;
                return max_m_2;
            }
            else
            {
                matrix = transpose(matrix, countB + 1);
                vector<int> temp2(CountVertices);
                BipartiteGraph graph = NewGraph(countB, matrix, temp2);
                vector<Edge> max_m2 = graph.MaxMatch();
                vector<Edge> max_m2_2(max_m2.size());
                bool p = graph.perfect(max_m2);
                for (int i = 0; i < max_m2.size(); i++)
                {
                    for (int j = 0; j < CountVertices; j++)
                    {
                        if (temp2[j] == max_m2[i].vertex1)
                            max_m2_2[i].vertex1 = j;
                        else if (temp2[j] == max_m2[i].vertex2)
                            max_m2_2[i].vertex2 = j;
                    }
                }
                if (p)
                {
                    for (int i = 0; i < edges.size(); i++)
                        for (int j = 0; j < max_m2_2.size(); j++)
                            if (edges[i].vertex1 == max_m2_2[j].vertex1 and edges[i].vertex2 == max_m2_2[j].vertex2)
                                max_m2_2[j].weight = edges[i].weight;
                    return max_m2_2;
                }
                else
                {
                    matrix = transpose(matrix, countB + 1);
                    vector<int> lwa; //Строки без назначений
                    for (int i = 1; i < countB + 1; i++)
                    {
                        bool ok = true;
                        for (int j = 1; j < countB + 1; j++)
                        {
                            if (matrix[i][j] == 0)
                            {
                                for (int k = 0; k < max_m2.size(); k++)
                                {
                                    if (matrix[i][0] == max_m2_2[k].vertex1 and matrix[0][j] == max_m2_2[k].vertex2 or matrix[i][0] == max_m2_2[k].vertex2 and matrix[0][j] == max_m2_2[k].vertex1)
                                    {
                                        ok = false;
                                    }
                                }
                            }
                        }
                        if (ok)
                            lwa.push_back(i);
                    }
                    vector<int> cwz; //Столбцы с нулями в этих строках
                    for (int i = 0; i < lwa.size(); i++)
                        for (int j = 1; j < countB + 1; j++)
                            if (matrix[lwa[i]][j] == 0)
                                cwz.push_back(j);

                    vector<int> rz; //Строки с "красными" нулями в этих столбцах
                    for (int i = 0; i < cwz.size(); i++)
                        for (int j = 1; j < countB + 1; j++)
                            if (matrix[j][cwz[i]] == 0)
                                for (int k = 0; k < max_m2_2.size(); k++)
                                    if (matrix[j][0] == max_m2_2[k].vertex1 and matrix[0][cwz[i]] == max_m2_2[k].vertex2 or matrix[j][0] == max_m2_2[k].vertex2 and matrix[0][cwz[i]] == max_m2_2[k].vertex1)
                                        rz.push_back(j);

                    vector<int> unmarked; //Неотмеченные строки
                    for (int i = 1; i < countB + 1; i++)
                    {
                        bool ok = true;
                        for (int j = 0; j < lwa.size(); j++)
                            if (lwa[j] == i)
                                ok = false;
                        for (int j = 0; j < rz.size(); j++)
                            if (rz[j] == i)
                                ok = false;
                        if (ok)
                            unmarked.push_back(i);
                    }
                    for (int i = 0; i < rz.size(); i++)
                        lwa.push_back(rz[i]); //Все не покрытые линиями строки
                    vector<int> ncwz; //Все не покрытые линиями столбцы
                    for (int i = 1; i < countB + 1; i++)
                    {
                        bool ok = true;
                        for (int j = 0; j < cwz.size(); j++)
                        {
                            if (i == cwz[j])
                                ok = false;
                        }
                        if (ok)
                            ncwz.push_back(i);
                    }

                    int min = 100000000;
                    for (int i = 0; i < lwa.size(); i++)
                        for (int j = 0; j < ncwz.size(); j++)
                            if (matrix[lwa[i]][ncwz[j]] < min)
                                min = matrix[lwa[i]][ncwz[j]];

                    for (int i = 0; i < lwa.size(); i++)
                        for (int j = 0; j < ncwz.size(); j++)
                            matrix[lwa[i]][ncwz[j]] = matrix[lwa[i]][ncwz[j]] - min; //Вычитаем из всех непокрытых элементов минимальный из них

                    for (int i = 0; i < unmarked.size(); i++)
                        for (int j = 0; j < cwz.size(); j++)
                            matrix[unmarked[i]][cwz[j]] = matrix[unmarked[i]][cwz[j]] + min;

                    vector<int> temp3(CountVertices);
                    BipartiteGraph graph = NewGraph(countB, matrix, temp3);
                    vector<Edge> max_m3 = graph.MaxMatch();
                    vector<Edge> max_m3_2(max_m3.size());
                    bool p1 = graph.perfect(max_m3);
                    for (int i = 0; i < max_m3.size(); i++)
                    {
                        for (int j = 0; j < CountVertices; j++)
                        {
                            if (temp3[j] == max_m3[i].vertex1)
                            {
                                max_m3_2[i].vertex1 = j;
                            }
                            else if (temp3[j] == max_m3[i].vertex2)
                            {
                                max_m3_2[i].vertex2 = j;
                            }
                        }
                    }
                    if (p1)
                    {
                        for (int i = 0; i < edges.size(); i++)
                            for (int j = 0; j < max_m3_2.size(); j++)
                                if (edges[i].vertex1 == max_m3_2[j].vertex1 and edges[i].vertex2 == max_m3_2[j].vertex2)
                                    max_m3_2[j].weight = edges[i].weight;
                        return max_m3_2;
                    }
                }
            }
        }
    }

    friend ostream& operator<<(ostream& stream, BipartiteGraph& graph);
    friend istream& operator>>(istream& stream, BipartiteGraph& graph);
};

ostream& operator<<(ostream& stream, BipartiteGraph& graph)
{
    for (int i = 0; i < graph.edges.size(); i++)
    {
        stream << graph.edges[i] << ", " << "color vertex1: " << graph.colors[graph.edges[i].vertex1] << ", color vertex2: " << graph.colors[graph.edges[i].vertex2];
    }
    return stream;
}

istream& operator>>(istream& stream, BipartiteGraph& graph)
{
    int countE;
    stream >> countE;
    for (int i = 0; i < countE; i++)
    {
        Edge n;
        stream >> n;
        graph.push(n.vertex1, n.vertex2, n.weight);
    }
    return stream;
}

int main()
{
    //BipartiteGraph testP(10);
    //testP.push(0, 1);
    //testP.push(2, 3);
    //testP.push(3, 4);
    //testP.push(4, 5);
    ////testP.push(6, 7);
    ////testP.push(6, 3);
    //testP.push(2, 1);
    //testP.push(5, 6);
    //testP.push(0, 7);
    //testP.push(1, 4);

   /* cout << testP;

    vector<Edge> r = testP.MaxMatch();
    cout << '\n';
    for (int i = 0; i < r.size(); ++i)
        cout << r[i];*/

        /*BipartiteGraph error;
        error.push(0, 1);
        error.push(2, 1);
        error.push(2, 3);
        error.push(4, 5);
        error.push(6, 3);
        error.push(7, 1);
        error.push(7, 8);
        error.push(4, 8);

        cout << error;

        vector<Edge> r = error.MaxMatch();
        cout << '\n';
        for (int i = 0; i < r.size(); ++i)
            cout << r[i];*/

            //BipartiteGraph error2;
            //error2.push(0, 1);
            //error2.push(0, 2);
            //error2.push(0, 3);
            //error2.push(0, 4);
            //error2.push(0, 5);
            ////error2.push(2, 1);
            //error2.push(6, 1);

            //cout << error2;
            //vector<Edge> r = error2.MaxMatch();
            //cout << '\n';
            //for (int i = 0; i < r.size(); ++i)
            //    cout << r[i];

            /*BipartiteGraph error3;
            error3.push(0, 1);
            error3.push(2, 3);
            error3.push(2, 4);
            error3.push(5, 1);
            error3.push(6, 4);
            error3.push(0, 7);
            error3.push(0, 4);

            cout << error3;

            vector<Edge> r = error3.MaxMatch();
             cout << '\n';
             for (int i = 0; i < r.size(); ++i)
                 cout << r[i];*/
    try
    {

        BipartiteGraph error4;
        auto start_time = std::chrono::high_resolution_clock::now();
        error4.push(0, 1, 7);
        error4.push(0, 2, 3);
        error4.push(0, 3, 6);
        error4.push(0, 4, 9);
        error4.push(0, 5, 5);
        error4.push(6, 1, 7);
        error4.push(6, 2, 5);
        error4.push(6, 3, 7);
        error4.push(6, 4, 5);
        error4.push(6, 5, 6);
        error4.push(7, 1, 7);
        error4.push(7, 2, 6);
        error4.push(7, 3, 8);
        error4.push(7, 4, 8);
        error4.push(7, 5, 9);
        error4.push(8, 1, 3);
        error4.push(8, 2, 1);
        error4.push(8, 3, 6);
        error4.push(8, 4, 5);
        error4.push(8, 5, 7);
        error4.push(9, 1, 2);
        error4.push(9, 2, 4);
        error4.push(9, 3, 9);
        error4.push(9, 4, 9);
        error4.push(9, 5, 5);



        vector<Edge> app = error4.appointment();
        auto end_time = std::chrono::high_resolution_clock::now();
        cout << '\n';
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        cout << error4;

        cout << '\n';

        for (int i = 0; i < app.size(); ++i)
            cout << app[i];
        cout << '\n';
        cout << '\n' << duration.count();





    }
    catch (Exception e)
    {
        e.print();
    }
    /*BipartiteGraph err;
    err.push(0, 1);
    err.push(2, 3);
    err.push(2, 4);
    err.push(2, 5);
    err.push(2, 6);
    err.push(7, 4);
    err.push(7, 5);
    err.push(8, 4);
    err.push(9, 4);

    cout << err;

    vector<Edge> r = err.MaxMatch();
    cout << '\n';
    for (int i = 0; i < r.size(); ++i)
        cout << r[i];*/

        //cout << '\n';
        //BipartiteGraph test;
        //ifstream fin("graph.txt");
        //fin >> test;
        ////cin >> test;
        //cout << test;

    char c; cin >> c;
    return 0;
}


