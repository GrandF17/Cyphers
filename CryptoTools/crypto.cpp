/* C++ code to demonstrate a 2D vector
with elements(vectors) inside it. */
#include <iostream>
#include <string>
#include <vector>
using namespace std;

vector<vector<int>> xorKey(vector<vector<int>> S1, vector<vector<int>> Key)
{
    for (size_t i = 0; i < S1.size(); i++)
    {
        for (size_t j = 0; j < S1[i].size(); j++)
        {
            S1[i][j] ^= Key[i][j];
        }
    }

    return S1;
}

vector<vector<int>> perm(vector<vector<int>> S1, vector<int> permutation)
{
    for (size_t i = 0; i < S1.size(); i++)
    {
        for (size_t j = 0; j < S1[i].size(); j++)
        {
            S1[i][j] = permutation[S1[i][j]];
        }
    }
    return S1;
}

vector<vector<int>> transpose(vector<vector<int>> S1)
{
    for (size_t i = 0; i < S1.size(); ++i)
    {
        for (size_t j = i; j < S1[i].size(); ++j)
        {
            int t = S1[i][j];
            S1[i][j] = S1[j][i];
            S1[j][i] = t;
        }
    }
    return S1;
}

vector<vector<int>> matrixMul(vector<vector<int>> S1, vector<vector<int>> matrix)
{
    vector<vector<int>> mult = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
    };
    for (size_t i = 0; i < S1.size(); ++i)
        for (size_t j = 0; j < matrix.size(); ++j)
            for (size_t k = 0; k < S1.size(); ++k)
                mult[i][j] ^= S1[i][k] * matrix[k][j];
    return mult;
}

void beautyLog(vector<vector<vector<int>>> info)
{
    for (int strings = 0; strings < info[0].size(); strings++)
    {
        string tmp = "";
        for (int element = 0; element < info.size(); element++)
        {
            for (int columns = 0; columns < info[0][strings].size(); columns++)
                tmp += (to_string(info[element][strings][columns]) + " ");

            if (element != info.size() - 1)
            {
                if (strings == 1)
                    tmp += "\t-->\t";
                else
                    tmp += "\t   \t";
            }
        }
        cout << tmp << endl;
    }
    cout << endl;
}

int main()
{
    vector<vector<int>> S1 = {
        {1, 2, 3},
        {4, 0, 5},
        {6, 7, 1},
    };

    vector<vector<vector<int>>> keys = {
        {
            {5, 6, 0},
            {0, 7, 4},
            {3, 2, 0},
        },
        {
            {0, 5, 2},
            {5, 5, 0},
            {0, 7, 2},
        },
        {
            {7, 0, 6},
            {5, 5, 3},
            {0, 1, 0},
        },
        {
            {0, 1, 0},
            {7, 2, 3},
            {4, 0, 7},
        },
    };

    vector<vector<int>> alpha = {
        {1, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
    };
    vector<vector<int>> matrixA = {
        {1, 0, 1},
        {0, 1, 0},
        {1, 0, 1},
    };

    vector<int> permutation = {5, 1, 6, 2, 7, 4, 3, 0};

    vector<vector<int>> result;

    result = S1;
    for (size_t keyAmount = 0; keyAmount < keys.size(); keyAmount++)
    {
        vector<vector<vector<int>>> info;

        info.push_back(result);
        result = xorKey(result, keys[keyAmount]);
        info.push_back(result);
        result = perm(result, permutation);
        info.push_back(result);
        result = transpose(result);
        info.push_back(result);
        result = matrixMul(result, matrixA);
        info.push_back(result);

        beautyLog(info);
    }

    cout << "-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_" << endl;

    // =============== S1` calc ===============

    vector<vector<int>> S1_ = S1;
    for (size_t i = 0; i < S1_.size(); i++)
        for (size_t j = 0; j < S1_[i].size(); j++)
            S1_[i][j] = S1[i][j] ^ alpha[i][j];

    result = S1_;
    for (size_t keyAmount = 0; keyAmount < keys.size(); keyAmount++)
    {
        vector<vector<vector<int>>> info;

        info.push_back(result);
        result = xorKey(result, keys[keyAmount]);
        info.push_back(result);
        result = perm(result, permutation);
        info.push_back(result);
        result = transpose(result);
        info.push_back(result);
        result = matrixMul(result, matrixA);
        info.push_back(result);

        beautyLog(info);
    }
    cout << "-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_" << endl;

    for (int i = 1; i < permutation.size(); i++)
    {
        cout << "alpha = " << i << endl;
        for (int j = 0; j < permutation.size(); j++)
        {
            int tmp = permutation[j] ^ permutation[(j + i) % permutation.size()];
            cout << "x = " << j << ": "
                 << permutation[j] << " XOR "
                 << permutation[(j + i) % permutation.size()]
                 << " = " << tmp << endl;
        }
    }
}