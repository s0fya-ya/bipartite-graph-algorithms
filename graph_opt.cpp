// TwoPartsGraph.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <chrono>
#include <algorithm>

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

class Graph
{
protected:
    int** mat;
    int CountVertices;
public:
    Graph(int c = 20)
    {
        CountVertices = c;
        mat = new int* [c];
        for (int i = 0; i < c; ++i)
        {
            mat[i] = new int[c];
            for (int j = 0; j < c; ++j)
                mat[i][j] = 0;
        }
    }

    void push(int v1, int v2, int w = 1)
    {
        if (v1 < CountVertices and v2 < CountVertices)
        {
            mat[v1][v2] = w;
            mat[v2][v1] = w;
        }
    }

    ~Graph()
    {
        for (int i = 0; i < CountVertices; ++i)
            delete[] mat[i];
        delete[] mat;
    }
};

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

class BipartiteGraph : public Graph //Класс двудольный граф
{
private:
    int CountEdges;
    Edge edges[225];
    Color colors[30];

    bool BFS() //Раскраска с помощью поиска в ширину
    {
        Color* ptr_end = colors + CountVertices;
        for (Color* p = colors; p < ptr_end; ++p)
            *p = uncolor;
        queue<int> q;
        q.push(edges[0].vertex1);
        colors[edges[0].vertex1] = blue;
        bool r = false;
        while (true)
        {
            r = true;
            int s;
            bool m;
            for (int j = 0; j < CountVertices; ++j)
            {
                m = false;
                for (int i = 0; i < CountVertices; ++i)
                {
                    if (mat[j][i] != 0)
                        m = true;
                }
                if (colors[j] == uncolor and m)
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

            Edge* ptr_end = edges + CountEdges;

            for (Edge* p = edges; p < ptr_end; ++p)
            {
                if ((*p).vertex1 == cur_vertex)
                {
                    if (colors[(*p).vertex2] == uncolor)
                    {
                        q.push((*p).vertex2);
                        colors[(*p).vertex2] = colors[cur_vertex] == blue ? pink : blue;
                    }
                    else if (colors[cur_vertex] == colors[(*p).vertex2])
                    {
                        for (int j = 0; j < CountVertices; ++j)
                            colors[j] = uncolor;
                        return false;
                    }
                }
                else if ((*p).vertex2 == cur_vertex)
                {
                    if (colors[(*p).vertex1] == uncolor)
                    {
                        q.push((*p).vertex1);
                        colors[(*p).vertex1] = colors[cur_vertex] == blue ? pink : blue;
                    }
                    else if (colors[cur_vertex] == colors[(*p).vertex1])
                    {
                        for (int j = 0; j < CountVertices; ++j)
                            colors[j] = uncolor;
                        return false;
                    }
                }
            }
        }
        return true;
    }

    bool DFS(Color p, int vertex = 0) //Раскраска с помощью поиска в глубину
    {
        colors[vertex] = p == blue ? pink : blue;
        for (int i = 0; i < CountEdges; ++i)
        {
            if (edges[i].vertex1 == vertex)
            {
                if (colors[edges[i].vertex2] == colors[vertex])
                {
                    for (int i = 0; i < CountVertices; ++i)
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
                    for (int i = 0; i < CountVertices; ++i)
                        colors[i] = uncolor;
                    return false;
                }
                else
                    if (colors[edges[i].vertex1] == uncolor)
                        return DFS(colors[vertex], edges[i].vertex1);
            }
        }
        bool r = true;
        for (int i = 0; i < CountEdges; ++i) ////////////////////////////////////////////////////////////////////////ЗАПОМНИТЬ БЛИН!!!!!!!
            if (colors[edges[i].vertex1] == uncolor or colors[edges[i].vertex2] == uncolor)
            {
                r = false;
                break;
            }
        if (r)
            return true;
    }

    bool FindMaxMatch(vector<Edge>& res, vector<Edge> M, vector<int> busy_V, vector<int>& busy_V2, int vertex)
    {
        if (busy_V[vertex] == 0)
        {
            Edge* ptr_end = edges + CountEdges;
            for (Edge* p = edges; p < ptr_end; ++p)
            {
                if ((*p).vertex1 == vertex)
                {
                    if (busy_V[(*p).vertex2] == 0)
                    {
                        res.push_back((*p));
                        busy_V2[(*p).vertex2] = 1;
                        busy_V2[vertex] = 1;
                        return true;
                    }
                }
                if ((*p).vertex2 == vertex)
                {
                    if (busy_V[(*p).vertex1] == 0)
                    {
                        res.push_back((*p));
                        busy_V2[(*p).vertex1] = 1;
                        busy_V2[vertex] = 1;
                        return true;
                    }
                }
            }
        }

        busy_V2[vertex] = 1;
        bool a = false;
        Edge* ptr_end = edges + CountEdges;

        for (Edge* p = edges; p < ptr_end; ++p) ///////////////////////////////////////////////////////////////ЗАПООООООООМНИТЬ ЭТО ТОЖЕ!!!!!!!
        {
            if ((*p).vertex1 == vertex)
                if (busy_V2[(*p).vertex2] == 0)
                {
                    a = true;
                    break;
                }
            if ((*p).vertex2 == vertex)
                if (busy_V2[(*p).vertex1] == 0)
                {
                    a = true;
                    break;
                }
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
        int ver; ///////////////////////////////////////////////////////////////КРИТИЧЕСКИ ВАЖНО ЗАПОНМИТЬ РРРРР!!!!!
        bool norm;
        int M_size = M.size();

        for (Edge* p = edges; p < ptr_end; ++p)
        {
            if ((*p).vertex1 == vertex and busy_V2[(*p).vertex2] == 0)
            {
                ver = vertex;
                vertex = (*p).vertex2;
                res.push_back((*p));
                busy_V2[vertex] = 1;
                norm = false;
                for (int j = 0; j < M_size; ++j)
                {/////////////////////////////////////////////////////////////////////////////2
                    if (M[j].vertex1 == vertex or M[j].vertex2 == vertex)
                    {
                        vertex = M[j].vertex1 == vertex ? M[j].vertex2 : M[j].vertex1;
                        res.push_back(M[j]);
                        norm = true;
                        break;
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

            if ((*p).vertex2 == vertex and busy_V2[(*p).vertex1] == 0)
            {
                ver = vertex;
                vertex = (*p).vertex1;
                res.push_back((*p));
                busy_V2[vertex] = 1;
                norm = false;

                for (int j = 0; j < M_size; ++j)
                {//////////////////////////////////////////////////////////////////////2
                    if (M[j].vertex1 == vertex or M[j].vertex2 == vertex)
                    {
                        vertex = M[j].vertex1 == vertex ? M[j].vertex2 : M[j].vertex1;
                        res.push_back(M[j]);
                        norm = true;
                        break;
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

        if (res.size() > 1)
        {
            int ind = res.size() - 1;
            busy_V2[res[ind].vertex1] = 0;
            busy_V2[res[ind].vertex2] = 0;
        }
        if (!res.empty())
            res.pop_back();
        if (!res.empty())
            res.pop_back();
        return false;
    }

    BipartiteGraph NewGraph(int countB, int**& matrix)
    {
        countB += 1; //ЗАААААААААААААААААААААПОООООООООООООООООООООООМНИИИИИИИИИИИИИИИИИИИИИИИИИИТЬ!!!!!!!!!!!!!
        for (int i = 1; i < countB; ++i)
        {
            int min = matrix[i][1];
            for (int j = 2; j < countB; ++j)
                if (matrix[i][j] < min) //Находим минимум в каждой строке
                    min = matrix[i][j];
            int countB_2;////////////////////////////////////////////////////////////////////////////////////////////2
            if (countB % 2 == 0)
            {
                countB_2 = countB / 2;
                matrix[i][countB_2] = matrix[i][countB_2] - min;
                for (int j = 1; j < countB_2; ++j)
                {
                    matrix[i][j] = matrix[i][j] - min;
                    matrix[i][countB_2 + j] = matrix[i][countB_2 + j] - min;
                }
            }
            else
            {
                countB_2 = countB / 2 + 1;
                for (int j = 1; j < countB_2; ++j)
                {
                    matrix[i][j] = matrix[i][j] - min;
                    matrix[i][countB_2 + j - 1] = matrix[i][countB_2 + j - 1] - min;
                }
            }
        }
        BipartiteGraph graph(CountVertices); /////////////////////////////////////////////////////2
        for (int i = 1; i < countB; ++i)
        {
            for (int j = i; j < countB; ++j)
            {
                if (matrix[i][j] == 0)
                    graph.push(matrix[i][0], matrix[0][j]);
                else if (matrix[j][i] == 0)
                    graph.push(matrix[j][0], matrix[0][i]);
            }
        }
        return graph;
    }

public:
    BipartiteGraph(int c = 20) : Graph(c)
    {
        Color* ptr_end = colors + 30;
        for (Color* p = colors; p < ptr_end; ++p)
            (*p) = uncolor;
        CountEdges = 0;
    }

    void push(int v1, int v2, int w = 1)
    {
        if (v1 == v2)
        {
            //Исключение
            throw Exception("You have violated the bipartiteness of the graph!");
        }

        if (v1 < CountVertices and v2 < CountVertices)
        {
            Edge newEdge(w, v1, v2);
            edges[CountEdges] = newEdge;
            CountEdges++;
            Graph::push(v1, v2, w);
            Color* ptr_end = colors + CountVertices;
            for (Color* p = colors; p < ptr_end; ++p)
                (*p) = uncolor;
            bool r = BFS();
            if (!r)
            {
                //Исключение
                throw Exception("You have violated the bipartiteness of the graph!");
            }

        }
        else
            throw Exception("You can not add more vertices to the graph!");
    }

    BipartiteGraph operator=(BipartiteGraph graph)
    {
        CountEdges = graph.CountEdges;
        for (int i = 0; i < CountEdges; ++i)
            edges[i] = graph.edges[i];
        CountVertices = graph.CountVertices;
        for (int i = 0; i < CountVertices; ++i)
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
        bool r, p;
        for (int i = 0; i < CountVertices; ++i)
        {
            if (busy_V[i] == 0)
            {
                r = FindMaxMatch(res, M, busy_V, busy_V2, i);
                if (!r)
                {
                    for (int j = 0; j < CountVertices; ++j)
                        busy_V2[j] = 0;
                    res.clear();
                    continue;
                }

                int ms = M.size(); //////////////////////////////////////////ЖОПА
                int rs = res.size();

                for (int j = 0; j < ms; ++j)
                {
                    p = true;
                    for (int k = 0; k < rs; ++k)
                    {
                        if (M[j] == res[k])
                            p = false;
                    }
                    if (p)
                        M2.push_back(M[j]);
                }

                for (int j = 0; j < rs; ++j)
                {
                    p = true;
                    for (int k = 0; k < ms; ++k)
                    {
                        if (res[j] == M[k])
                            p = false;
                    }
                    if (p)
                        M2.push_back(res[j]);
                }

                M = M2;

                //for (int j = 0; j < CountVertices; ++j) ///////////////////////////////////////ЗЗЗААААПППОООМНИИИИИИТЬЬЬЬЬ
                //{
                //    busy_V2[j] = 0;
                //    busy_V[j] = 0;
                //}

                fill(busy_V2.begin(), busy_V2.end(), 0); /////////////////////////////////////////////////////////////2
                fill(busy_V.begin(), busy_V.end(), 0);

                ms = M.size();
                for (int j = 0; j < ms; ++j)
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

    vector<Edge> appointment() //Задача о назначениях
    {
        int countB = 0; //Количество вершин первой доли
        int countP = 0; //Количество вершин второй доли
        Color* ptr_end = colors + CountVertices;
        for (Color* p = colors; p < ptr_end; ++p)
        {
            if (*p == blue)
                countB++;
            if (*p == pink)
                countP++;
        }

        int c = countB + countP - 1;
        ///////////////////////////////////////////////.vdo,scdnhtvyu
        if (countP > countB)
        {
            int cpb = c + countP - countB;
            for (int i = c; i < cpb; ++i)
                for (int j = 0; j < c; ++j)
                    if (colors[j] == pink)
                        push(i, j, 0);
            countB = countP;
        }
        /////////////////////////////////////////////////////vlefgweyu
        else if (countB > countP)
        {
            int cbp = c + countB - countP;
            for (int i = c; i < cbp; ++i)
                for (int j = 0; j < c; ++j)
                    if (colors[j] == blue)
                        push(i, j, 0);
            countP = countB;
        }

        int countB1 = countB + 1;

        int** matrix = new int* [countB1];
        for (int i = 0; i < countB1; ++i)
            matrix[i] = new int[countB1];

        int k = 1;
        int j = 1;
        for (int i = 0; i < CountVertices; ++i) //Заполнение нулевого столбца матрицы номерами голубых вершин
        {
            if (colors[i] == blue)
            {
                matrix[k][0] = i;
                ++k;
            }
            if (colors[i] == pink)
            {
                matrix[0][j] = i;
                ++j;
            }
        }/////////////////////////////////////////////////////////////////////////////////////////////////////2

        //int j = 1;
        //for (int i = 0; i < CountVertices; ++i) //Заполнение нулевой строки матрицы номерами розовых вершин
        //{
        //    if (colors[i] == pink)
        //    {
        //        matrix[0][j] = i;
        //        ++j;
        //    }
        //}

        matrix[0][0] = -1;
        for (int i = 0; i < CountEdges; ++i) //Заполнение матрицы
        {
            int z = 0;
            int z2 = 0;
            for (int j = 1; j < countB1; ++j)
            {
                if (matrix[0][j] == edges[i].vertex1 or matrix[0][j] == edges[i].vertex2)////////////////////////////////////////////////2
                {
                    z = j;
                    //break;
                }
                if (matrix[j][0] == edges[i].vertex1 or matrix[j][0] == edges[i].vertex2)
                {
                    z2 = j;
                    //break;
                }
            }
            /*for (int j = 1; j < countB1; ++j)
                if (matrix[j][0] == edges[i].vertex1 or matrix[j][0] == edges[i].vertex2)
                {
                    z2 = j;
                    break;
                }*/
            matrix[z2][z] = edges[i].weight;
        }

        while (true)
        {
            BipartiteGraph graph = NewGraph(countB, matrix);

            vector<Edge> max_m = graph.MaxMatch();
            int max_m_size = max_m.size();
            if (max_m_size == countB)
            {
                Edge* ptr_edg = edges + CountEdges;
                for (Edge* p = edges; p < ptr_edg; ++p)
                    for (int j = 0; j < max_m_size; ++j)
                        if ((*p).vertex1 == max_m[j].vertex1 and (*p).vertex2 == max_m[j].vertex2)
                            max_m[j].weight = (*p).weight;
                return max_m;
            }
            else
            {
                int** res = new int* [countB1];
                for (int i = 0; i < countB1; ++i)
                    res[i] = new int[countB1];
                for (int i = 0; i < countB1; ++i)
                    for (int j = 0; j < countB1; ++j)
                        res[i][j] = matrix[j][i];
                matrix = res;
                BipartiteGraph graph = NewGraph(countB, matrix);
                vector<Edge> max_m2 = graph.MaxMatch();
                int max_m2_size = max_m2.size();
                if (max_m2_size == countB)
                {
                    Edge* ptr_edg = edges + CountEdges;
                    for (Edge* p = edges; p < ptr_edg; ++p)
                        for (int j = 0; j < max_m2_size; ++j)
                            if ((*p).vertex1 == max_m2[j].vertex1 and (*p).vertex2 == max_m2[j].vertex2)
                                max_m2[j].weight = (*p).weight;
                    return max_m2;
                }
                else
                {
                    int** res = new int* [countB1];
                    for (int i = 0; i < countB1; ++i)
                        res[i] = new int[countB1];
                    for (int i = 0; i < countB1; ++i)
                        for (int j = 0; j < countB1; ++j)
                            res[i][j] = matrix[j][i];
                    matrix = res;
                    vector<int> lwa; //Строки без назначений
                    vector<int> cwz; //Столбцы с нулями в этих строках
                    for (int i = 1; i < countB1; i++) ////////////////////////////////////////////////////////2
                    {
                        bool ok = true;
                        for (int k = 0; k < max_m2_size; ++k)
                        {
                            if (/*matrix[i][0] == max_m2[k].vertex1 or*/ matrix[i][0] == max_m2[k].vertex2) /////////////////////////////2
                            {
                                ok = false;
                                break;
                            }
                        }
                        if (ok)
                        {
                            lwa.push_back(i);
                            for (int j = 1; j < countB1; ++j)
                                if (matrix[i][j] == 0)
                                    cwz.push_back(j);
                        }
                    }
                    cout << "a";
                    int lwa_size = lwa.size(); //////////////////////////////////////////////////////////////


                    //for (int i : lwa) ///////////////////////////////////////////////2
                    //    for (int j = 1; j < countB1; ++j)
                    //        if (matrix[i][j] == 0)
                    //            cwz.push_back(j);

                    int cwz_size = cwz.size(); //////////////////////////////////////////////////////////////
                    vector<int> rz; //Строки с "красными" нулями в этих столбцах
                    for (int i = 0; i < cwz_size; ++i)
                        for (int j = 1; j < countB1; ++j)
                            if (matrix[j][cwz[i]] == 0)
                                for (int k = 0; k < max_m2_size; ++k)
                                    if (matrix[j][0] == max_m2[k].vertex2 and matrix[0][cwz[i]] == max_m2[k].vertex1) /////////////////////////////2
                                        rz.push_back(j);

                    int rz_size = rz.size(); /////////////////////////////////////////////////////////////////
                    vector<int> unmarked; //Неотмеченные строки
                    for (int i = 1; i < countB1; ++i)
                    {
                        bool ok = true;
                        vector<int>::iterator it = lwa.begin();
                        vector<int>::iterator it2 = lwa.end();
                        for (; it != it2; it++)
                            if (*it == i)
                                ok = false;
                        for (int j = 0; j < rz_size; ++j)
                            if (rz[j] == i)
                                ok = false;
                        if (ok)
                            unmarked.push_back(i);
                    }
                    int unmarked_size = unmarked.size(); ////////////////////////////////////////////////////////

                    for (int i = 0; i < rz_size; ++i)
                        lwa.push_back(rz[i]); //Все не покрытые линиями строки
                    lwa_size = lwa.size(); //////////////////////////////////////////////////////////////////////
                    vector<int> ncwz; //Все не покрытые линиями столбцы
                    for (int i = 1; i < countB1; ++i)
                    {
                        bool ok = true;
                        for (int j = 0; j < cwz_size; ++j)
                        {
                            if (i == cwz[j])
                                ok = false;
                        }
                        if (ok)
                            ncwz.push_back(i);
                    }

                    int ncwz_size = ncwz.size(); ////////////////////////////////////////////////////////////////

                    int min = 100000000;
                    for (int i = 0; i < lwa_size; ++i)
                        for (int j = 0; j < ncwz_size; ++j)
                            if (matrix[lwa[i]][ncwz[j]] < min)
                                min = matrix[lwa[i]][ncwz[j]];

                    for (int i = 0; i < lwa_size; ++i)
                        for (int j = 0; j < ncwz_size; ++j)
                            matrix[lwa[i]][ncwz[j]] = matrix[lwa[i]][ncwz[j]] - min; //Вычитаем из всех непокрытых элементов минимальный из них

                    for (int i = 0; i < unmarked_size; ++i)
                        for (int j = 0; j < cwz_size; ++j)
                            matrix[unmarked[i]][cwz[j]] = matrix[unmarked[i]][cwz[j]] + min;

                    BipartiteGraph graph(CountVertices);
                    for (int i = 1; i < countB1; ++i)
                        for (int j = 1; j < countB1; ++j)
                            if (matrix[i][j] == 0)
                                graph.push(matrix[i][0], matrix[0][j]);

                    vector<Edge> max_m3 = graph.MaxMatch();
                    int max_m3_size = max_m3.size();
                    if (max_m3_size == countB)
                    {
                        for (int i = 0; i < CountEdges; ++i)
                            for (int j = 0; j < max_m3_size; ++j)
                                if (edges[i].vertex1 == max_m3[j].vertex1 and edges[i].vertex2 == max_m3[j].vertex2)
                                    max_m3[j].weight = edges[i].weight;
                        return max_m3;
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
    for (int i = 0; i < graph.CountEdges; ++i)
    {
        stream << graph.edges[i] << ", " << "color vertex1: " << graph.colors[graph.edges[i].vertex1] << ", color vertex2: " << graph.colors[graph.edges[i].vertex2];
    }
    stream << '\n' << ":: ";
    for (int i = 0; i < graph.CountVertices; ++i)
    {
        stream << i << " ";
    }
    for (int i = 0; i < graph.CountVertices; ++i)
    {
        stream << '\n' << i << ": ";
        for (int j = 0; j < graph.CountVertices; ++j)
        {
            stream << graph.mat[i][j] << " ";
        }
    }
    return stream;
}

istream& operator>>(istream& stream, BipartiteGraph& graph)
{
    int countE;
    stream >> countE;
    for (int i = 0; i < countE; ++i)
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

        BipartiteGraph error4(11);
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


