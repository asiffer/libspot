// main.cpp

#include "spot.h"
#include <random>
#include <iostream>
#include <algorithm>
#include <vector>
#include <math.h>

using namespace std;

vector<double> gaussian_white_noise(double mu, double sigma, int N)
{
    vector<double> v(N);
    random_device rd;
    default_random_engine gen(rd());
    normal_distribution<double> gaussian(mu, sigma);

    for (int i = 0; i < N; i++)
    {
        v[i] = gaussian(gen);
    }
    return v;
}

int main(int argc, const char *argv[])
{
    int N = 10000;
    vector<double> data = gaussian_white_noise(0, 1, N);

    int nb_up_alarm = 0;
    int nb_down_alarm = 0;

    Spot S;
    int output;

    for (auto &x : data)
    {
        output = S.step(x);

        if (output == SPOTEVENT::ALERT_UP)
        {
            nb_up_alarm++;
        }
        if (output == SPOTEVENT::ALERT_DOWN)
        {
            nb_down_alarm++;
        }
    }

    cout << "#Up Alerts: " << nb_up_alarm << endl;
    cout << "#Down Alerts: " << nb_down_alarm << endl;

    return 0;
}