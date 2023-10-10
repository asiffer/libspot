// TODO: maybe writing a cpython extension could be nice but I don't know how
// to integrate the Spot struct currently. Maybe the ctypes interface is enough
// for the moment...

#define PY_SSIZE_T_CLEAN
#include "../dist/spot.h"
#include <Python.h>
#include <structmember.h>
#define STR(x) STR_(x)
#define STR_(x) #x

#define SPOT_INIT_LOW 0
#define SPOT_INIT_DISCARD_ANOMALIES 1
#define SPOT_INIT_LEVEL 0.98
#define SPOT_INIT_MAX_EXCESS 500

// define a wrapper around the raw Spot structure
typedef struct {
    PyObject_HEAD struct Spot _spot;
} Spot;

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
        PyList_SET_ITEM(list, i, num);
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

    return Py_BuildValue("{sksksdsisN}", "cursor", ubend->cursor, "capacity",
                         ubend->capacity, "last_erased_data",
                         ubend->last_erased_data, "filled", ubend->filled,
                         "data", data);
}

static PyObject *Peaks_as_dict(struct Peaks *peaks) {
    PyObject *container = Ubend_as_dict(&(peaks->container));
    return Py_BuildValue("{sdsdsdsdsN}", "e", peaks->e, "e2", peaks->e2, "min",
                         peaks->min, "max", peaks->max, "container",
                         container);
}

static PyObject *Tail_as_dict(struct Tail *tail) {
    PyObject *peaks = Peaks_as_dict(&(tail->peaks));
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
                                                                     "Compute "
                                                                     "the "
                                                                     "first "
                                                                     "excess "
                                                                     "and "
                                                                     "anomaly "
                                                                     "threshol"
                                                                     "ds "
                                                                     "based "
                                                                     "on "
                                                                     "training"
                                                                     " data");

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

    // libspot API call
    int result = spot_init(&(self->_spot), q, low, discard_anomalies, level,
                           max_excess);
    if (result < 0) {
        const unsigned long size = 256;
        char buffer[size];
        error_msg(-result, buffer, size);
        PyErr_SetString(PyExc_RuntimeError, buffer);
    }
    return result;
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

    unsigned long size = (unsigned long)PySequence_Fast_GET_SIZE(seq);

    // allocate a new raw buffer to pass to the fit method
    double *x = malloc(size * sizeof(double));
    for (unsigned long i = 0; i < size; i++) {
        // https://docs.python.org/3/c-api/float.html#c.PyFloat_AsDouble
        x[i] = PyFloat_AsDouble(PySequence_Fast_GET_ITEM(seq, i));
    }
    // libspot API call
    int result = spot_fit(&(self->_spot), x, size);
    // free the buffer
    free(x);
    // check result

    if (result < 0) {
        const unsigned long size = 256;
        char buffer[size];
        error_msg(-result, buffer, size);
        PyErr_SetString(PyExc_RuntimeError, buffer);
    }
    Py_RETURN_NONE;
}

PyDoc_STRVAR(Spot_step_doc, "step($self, x)\n--\n\n"
                            "Fit-predict step");

static PyObject *Spot_step(Spot *self, PyObject *x) {
    double z = PyFloat_AsDouble(x);

    // libspot API call
    int result = spot_step(&(self->_spot), z);

    return PyLong_FromLong(result);
}

PyDoc_STRVAR(Spot_quantile_doc, "quantile($self, q)\n--\n\n"
                                "Compute the value zq such that P(X>zq) = q");

static PyObject *Spot_quantile(Spot *self, PyObject *x) {
    double q = PyFloat_AsDouble(x);

    // libspot API call
    double z = spot_quantile(&(self->_spot), q);

    return PyFloat_FromDouble(z);
}

PyDoc_STRVAR(Spot_probability_doc,
             "probability($self, z)\n--\n\n"
             "Compute the probability p such that P(X>z) = p");

static PyObject *Spot_probability(Spot *self, PyObject *x) {
    double z = PyFloat_AsDouble(x);

    // libspot API call
    double q = spot_probability(&(self->_spot), z);

    return PyFloat_FromDouble(q);
}

PyDoc_STRVAR(Spot_raw_doc, "raw($self)\n--\n\n"
                           "Return the internal C structure as a bytearray");

static PyObject *Spot_raw(Spot *self) {
    const char *buffer = (char *)(&self->_spot);
    PyObject *bytearray =
        PyByteArray_FromStringAndSize(buffer, sizeof(struct Spot));
    return bytearray;
}

