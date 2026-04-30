#define Py_LIMITED_API 0x03060000 // Python 3.6+
#define PY_SSIZE_T_CLEAN

#include "../dist/spot.h"
#include <Python.h>

#define STR(x) STR_(x)
#define STR_(x) #x

#define SPOT_INIT_LOW 0
#define SPOT_INIT_DISCARD_ANOMALIES 1
#define SPOT_INIT_LEVEL 0.98
#define SPOT_INIT_MAX_EXCESS 500

// define a wrapper around the raw Spot structure
// clang-format off
typedef struct {
    PyObject_HEAD
    struct Spot _spot;
} Spot;

// clang-format on

static PyObject *to_python_list(double *buffer, unsigned long size) {
    PyObject *list = PyList_New(size);
    if (!list)
        return NULL;

    for (unsigned long i = 0; i < size; i++) {
        PyObject *num = PyFloat_FromDouble(buffer[i]);
        if (!num) {
            Py_DECREF(list);
            return NULL;
        }
        // PyList_SetItem always steals the reference to num, even on failure
        // it calls Py_XDECREF(newitem) internally before returning -1.
        if (PyList_SetItem(list, i, num) < 0) {
            Py_DECREF(list);
            return NULL;
        }
    }
    return list;
}

static PyObject *excesses(struct Ubend *ubend) {
    unsigned long size = ubend->cursor;
    if (ubend->filled) {
        size = ubend->capacity;
    }
    return to_python_list(ubend->data, size);
}

//
// Methods to turn struct into dict
//

static PyObject *Ubend_as_dict(struct Ubend *ubend) {
    PyObject *data = excesses(ubend);
    if (data == NULL) {
        return NULL;
    }

    return Py_BuildValue("{sksksdsisN}", "cursor", ubend->cursor, "capacity",
                         ubend->capacity, "last_erased_data",
                         ubend->last_erased_data, "filled", ubend->filled,
                         "data", data);
}

static PyObject *Peaks_as_dict(struct Peaks *peaks) {
    PyObject *container = Ubend_as_dict(&(peaks->container));
    if (container == NULL) {
        return NULL;
    }

    return Py_BuildValue("{sdsdsdsdsN}", "e", peaks->e, "e2", peaks->e2, "min",
                         peaks->min, "max", peaks->max, "container",
                         container);
}

static PyObject *Tail_as_dict(struct Tail *tail) {
    PyObject *peaks = Peaks_as_dict(&(tail->peaks));
    if (peaks == NULL) {
        return NULL;
    }

    return Py_BuildValue("{sdsdsN}", "gamma", tail->gamma, "sigma",
                         tail->sigma, "peaks", peaks);
}

//
// Spot object
//

PyDoc_STRVAR(
    Spot_init_doc,
    "Spot(q"
    ", low = " STR(SPOT_INIT_LOW) ", discard_anomalies = " STR(SPOT_INIT_DISCARD_ANOMALIES) ", level = " STR(
        SPOT_INIT_LEVEL) ", max_excess = " STR(SPOT_INIT_MAX_EXCESS) ")\n--"
                                                                     "\n\n"
                                                                     "Initiali"
                                                                     "ze a "
                                                                     "new "
                                                                     "Spot "
                                                                     "instanc"
                                                                     "e");

