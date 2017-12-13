#include "brent.h"


using namespace std;


double brent(bool * found, double a, double b, function<double(double)> f, double epsilon, double delta)
{
    double fa = f(a);
    double fb = f(b);
    double fc;
    double c;
    double s;
    double d;
    double fs;
    bool mflag;

    double dab, dac, dbc;
    double t,u,v,w;

    bool cond, cond1, cond2, cond3, cond4, cond5;

    if (fa*fb < 0)
    {
        if ( abs(fa) < abs(fb) ) // swap (a,b)
        {
            c = a;
            fc = fa;
            a = b;
            fa = fb;
            b = c;
            fb = fc;
        }

        c = a;
        fc = fa;
        mflag = true;


        while (fs != 0 && abs(b-a) > epsilon )
        {
            if (fc != fa && fb != fc) // inverse quadratic interpolation
            {
                dab = fa - fb;
                dac = fa - fc;
                dbc = fb - fc;
                s = (a*fa*fc)/(dab*dac) + (b*fa*fc)/(-dab*dbc) + (c*fa*fb)/(dac*dbc);
            }
            else // secant method
            {
                s = b - fb*(b-a)/(fb-fa);
            }

            u = (3*a+b)/4;
            v = abs(s-b);
            w = abs(b-c);
            t = abs(c-d);

            cond1 = (s<u && s<b) || (s>u && s>b);
            cond2 = mflag && v >= w/2;
            cond3 = !mflag && v >= t/2;
            cond4 = mflag && w < abs(delta);
            cond5 = !mflag && t < abs(delta);

            cond = cond1 || cond2 || cond3 || cond4 || cond5;

            if (cond)
            {
                s = (a+b)/2;
                mflag = true;
            }
            else
            {
                mflag = false;
            }

            fs = f(s);
            d = c;
            c = b;

            if (fa*fs < 0)
            {
                b = s;
                fb = fs;
            }
            else
            {
                a = s;
                fa = fs;
            }

            if ( abs(fa) < abs(fb) ) // swap (a,b)
            {
                c = a;
                fc = fa;
                a = b;
                fa = fb;
                b = c;
                fb = fc;
            }

        }
    *found = true;
    return(s);
    }
    else
    {
        *found = false;
        return(1);
        //throw invalid_argument("f(a) and f(b) must have a different sign");
    }
}


