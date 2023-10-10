#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// we use -O2 optimization flag
#pragma GCC optimize("O2")

double const PI = 0x1.921fb54442d18p+1;
double const _NAN = 0.0 / 0.0;
double const DMAX = RAND_MAX;

// U(0, 1)
double runif() { return (double)rand() / DMAX; }

double rgauss() { return sqrt(-2 * log(runif())) * cos(2 * PI * runif()); }

void fill_runif(double *data, unsigned long size) {
    for (unsigned long i = 0; i < size; i++) {
        data[i] = runif();
    }
}

void fill_rgauss(double *data, unsigned long size) {
    for (unsigned long i = 0; i < size; i++) {
        data[i] = rgauss();
    }
}

// See aakinshin.net/posts/p2-quantile-estimator/
struct P2 {
    double q[5];
    double n[5];
    double np[5];
    double dn[5];
};

static void swap(double *a, double *b) {
    double temp = *a;
    *a = *b;
    *b = temp;
}

static void sort5(double a[static 5]) {
    if (a[1] > a[0]) // Compare 1st and 2nd element #1
        swap(&a[0], &a[1]);
    if (a[3] > a[2]) // Compare 3rd and 4th element #2
        swap(&a[3], &a[2]);
    if (a[0] > a[2]) { // Compare 1st and 3rd element #3
        // run this if 1st element > 3rd element
        swap(&a[1], &a[2]);
        swap(&a[2], &a[3]);
    } else {
        swap(&a[1], &a[2]);
        swap(&a[0], &a[1]);
    }
    // Now 1st, 2nd and 3rd elements are sorted
    // Sort 5th element into 1st, 2nd and 3rd elements
    if (a[4] > a[1]) {     // #4
        if (a[4] > a[0]) { // #5
            swap(&a[4], &a[3]);
            swap(&a[3], &a[2]);
            swap(&a[2], &a[1]);
            swap(&a[1], &a[0]);
        } else {
            swap(&a[4], &a[3]);
            swap(&a[3], &a[2]);
            swap(&a[2], &a[1]);
        }
    } else {
        if (a[4] > a[2]) { // #5
            swap(&a[4], &a[3]);
            swap(&a[3], &a[2]);
        } else {
            swap(&a[4], &a[3]);
        }
    }
    // Sort new 5th element into 2nd, 3rd and 4th
    if (a[4] > a[2]) {     // #6
        if (a[4] > a[1]) { // #7
            swap(&a[4], &a[3]);
            swap(&a[3], &a[2]);
            swap(&a[2], &a[1]);
        } else {
            swap(&a[4], &a[3]);
            swap(&a[3], &a[2]);
        }
    } else {
        if (a[4] > a[3]) { // #7
            swap(&a[4], &a[3]);
        }
    }
}

static void init_p2(struct P2 *p2, double p) {
    for (unsigned i = 0; i < 5; i++) {
        p2->q[i] = 0.0;
        p2->n[i] = (double)i;
        p2->np[i] = 0.0;
        p2->dn[i] = 0.0;
    }

    p2->np[1] = 2 * p;
    p2->np[2] = 4 * p;
    p2->np[3] = 2 + 2 * p;
    p2->np[4] = 4;

    p2->dn[1] = p / 2;
    p2->dn[2] = p;
    p2->dn[3] = (p + 1) / 2;
    p2->dn[4] = 1;
}

static double sign(double d) {
    if (d > 0) {
        return 1;
    }
    if (d < 0) {
        return -1;
    }
    return 0;
}

static double linear(struct P2 *p2, unsigned int i, int d) {
    return p2->q[i] +
           d * (p2->q[i + d] - p2->q[i]) / (p2->n[i + d] - p2->n[i]);
}

static double parabolic(struct P2 *p2, unsigned int i, int d) {
    return p2->q[i] +
           (d / (p2->n[i + 1] - p2->n[i - 1])) *
               ((p2->n[i] - p2->n[i - 1] + d) * (p2->q[i + 1] - p2->q[i]) /
                    (p2->n[i + 1] - p2->n[i]) +
                (p2->n[i + 1] - p2->n[i] - d) * (p2->q[i] - p2->q[i - 1]) /
                    (p2->n[i] - p2->n[i - 1]));
}