static int Spot_init(Spot *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"q",     "low",        "discard_anomalies",
                             "level", "max_excess", NULL};
    // init parameters
    // *args
    double q;
    // **kwargs
    int low = SPOT_INIT_LOW;
    int discard_anomalies = SPOT_INIT_DISCARD_ANOMALIES;
    double level = SPOT_INIT_LEVEL;
    unsigned long max_excess = SPOT_INIT_MAX_EXCESS;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "d|$iidk", kwlist, &q, &low,
                                     &discard_anomalies, &level, &max_excess))
        return -1;

    // free existing buffer if __init__ is called again
    free(self->_spot.tail.peaks.container.data);
    self->_spot.tail.peaks.container.data = NULL;
    // allocate buffer for excesses
    double *buffer = malloc(max_excess * sizeof(double));
    if (!buffer) {
        PyErr_SetString(PyExc_MemoryError, "Unable to allocate buffer");
        return -1;
    }
    // libspot API call
    int result = spot_init(&(self->_spot), q, low, discard_anomalies, level,
                           buffer, max_excess);
    if (result < 0) {
        free(buffer); // clean the buffer if initialization failed
        self->_spot.tail.peaks.container.data =
            NULL; // reset to zero to avoid double free in dealloc

        // grab error
        char msg[256];
        libspot_error(-result, msg, 256);
        PyErr_SetString(PyExc_RuntimeError, msg);
        return -1;
    }

    return 0;
}

PyDoc_STRVAR(Spot_fit_doc,
             "fit($self, data)\n--\n\n"
             "Compute the first excess and anomaly thresholds based "
             "on training data");

static PyObject *Spot_fit(Spot *self, PyObject *data) {
    PyObject *seq = PySequence_Fast(data, "cannot turn arg into sequence");
    if (seq == NULL) {
        return NULL;
    }

    Py_ssize_t size = PySequence_Size(seq);
    if (size < 0) { // return -1 in case of error
        Py_DECREF(seq);
        return NULL; // propagate original exception
    }
    if (size == 0) {
        Py_DECREF(seq);
        PyErr_SetString(PyExc_ValueError, "Data sequence cannot be empty");
        return NULL;
    }

    // allocate a new raw buffer to pass to the fit method
    double *x = malloc(size * sizeof(double));
    if (x == NULL) {
        Py_DECREF(seq);
        PyErr_SetString(PyExc_MemoryError, "Unable to allocate buffer");
        return NULL;
    }

    // copy data from the python sequence to the raw buffer
    for (Py_ssize_t i = 0; i < size; i++) {
        // PySequence_GetItem creates a new reference (need to Decref later)
        PyObject *item = PySequence_GetItem(seq, i);
        // Return the ith element of seq, or NULL on failure
        if (item == NULL) {
            free(x);
            Py_DECREF(seq);
            return NULL;
        }
        // https://docs.python.org/3/c-api/float.html#c.PyFloat_AsDouble
        x[i] = PyFloat_AsDouble(item);
        if (x[i] == -1.0 && PyErr_Occurred()) {
            free(x);
            Py_DECREF(item);
            Py_DECREF(seq);
            return NULL;
        };
        Py_DECREF(item);
    }
    unsigned long usize = (unsigned long)size;
    // libspot API call
    int result = spot_fit(&(self->_spot), x, usize);
    // decref seq
    Py_DECREF(seq);
    // free the buffer
    free(x);
    // check result

    if (result < 0) {
        char buffer[256];
        libspot_error(-result, buffer, 256);
        PyErr_SetString(PyExc_RuntimeError, buffer);
        return NULL;
    }

    Py_RETURN_NONE;
}

PyDoc_STRVAR(Spot_step_doc, "step($self, x)\n--\n\n"
                            "Fit-predict step");

static PyObject *Spot_step(Spot *self, PyObject *x) {
    double z = PyFloat_AsDouble(x);
    if (z == -1.0 && PyErr_Occurred()) {
        return NULL;
    }

    // libspot API call
    // it can return an error code if the data is NaN
    // we keep it in the result (do not want to raise exception)
    // caller can check if the result is < 0 to detect error
    int result = spot_step(&(self->_spot), z);

    return PyLong_FromLong(result);
}

PyDoc_STRVAR(Spot_quantile_doc, "quantile($self, q)\n--\n\n"
                                "Compute the value zq such that P(X>zq) = q");

static PyObject *Spot_quantile(Spot *self, PyObject *x) {
    double q = PyFloat_AsDouble(x);
    if (q == -1.0 && PyErr_Occurred()) {
        return NULL;
    }

    // libspot API call
    double z = spot_quantile(&(self->_spot), q);

    return PyFloat_FromDouble(z);
}