// static PyObject *Spot_fromraw(void *null, char *buffer, unsigned long size)
// {
//     // TODO: the problem with (de)serialization is that we need to store
//     // the excesses.
//     Spot spot;
//     PyObject *q_fake = PyFloat_FromDouble(1e-8);
//     /* Call the class object. */
//     PyObject *obj = PyObject_CallObject((PyObject *)&spot, q_fake);
//     Spot *s = (Spot *)obj;
//     Py_DECREF(q_fake);
//     memcpy(&(s->_spot), buffer, size);
//     return obj;
// }

PyDoc_STRVAR(Spot_excess_doc, "excess($self)\n--\n\n"
                              "Return the stored excesses");

static PyObject *Spot_excesses(Spot *self) {
    return excesses(&self->_spot.tail.peaks.container);
}

PyDoc_STRVAR(
    Spot_as_dict_doc,
    "as_dict($self)\n--\n\n"
    "[EXPRIMENTAL] Return the internal C struct as a python dictionnary");

static PyObject *Spot_as_dict(Spot *self) {

    struct Spot *spot = &(self->_spot);
    PyObject *tail = Tail_as_dict(&(spot->tail));
    return Py_BuildValue(
        "{sdsdsisisdsdsksksN}", "q", spot->q, "level", spot->level,
        "discard_anomalies", spot->discard_anomalies, "low", spot->low,
        "anomaly_threshold", spot->anomaly_threshold, "excess_threshold",
        spot->excess_threshold, "Nt", spot->Nt, "n", spot->n, "tail", tail);
}

static void Spot_dealloc(Spot *self) {
    // free internal structure
    // libspot API call
    spot_free(&(self->_spot));
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyMemberDef Spot_members[] = {
    {"q", T_DOUBLE, offsetof(Spot, _spot.q), READONLY, "Anomaly probability"},
    {"level", T_DOUBLE, offsetof(Spot, _spot.level), READONLY,
     "Location of the tail"},
    {"anomaly_threshold", T_DOUBLE, offsetof(Spot, _spot.anomaly_threshold),
     READONLY, "Normal/abnormal threshold"},
    {"excess_threshold", T_DOUBLE, offsetof(Spot, _spot.excess_threshold),
     READONLY, "Tail threshold"},
    {"Nt", T_ULONG, offsetof(Spot, _spot.Nt), READONLY,
     "Normal/abnormal threshold"},
    {"n", T_ULONG, offsetof(Spot, _spot.n), READONLY, "Tail threshold"},
    {"gamma", T_DOUBLE, offsetof(Spot, _spot.tail.gamma), READONLY,
     "GPD gamma parameter"},
    {"sigma", T_DOUBLE, offsetof(Spot, _spot.tail.sigma), READONLY,
     "GPD sigma parameter"},
    {NULL} /* Sentinel */
};

static PyMethodDef Spot_methods[] = {
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

static PyTypeObject SpotType = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "libspot.Spot",
    .tp_doc = Spot_init_doc,
    .tp_basicsize = sizeof(Spot),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc)Spot_init,
    .tp_dealloc = (destructor)Spot_dealloc,
    .tp_methods = Spot_methods,
    .tp_members = Spot_members,
};

static PyModuleDef libspotmodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "libspot",
    .m_doc = "Born to flag outliers, from python :)",
    .m_size = -1,
};

PyMODINIT_FUNC PyInit_libspot(void) {
    PyObject *m;
    if (PyType_Ready(&SpotType) < 0) {
        return NULL;
    }

    m = PyModule_Create(&libspotmodule);
    if (m == NULL) {
        return NULL;
    }

    Py_INCREF(&SpotType);

    // inject __version__
    const unsigned long size = 64;
    char buffer[size];
    libspot_version(buffer, size);
    if (PyModule_AddStringConstant(m, "__version__", buffer) < 0) {
        return NULL;
    }

    // add global constants
    if (PyModule_AddIntConstant(m, "NORMAL", NORMAL) < 0) {
        return NULL;
    }
    if (PyModule_AddIntConstant(m, "EXCESS", EXCESS) < 0) {
        return NULL;
    }
    if (PyModule_AddIntConstant(m, "ANOMALY", ANOMALY) < 0) {
        return NULL;
    }

    // add Spot object
    PyModule_AddObject(m, "Spot", (PyObject *)&SpotType);

    // set default allocators
    // libspot API call
    set_allocators(malloc, free);
    return m;
}
