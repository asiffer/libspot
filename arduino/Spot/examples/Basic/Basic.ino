// Basic.ino

#include "math.h"
#include "Spot.h"

#define MAX_EXCESS 200
#define TRAIN_SIZE 20000
#define TEST_SIZE 500000

struct Spot spot;
double q = 1e-4;           // anomaly probability
int low = 0;               // observe upper tail
int discard_anomalies = 0; // reject anomalies from the model
double level = 0.99;       // tail quantile
double buffer[MAX_EXCESS];

double train[TRAIN_SIZE];

int status = 0;
unsigned long testStartMs = 0;

// N(0, 1) - Box-Muller transform
double gaussian_random() {
    double u = 1.0 - ((double)rand() / (double)RAND_MAX);
    double v = (double)rand() / (double)RAND_MAX;
    return sqrt(-2.0 * log(u)) * cos(2.0 * M_PI * v);
}

void setup() {
    Serial.begin(115200);

    // provide platform specidic math function
    set_math_functions(log, exp, pow);

    // init the structure
    status = spot_init(&spot, q, low, discard_anomalies, level, buffer,
                       MAX_EXCESS, );
    if (spot_init(&spot, q, low, discard_anomalies, level, buffer,
                  MAX_EXCESS) < 0) {
        Serial.printf("spot_init failed, code: %d\n", -status);
        while (true) {
            delay(1000);
        }
    }
    Serial.println("spot_init succeeded");

    // initial data
    for (unsigned long i = 0; i < TRAIN_SIZE; i++) {
        train[i] = gaussian_random();
    }

    // fit
    status = spot_fit(&spot, train, TRAIN_SIZE);
    if (status < 0) {
        Serial.printf("spot_fit failed, code: %d\n", -status);
        while (true) {
            delay(1000);
        }
    }
    Serial.println("spot_fit succeeded");

    // start
    testStartMs = millis();
}

unsigned int n = 0;
int normal = 0;
int excess = 0;
int anomaly = 0;

void loop() {
    n++;
    if (n == TEST_SIZE) {
        unsigned long elapsedMs = millis() - testStartMs;

        Serial.printf("Elapsed time: %d ms (%d values)\n", elapsedMs,
                      TEST_SIZE);
        Serial.printf("Throughput: %f values/s\n",
                      (double)TEST_SIZE / ((double)elapsedMs / 1000.0));
        Serial.printf("NORMAL:%d EXCESS:%d ANOMALY:%d\n", normal, excess,
                      anomaly);
    } else {
        switch (spot_step(&spot, gaussian_random())) {
        case ANOMALY:
            anomaly++;
            break;
        case EXCESS:
            excess++;
            break;
        case NORMAL:
            normal++;
            break;
        }
    }
}