PyDoc_STRVAR(Spot_probability_doc,
             "probability($self, z)\n--\n\n"
             "Compute the probability p such that P(X>z) = p");

static PyObject *Spot_probability(Spot *self, PyObject *x) {
    double z = PyFloat_AsDouble(x);
    if (z == -1.0 && PyErr_Occurred()) {
        return NULL;
    }

    // libspot API call
    double q = spot_probability(&(self->_spot), z);

    return PyFloat_FromDouble(q);
}

PyDoc_STRVAR(Spot_raw_doc, "raw($self)\n--\n\n"
                           "Return the internal C structure as bytes");

static PyObject *Spot_raw(Spot *self) {
    const char *buffer = (char *)(&self->_spot);
    // bytes is immutable in comparison to bytearray
    PyObject *bytearray =
        PyBytes_FromStringAndSize(buffer, sizeof(struct Spot));

    return bytearray;
}

PyDoc_STRVAR(Spot_excess_doc, "excess($self)\n--\n\n"
                              "Return the stored excesses");

static PyObject *Spot_excesses(Spot *self) {
    return excesses(&self->_spot.tail.peaks.container);
}

PyDoc_STRVAR(
    Spot_as_dict_doc,
    "as_dict($self)\n--\n\n"
    "[EXPERIMENTAL] Return the internal C struct as a python dictionary");

static PyObject *Spot_as_dict(Spot *self) {
    struct Spot *spot = &(self->_spot);
    PyObject *tail = Tail_as_dict(&(spot->tail));
    if (tail == NULL) {
        return NULL;
    }

    return Py_BuildValue(
        "{sdsdsisisdsdsksksN}", "q", spot->q, "level", spot->level,
        "discard_anomalies", spot->discard_anomalies, "low", spot->low,
        "anomaly_threshold", spot->anomaly_threshold, "excess_threshold",
        spot->excess_threshold, "Nt", spot->Nt, "n", spot->n, "tail", tail);
}

static void Spot_dealloc(Spot *self) {
    // see
    // https://docs.python.org/3/c-api/typeobj.html#c.PyTypeObject.tp_dealloc
    free(self->_spot.tail.peaks.container.data);
    PyObject *tp = (PyObject *)Py_TYPE((PyObject *)self);
    PyObject_Free(self);
    Py_DECREF(tp);
}

static PyObject *Spot_get_q(PyObject *self, void *closure) {
    return PyFloat_FromDouble(((Spot *)self)->_spot.q);
}

static PyObject *Spot_get_level(PyObject *self, void *closure) {
    return PyFloat_FromDouble(((Spot *)self)->_spot.level);
}

static PyObject *Spot_get_anomaly_threshold(PyObject *self, void *closure) {
    return PyFloat_FromDouble(((Spot *)self)->_spot.anomaly_threshold);
}

static PyObject *Spot_get_excess_threshold(PyObject *self, void *closure) {
    return PyFloat_FromDouble(((Spot *)self)->_spot.excess_threshold);
}

static PyObject *Spot_get_Nt(PyObject *self, void *closure) {
    return PyLong_FromUnsignedLong(((Spot *)self)->_spot.Nt);
}

static PyObject *Spot_get_n(PyObject *self, void *closure) {
    return PyLong_FromUnsignedLong(((Spot *)self)->_spot.n);
}

static PyObject *Spot_get_gamma(PyObject *self, void *closure) {
    return PyFloat_FromDouble(((Spot *)self)->_spot.tail.gamma);
}

static PyObject *Spot_get_sigma(PyObject *self, void *closure) {
    return PyFloat_FromDouble(((Spot *)self)->_spot.tail.sigma);
}

