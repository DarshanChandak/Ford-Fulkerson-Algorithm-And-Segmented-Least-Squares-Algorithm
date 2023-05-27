#include <bits/stdc++.h>
#include <chrono>
#include <ctime>
using namespace std;

enum EDGE_TYPE
{
    F,
    B
};

class Path
{

private:
    vector<long long> vertices;

public:
    Path(vector<long long> vertices)
    {
        this->vertices = vertices;
    }
    vector<long long> getVertices()
    {
        return this->vertices;
    }
};

class FlowGraph
{

private:
    vector<long long> vertices;
    long long source;
    long long sink;
    map<pair<long long, long long>, long long> capacityLeft;
    map<pair<long long, long long>, long long> flow;

public:
    FlowGraph(long long source, long long sink, vector<long long> vertices, vector<tuple<long long, long long, long long>> adj[], long long size)
    {
        this->source = source;
        this->sink = sink;
        this->vertices = vertices;
        for (int i = 0; i < size; i++)
        {
            for (auto j : adj[i])
            {
                this->flow[make_pair(i, get<0>(j))] = get<1>(j);
                this->capacityLeft[make_pair(i, get<0>(j))] = get<2>(j);
            }
        }
    }
    map<pair<long long, long long>, long long> getFlow()
    {
        return this->flow;
    }
    map<pair<long long, long long>, long long> getCapacityLeft()
    {
        return this->capacityLeft;
    }
    void setFlow(map<pair<long long, long long>, long long> flow)
    {
        this->flow = flow;
        return;
    }
    void setCapacityLeft(map<pair<long long, long long>, long long> capacityLeft)
    {
        this->capacityLeft = capacityLeft;
        return;
    }
    long long getSource()
    {
        return this->source;
    }
    long long getSink()
    {
        return this->sink;
    }
    vector<long long> getVertices()
    {
        return this->vertices;
    }
};

class ResidualGraph
{

public:
    vector<long long> vertices;

    long long source;

    long long sink;

    vector<tuple<long long, long long, EDGE_TYPE>> adj[1000];

    map<pair<long long, long long>, EDGE_TYPE> edgeType;

public:
    ResidualGraph(FlowGraph G1)
    {
        this->source = G1.getSource();
        this->sink = G1.getSink();
        this->vertices = G1.getVertices();
        map<pair<long long, long long>, long long> flow = G1.getFlow();
        map<pair<long long, long long>, long long> capacity = G1.getCapacityLeft();
        this->edgeType.clear();
        for (auto m : flow)
        {
            if (m.second != 0)
            {
                this->edgeType[make_pair(m.first.second, m.first.first)] = B;
                this->adj[m.first.second].push_back(make_tuple(m.first.first, m.second, B));
            }
        }
        for (auto m : capacity)
        {
            if (m.second != 0)
            {
                this->edgeType[m.first] = F;
                this->adj[m.first.first].push_back(make_tuple(m.first.second, m.second, F));
            }
            else
            {
                this->adj[m.first.first].push_back(make_tuple(m.first.second, -1, F));
            }
        }
    }
    Path getAPath()
    {

        unordered_map<long long, long long> parent;         // stores the parent of each node in the BFS traversal
        queue<long long> q;                                 // queue for BFS traversal
        vector<bool> visited(this->vertices.size(), false); // visited set

        q.push(this->source);
        visited[this->source] = true;
        parent[this->source] = -1;

        while (!q.empty())
        {
            long long curr_node = q.front();
            q.pop();
            if (curr_node == this->sink)
            {
                // Print path from source to destination
                vector<long long> vecx;
                long long node = this->sink;
                while (node != -1)
                {
                    vecx.push_back(node);
                    node = parent[node];
                }
                reverse(vecx.begin(), vecx.end());
                Path path(vecx);
                return path;
            }

            for (auto neighbor : this->adj[curr_node])
            {
                long long next_node = get<0>(neighbor);
                long long isAvailable = get<1>(neighbor);
                if (!visited[next_node] && isAvailable != -1)
                {
                    q.push(next_node);
                    visited[next_node] = true;
                    parent[next_node] = curr_node;
                }
            }
        }
        vector<long long> vecx;
        Path path(vecx);
        return path;
    }

    map<pair<long long, long long>, EDGE_TYPE> getEdgeType()
    {
        return this->edgeType;
    }
};