static double p2_quantile(struct P2 *p2, double *x, unsigned long size) {
    double xj = 0.0;
    unsigned int k = 0;
    unsigned int i = 0;
    double d = 0.0;
    double qp = 0.0;

    if (size < 5) {
        return _NAN;
    }
    // init q with the 5 first values
    for (i = 0; i < 5; i++) {
        p2->q[i] = x[i];
    }

    sort5(p2->q);
    // now treat the other values
    for (unsigned long j = 5; j < size; j++) {
        xj = x[j];
        if (xj < p2->q[0]) {
            k = 0;
            p2->q[0] = xj;
        } else if (xj > p2->q[4]) {
            k = 3;
            p2->q[4] = xj;
        } else {
            k = 0;
            while (xj > p2->q[k]) {
                k++;
            }
            k--;
            // here q[k] < x < q[k + 1]
            for (i = k + 1; i < 5; i++) {
                p2->n[i] += 1.0;
            }
            for (i = 0; i < 5; i++) {
                p2->np[i] += p2->dn[i];
            }

            // update other markers
            for (i = 1; i < 4; i++) {
                d = p2->np[i] - p2->n[i];
                if ((d >= 1 && (p2->n[i + 1] - p2->n[i]) > 1) ||
                    (d <= -1 && (p2->n[i - 1] - p2->n[i]) < -1)) {
                    d = sign(d);
                    qp = parabolic(p2, i, (int)d);
                    if (!(p2->q[i - 1] < qp && qp < p2->q[i + 1])) {
                        qp = linear(p2, i, (int)d);
                    }
                    p2->q[i] = qp;
                    p2->n[i] += d;
                }
            }
        }
    }
    return p2->q[2];
}

double quantile(double p, double *data, unsigned long size) {
    struct P2 p2;
    init_p2(&p2, p);
    double q = p2_quantile(&p2, data, size);
    return q;
}

static int cmp(const void *a, const void *b) {
    if (*(double *)a > *(double *)b)
        return 1;
    else if (*(double *)a < *(double *)b)
        return -1;
    else
        return 0;
}

size_t const sizes[] = {1000, 2000,  3000,  4000,  5000,
                        7000, 10000, 15000, 20000, 25000};
size_t const n = sizeof(sizes) / sizeof(size_t);
unsigned int const seeds[] = {
    1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17,
    18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
    35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68,
    69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85,
    86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100};
size_t const m = sizeof(seeds) / sizeof(unsigned int);

void accuracy(double target) {
    double *data = NULL;

    // target
    double const p = target;

    double q_p2 = 0.0;
    double q_ref = 0.0;

    double mean = 0.0;
    double maxi = 0.0;
    double rerr = 0.0;

    printf("  size | mean relative error | max relative error \n");
    printf("-------|---------------------|--------------------\n");
    for (size_t i = 0; i < n; i++) {
        data = malloc(sizes[i] * sizeof(double));
        mean = 0.0;
        maxi = 0.0;
        for (size_t j = 0; j < m; j++) {
            srand(seeds[j]);
            fill_rgauss(data, sizes[i]);

            q_p2 = quantile(p, data, sizes[i]);

            qsort(data, sizes[i], sizeof(double), cmp);
            q_ref = data[(int)(p * sizes[i])];

            rerr = fabs(q_p2 - q_ref) / q_ref;
            if (rerr > maxi) {
                maxi = rerr;
            }
            mean += rerr;
        }
        mean /= m;
        printf("%6d |%19.2f%% |%18.2f%% \n", sizes[i], 100 * mean, 100 * maxi);
        free(data);
    }
}

void speed(double target) {
    double *data = NULL;
    clock_t start;
    unsigned long long reference = 0;
    unsigned long long custom = 0;

    // target
    double const p = target;

    double q_p2 = 0.0;
    double q_ref = 0.0;

    double mean = 0.0;
    double maxi = 0.0;
    double rerr = 0.0;

    printf("  size |    sort |      p2 | speed factor \n");
    printf("-------|---------|---------|--------------\n");
    for (size_t i = 0; i < n; i++) {
        data = malloc(sizes[i] * sizeof(double));
        reference = 0;
        custom = 0;

        for (size_t j = 0; j < m; j++) {
            srand(seeds[j]);
            fill_rgauss(data, sizes[i]);

            start = clock();
            qsort(data, sizes[i], sizeof(double), cmp);
            q_ref = data[(int)(p * sizes[i])];
            reference += clock() - start;
        }

        for (size_t j = 0; j < m; j++) {
            srand(seeds[j]);
            fill_rgauss(data, sizes[i]);
            start = clock();
            q_p2 = quantile(p, data, sizes[i]);
            custom += clock() - start;
        }

        printf("%6d |%8d |%8d |%13.1f\n", sizes[i], reference, custom,
               (double)reference / (double)custom);
        free(data);
    }
}

int main(int argc, const char *argv[]) {
    double p = 0.99;
    speed(p);
    accuracy(p);
    return 0;
}