// clang-format off
static PyGetSetDef Spot_getset[] = {
    {"q",                 Spot_get_q,                 NULL, "Anomaly probability",          NULL},
    {"level",             Spot_get_level,             NULL, "Location of the tail",         NULL},
    {"anomaly_threshold", Spot_get_anomaly_threshold, NULL, "Normal/abnormal threshold",    NULL},
    {"excess_threshold",  Spot_get_excess_threshold,  NULL, "Tail threshold",               NULL},
    {"Nt",                Spot_get_Nt,                NULL, "Total number of excesses",     NULL},
    {"n",                 Spot_get_n,                 NULL, "Total number of observations", NULL},
    {"gamma",             Spot_get_gamma,             NULL, "GPD gamma parameter",          NULL},
    {"sigma",             Spot_get_sigma,             NULL, "GPD sigma parameter",          NULL},
    {NULL}
};
// clang-format on

static const PyMethodDef Spot_methods[] = {
    {"fit", (PyCFunction)Spot_fit, METH_O, Spot_fit_doc},
    {"step", (PyCFunction)Spot_step, METH_O, Spot_step_doc},
    {"quantile", (PyCFunction)Spot_quantile, METH_O, Spot_quantile_doc},
    {"probability", (PyCFunction)Spot_probability, METH_O,
     Spot_probability_doc},
    {"raw", (PyCFunction)Spot_raw, METH_NOARGS, Spot_raw_doc},
    {"excesses", (PyCFunction)Spot_excesses, METH_NOARGS, Spot_excess_doc},
    {"as_dict", (PyCFunction)Spot_as_dict, METH_NOARGS, Spot_as_dict_doc},
    {NULL} /* Sentinel */
};

// new object definition using Limited API
// see
// https://github.com/joerick/python-abi3-package-sample/tree/main
// https://doc.qt.io/qtforpython-6/developer/limited_api.html#future-versions-of-the-limited-api

static PyType_Slot SpotType_slots[] = {
    {Py_tp_getset, (void *)Spot_getset}, {Py_tp_methods, (void *)Spot_methods},
    {Py_tp_init, (void *)Spot_init},     {Py_tp_dealloc, (void *)Spot_dealloc},
    {Py_tp_doc, (void *)Spot_init_doc},  {0, NULL},
};

static PyType_Spec SpotType_spec = {
    .name = "libspot.Spot",
    .basicsize = sizeof(Spot),
    .itemsize = 0,
    .flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .slots = SpotType_slots,
};

static PyModuleDef libspotmodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "libspot",
    .m_doc = "Born to flag outliers, from python :)",
    .m_size = -1,
};

PyMODINIT_FUNC PyInit_libspot(void) {
    // creates a new reference
    PyObject *SpotType = PyType_FromSpec(&SpotType_spec);
    if (SpotType == NULL) {
        return NULL;
    }

    // also creates a new reference
    PyObject *m = PyModule_Create(&libspotmodule);
    if (m == NULL) {
        Py_DECREF(SpotType);
        return NULL;
    }

    // inject __version__
    char buffer[64];
    libspot_version(buffer, 64);
    if (PyModule_AddStringConstant(m, "__version__", buffer) < 0) {
        Py_DECREF(SpotType);
        Py_DECREF(m);
        return NULL;
    }

    // add global constants
    if (PyModule_AddIntConstant(m, "NORMAL", NORMAL) < 0) {
        Py_DECREF(SpotType);
        Py_DECREF(m);
        return NULL;
    }
    if (PyModule_AddIntConstant(m, "EXCESS", EXCESS) < 0) {
        Py_DECREF(SpotType);
        Py_DECREF(m);
        return NULL;
    }
    if (PyModule_AddIntConstant(m, "ANOMALY", ANOMALY) < 0) {
        Py_DECREF(SpotType);
        Py_DECREF(m);
        return NULL;
    }

    // add Spot object
    if (PyModule_AddObject(m, "Spot", SpotType) < 0) {
        Py_DECREF(SpotType);
        Py_DECREF(m);
        return NULL;
    }

    // set builtin math functions
    set_math_functions(log, exp, pow);
    return m;
}