class Auxiliary
{

public:
    static void augment(Path path, FlowGraph &G1, ResidualGraph &G2)
    {
        long long b = findBottleNeck(path, G1, G2);
        vector<long long> vertices = path.getVertices();
        map<pair<long long, long long>, EDGE_TYPE> edgeTpe = G2.getEdgeType();
        map<pair<long long, long long>, long long> flow = G1.getFlow();
        map<pair<long long, long long>, long long> capacity = G1.getCapacityLeft();
        for (long long i = 1; i < vertices.size(); i++)
        {
            if (edgeTpe[make_pair(vertices[i - 1], vertices[i])] == F)
            {
                flow[make_pair(vertices[i - 1], vertices[i])] += b;
                if (capacity[make_pair(vertices[i - 1], vertices[i])] - b != 0)
                {
                    capacity[make_pair(vertices[i - 1], vertices[i])] -= b;
                }
                else
                {
                    capacity.erase(make_pair(vertices[i - 1], vertices[i]));
                }
            }
            else
            {
                if (flow[make_pair(vertices[i], vertices[i - 1])] - b != 0)
                {
                    flow[make_pair(vertices[i], vertices[i - 1])] -= b;
                }
                else
                {
                    flow.erase(make_pair(vertices[i], vertices[i - 1]));
                }
                capacity[make_pair(vertices[i], vertices[i - 1])] += b;
            }
        }
        G1.setFlow(flow);
        G1.setCapacityLeft(capacity);
        return;
    }

    static long long findBottleNeck(Path path, FlowGraph G1, ResidualGraph G2)
    {
        map<pair<long long, long long>, EDGE_TYPE> edgeType = G2.getEdgeType();
        map<pair<long long, long long>, long long> capacityLeft = G1.getCapacityLeft();
        map<pair<long long, long long>, long long> flow = G1.getFlow();
        long long bottleNeck = INT_MAX;
        vector<long long> vertices = path.getVertices();
        for (long long i = 1; i < vertices.size(); i++)
        {
            if (edgeType.find(make_pair(vertices[i - 1], vertices[i])) != edgeType.end())
            {
                EDGE_TYPE currentEdgeType = edgeType[make_pair(vertices[i - 1], vertices[i])];
                if (currentEdgeType == F)
                {
                    bottleNeck = min(bottleNeck, capacityLeft[make_pair(vertices[i - 1], vertices[i])]);
                }
                else
                {
                    bottleNeck = min(bottleNeck, flow[make_pair(vertices[i], vertices[i - 1])]);
                }
            }
            else
            {
                EDGE_TYPE currentEdgeType = edgeType[make_pair(vertices[i], vertices[i - 1])];
                if (currentEdgeType == F)
                {
                    bottleNeck = min(bottleNeck, capacityLeft[make_pair(vertices[i], vertices[i - 1])]);
                }
                else
                {
                    bottleNeck = min(bottleNeck, flow[make_pair(vertices[i], vertices[i - 1])]);
                }
            }
        }
        return bottleNeck;
    }
};

class NetworkFlow
{

public:
    static FlowGraph FordFulkerson(FlowGraph G1, ResidualGraph G2)
    {
        while (1)
        {
            Path path = G2.getAPath();
            if (path.getVertices().size() == 0)
                goto c;
            Auxiliary::augment(path, G1, G2);
            ResidualGraph G3(G1);
            G2 = G3;
        }
    c:;
        return G1;
    }
};
class stCut
{
    long long noVertex;
    map<long long, long long> edges;
    vector<long long> verticesReachable;
    vector<long long> verticesNotReachable;

public:
    stCut(long long noVertex)
    {
        this->noVertex = noVertex;
    }
    void solve(ResidualGraph G)
    {
        vector<bool> vis(noVertex, false);
        queue<long long> q;
        q.push(G.source);
        vis[G.source] = true;
        while (!q.empty())
        {
            long long node = q.front();
            q.pop();
            for (auto it : G.adj[node])
            {
                if (true)
                {
                    if (vis[get<0>(it)] == false)
                    {
                        q.push(get<0>(it));
                        vis[get<0>(it)] = true;
                    }
                }
            }
        }
        for (long long i = 0; i <= noVertex; i++)
        {
            if (vis[i] == true)
            {
                this->verticesReachable.push_back(i);
            }
            else
            {
                this->verticesNotReachable.push_back(i);
            }
        }
        ofstream out2;
        out2.open("output2.txt");
        out2 << "Nodes reachable from source ::\n";
        for (long long i = 0; i < this->verticesReachable.size(); i++)
        {
            out2 << this->verticesReachable[i] << " ";
        }
        out2 << "\n";
        out2 << "Nodes not reachable from source ::\n";
        for (long long i = 0; i < this->verticesNotReachable.size(); i++)
        {
            out2 << this->verticesNotReachable[i] << " ";
        }
        out2 << "\n";
    }
};

