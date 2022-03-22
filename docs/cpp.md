# C++

## Introduction
`libspot` is the original C++ library. You can use it as-is of through [python3 bindings](../python/).
`libspot` implements the core algorithm **SPOT** which detects anomalies. 

## Installing

### Git
The code is hosted on github, so you can clone the repository

```shell
git clone https://github.com/asiffer/libspot.git
cd libspot && make
sudo make install
```

### Ubuntu
For Ubuntu users, the following personal package archive (ppa) can be added to you system:

```shell
sudo add-apt-repository ppa:asiffer/libspot
sudo apt-get update
```

Then you can easily install the `libspot` package (and the `libspot-dev` package if you want the headers):

```shell
sudo apt install libspot libspot-dev 
```

### Debian-like distributions
As the packaging is made on launchpad, they are specific to Ubuntu. However, the .deb files can easily be added to other debian-like systems.
In `/etc/apt/sources.list`:
```ini
...
deb http://ppa.launchpad.net/asiffer/libspot/ubuntu bionic main
```
Then you can add the public key to your system and download the package:
```shell
$ curl https://keyserver.ubuntu.com/pks/lookup\?op\=get\&search\=0x76F580E5B58944D9 | sudo apt-key add -
$ sudo apt update
$ sudo apt install libspot libspot-dev 
```

## Construct a Spot object

Here is the full constructor of the `Spot` object:

```cpp
Spot(double q, int n_init, double level, bool up, bool down, bool alert, bool bounded, int max_excess);
```

The main parameter is `q`. It defines the probability of an abnormal event. For example, if `q = 0.001`, it means that the algorithm will consider events with a probability lower than `q` as abnormal.

The parameters `n_init` and `level` are involved in the calibration step of the algorithm. `n_init` is the number of data required to calibrate and `level` is the proportion of these initial data not involved in the tail distribution fit. For example, let us use `n_init = 1000` and `level = 0.99`. The algorithm will drop the 990 lowest data and will keep the 10 highest to make a first fit. 

!!! warning
	In practice, the user must ensure that <code>n_init * (1 - level) > 10</code> to have enough data for the fit (at least 10). Furthermore, the <code>level</code> must be high enough to reduce the bias of the algorithm. We may advice to have the <code>level</code> as close to 1 as possible.


The booleans `up` and `down` define whether the algorithm computes upper/lower thresholds or not.

The boolean `alert = true` enables alert triggering. If `alert = false`, it means that all the data (even those out of the bounds) will be taken into account.

The boolean `bounded = true` enables the memory bounding. The bound is given by `max_excess` which is the maximum number of data stored to perform the tail fit.


!!! warning
	If the memory is not bounded, the algorithm will store more and more data to perform the tail fit. However, the computation time will cause more problems than the memory growth because the tail fit complexity depends on this number of stored data.



## Other constructors
**libspot** provides other constructors. For instance rather than specifying `n_init` (the number of data to calibrate), the user can provide an initial batch of data `init_data`. This batch will directly be used to perform the calibration.

```cpp
Spot(double q, vector<double> init_data, double level, bool up, bool down, bool alert, bool bounded, int max_excess);
```


Finally, shorter constructors can also be used:
```cpp
Spot(double q = 1e-3, int n_init = 1000);
Spot(double q, vector<double> init_data);
```

The default values of the parameters are the following:
```cpp
q = 1e-3;
n_init = 1000;
level = 0.99;
up = true;
down = true;
alert = true;
bounded = true;
max_excess = 200;
```

## Run Spot

Once a `Spot` object is created you can feed it with data through the `step` method.
```cpp
int Spot.step(double x)
```
According to the state of the algorithm or the detected event, this method returns an enum item `SPOTEVENT::` (corresponding to an integer).

| enum item   | integer | meaning                        |
|:------------|:-------:|:-------------------------------|
| NORMAL      | 0       | Normal data                    |
| ALERT_UP    | 1       | Abnormal data (too high)       |
| ALERT_DOWN  | -1      | Abnormal data (too low)        |
| EXCESS_UP   | 2       | Excess (update the up model)   |
| EXCESS_DOWN | -2      | Excess (update the down model) |
| INIT_BATCH  | 3       | Data for initial batch         |
| CALIBRATION | 4       | Calibration step               |


## Example

Here we give an example where Spot is applied on a Gaussian white noise of 10000 values.

```cpp
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
	normal_distribution<double> gaussian(mu,sigma);

	for (int i = 0; i < N; i++) {
		v[i] = gaussian(gen);
	}
	return v;
}


int main(int argc, const char * argv[])
{
	int N = 10000;
	vector<double> data = gaussian_white_noise(0,1,N);

	int nb_up_alarm = 0;
	int nb_down_alarm = 0;

  	Spot S;
	int output;

	for(auto & x : data) {
		output = S.step(x);

		if (output == SPOTVEVENT::ALERT_UP) {
			nb_up_alarm++;
		}
		if (output == SPOTVEVENT::ALERT_DOWN) {
			nb_down_alarm++;
		}
	}

	cout << "#Up Alerts: " << nb_up_alarm << endl;
	cout << "#Down Alerts: " << nb_down_alarm << endl;

	return 0;
}
```


You can compile it with `g++` and the `C++11` standard:
```shell
$ g++ -std=c++11 -Wall main.cpp -lspot
```


