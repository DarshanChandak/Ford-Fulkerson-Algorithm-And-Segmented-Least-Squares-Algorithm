#include <bits/stdc++.h>
#include <chrono>
#include <ctime>
using namespace std;

// ASSUMPTION - NO POINTS WITH SAME X COORDINATES WILL BE GIVEN

class SegmentedLeastSquare
{

    // private variables

private:
    // penalty

    long double c;

    vector<pair<long double, long double>> points;

    // eij maps error between point i to point j

    map<pair<long long, long long>, double> eij;

    // lineij maps slope and intercept of best fit line between point i to point j

    map<pair<long long, long long>, tuple<long double, long double, long double>> lineij;

    // OPT[i] holds the optimal value till point i

    vector<long double> OPT;

    map<long long, long long> parent;

    vector<long long> vecx;

public:
    map<pair<long long, long long>, tuple<long double, long double, long double>> getLineij()
    {
        return this->lineij;
    }

    vector<long long> getVecx()
    {
        return this->vecx;
    }

    // Calculate the minimum fit line and its RSS

    void linearRegression(vector<pair<long double, long double>> points, long long i, long long j, long double &slope, long double &intercept, long double &rss)
    {
        long long n = j - i + 1;

        long double sumX = 0.0;
        long double sumY = 0.0;
        long double sumXY = 0.0;
        long double sumX2 = 0.0;

        for (long long k = i; k <= j; k++)
        {
            sumX += points[k].first;
            sumY += points[k].second;
            sumXY += points[k].first * points[k].second;
            sumX2 += points[k].first * points[k].first;
        }

        long double xBar = sumX / n;
        long double yBar = sumY / n;

        slope = (sumXY - n * xBar * yBar) / (sumX2 - n * xBar * xBar);
        intercept = yBar - slope * xBar;

        rss = 0.0;

        for (long long k = i; k <= j; k++)
        {
            long double error = points[k].second - (slope * points[k].first + intercept);
            rss += error * error;
        }
    }

    // Constructor

    SegmentedLeastSquare(long double c, vector<pair<long double, long double>> points)
    {

        this->c = c;
        this->points = points;
        sort(this->points.begin(), this->points.end());
        for (long long i = 0; i < this->points.size(); i++)
        {
            this->OPT.push_back(pow(10, 308));
            for (long long j = i; j < this->points.size(); j++)
            {
                if (i == j)
                {
                    this->eij[make_pair(i, j)] = 0;
                    this->lineij[make_pair(i, j)] = make_tuple(-1, -1, -1);
                    continue;
                }
                long double slope = 0.0;
                long double intercept = 0.0;
                long double rss = 0.0;
                linearRegression(this->points, i, j, slope, intercept, rss);
                this->eij[make_pair(i, j)] = rss;
                this->lineij[make_pair(i, j)] = make_tuple(slope, intercept, rss);
            }
        }
    }
    // printing parent and total cost
    long double recurrence()
    {
        parent[0] = -1;
        this->OPT[0] = this->c;
        this->OPT[1] = INT_MAX;
        for (long long j = 1; j < this->points.size(); j++)
        {
            this->OPT[j] = INT_MAX;
            for (long long i = 0; i < j; i++)
            {
                if (i != 0)
                {
                    if (this->OPT[j] > eij[make_pair(i, j)] + this->c + OPT[i - 1])
                    {
                        parent[j] = i;
                        this->OPT[j] = eij[make_pair(i, j)] + this->c + OPT[i - 1];
                    }
                }
                else
                {
                    if (this->OPT[j] > eij[make_pair(0, j)] + this->c)
                    {
                        parent[j] = i;
                        this->OPT[j] = eij[make_pair(0, j)] + this->c;
                    }
                }
            }
            int k = this->OPT[j];
        }
        ofstream outFileParent;
        outFileParent.open("output1.txt");
        long long node = this->points.size() - 1;
        while (1)
        {

            this->vecx.push_back(node);
            outFileParent << this->points[node].first << " " << this->points[node].second << "\n";
            node = parent[node];
            outFileParent << this->points[node].first << " " << this->points[node].second << "\n";
            this->vecx.push_back(node);
            if (node == -1)
                goto c;
            node--;
            if (node == -1)
                goto c;
        }

    c:;
        outFileParent.close();
        return OPT[this->points.size() - 1];
    }
};

int main()
{
    auto start = chrono::system_clock::now();
    long long int n, t;
    long double cost;
    vector<pair<long double, long double>> points;
    ifstream inputFile;
    ofstream outFile;
    outFile.open("output.txt");
    inputFile.open("input.txt");
    inputFile >> t;
    inputFile >> n;
    inputFile >> cost;
    for (long long i = 0; i < n; i++)
    {
        long double x, y;
        inputFile >> x;
        inputFile >> y;
        points.push_back(make_pair(x, y));
    }

    inputFile.close();

    SegmentedLeastSquare sq(cost, points);
    double ans = sq.recurrence();
    outFile << "VALUE::" << ans << endl;
    outFile.close();

    ofstream outFile1;
    outFile1.open("output1.txt");
    map<pair<long long, long long>, tuple<long double, long double, long double>> lineij = sq.getLineij();
    vector<long long> vecx = sq.getVecx();
    outFile1 << 1 << "\n";
    outFile1 << cost << "\n";
    outFile1 << vecx.size() / 2 << "\n";
    for (long i = 0; i < vecx.size(); i += 2)
    {
        outFile1 << get<0>(lineij[{vecx[i + 1], vecx[i]}]) << " " << get<1>(lineij[{vecx[i + 1], vecx[i]}]) << "\n";
    }
    outFile1.close();
    auto end = chrono::system_clock::now();
    chrono::duration<double> elapsed_time = end - start;
    cout << "Elapsed Time: " << elapsed_time.count() << "s" << endl;
    return 0;
}