int main()
{
    auto start = chrono::system_clock::now();
    vector<long long> vertices;
    ifstream inputFile;
    ofstream out1, out3;
    inputFile.open("input.txt");
    out1.open("output1.txt");
    out3.open("output3.txt");
    long long noVertices, src, sink, noEdges;
    inputFile >> noVertices;
    inputFile >> src;
    inputFile >> sink;
    inputFile >> noEdges;
    for (long long i = 0; i < noVertices; i++)
        vertices.push_back(i);
    vector<tuple<long long, long long, long long>> adjFlow[noVertices];
    vector<tuple<long long, long long, long long>> adjFlowBipart[noVertices];

    for (long long i = 0; i < noEdges; i++)
    {
        long long u, v, f, c;
        inputFile >> u;
        inputFile >> v;
        inputFile >> c;
        adjFlow[u].push_back(make_tuple(v, 0, c));
        adjFlowBipart[u].push_back(make_tuple(v, 0, 1));
    }
    inputFile.close();

    // assumption :: vertices have to be given in an order from 1 to N, inclusive both, size is N

    FlowGraph G1(src, sink, vertices, adjFlow, noVertices);
    ResidualGraph G2(G1);
    G1 = NetworkFlow::FordFulkerson(G1, G2);
    map<pair<long long, long long>, EDGE_TYPE> edgeType = G2.getEdgeType();
    map<long long, long long> edges;
    for (auto m : edgeType)
    {
        edges[m.first.first] = m.first.second;
    }
    map<pair<long long, long long>, long long> flow = G1.getFlow();
    map<pair<long long, long long>, long long> capacity = G1.getCapacityLeft();
    out1 << "FLOW GOING::\n";
    out1 << flow.size() << endl;
    long long max_flow = 0;
    for (auto m : flow)
    {
        if (m.first.first == src)
            max_flow += m.second;
        out1 << m.first.first << " " << m.first.second << " " << m.second << "\n";
    }
    out1 << "CAPACITY LEFT::\n";
    for (auto m : capacity)
    {
        out1 << m.first.first << " " << m.first.second << " " << m.second << "\n";
    }
    out1 << "max flow :" << max_flow << endl;
    out1.close();
    ResidualGraph G_cut(G1);
    stCut st(noVertices);
    st.solve(G_cut);
    // st.min_stCut(capacity,src,noVertices,edges);

    FlowGraph G3(src, sink, vertices, adjFlowBipart, noVertices);
    ResidualGraph G4(G3);
    G3 = NetworkFlow::FordFulkerson(G3, G4);
    map<pair<long long, long long>, long long> flowBipartite = G3.getFlow();
    long long bipartite = 0;
    for (auto m : flowBipartite)
    {
        if (m.first.first == 2)
        {
            bipartite += m.second;
        }
    }
    map<pair<long long, long long>, long long> flow1 = G3.getFlow();
    map<pair<long long, long long>, long long> capacity1 = G3.getCapacityLeft();
    out3 << "FLOW GOING::\n";
    int cnt = 0;
    for (auto m : flow1)
    {
        if (m.first.first != src && m.first.second != sink)
            cnt++;
    }
    out3 << cnt << endl;
    for (auto m : flow1)
    {
        if (m.first.first != src && m.first.second != sink)
            out3 << m.first.first << " " << m.first.second << " " << m.second << "\n";
    }
    out3 << "CAPACITY LEFT::\n";
    for (auto m : capacity1)
    {
        out3 << m.first.first << " " << m.first.second << " " << m.second << "\n";
    }
    out3.close();
    auto end = chrono::system_clock::now();
    chrono::duration<double> elapsed_time = end - start;
    cout << "Elapsed Time: " << elapsed_time.count() << "s" << endl;
    return 0;
}
