// generated from rosidl_generator_py/resource/_idl_support.c.em
// with input from controller:msg/JointParams.idl
// generated code does not contain a copyright notice
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <Python.h>
#include <stdbool.h>
#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-function"
#endif
#include "numpy/ndarrayobject.h"
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif
#include "rosidl_runtime_c/visibility_control.h"
#include "controller/msg/detail/joint_params__struct.h"
#include "controller/msg/detail/joint_params__functions.h"


ROSIDL_GENERATOR_C_EXPORT
bool controller__msg__joint_params__convert_from_py(PyObject * _pymsg, void * _ros_message)
{
  // check that the passed message is of the expected Python class
  {
    char full_classname_dest[41];
    {
      char * class_name = NULL;
      char * module_name = NULL;
      {
        PyObject * class_attr = PyObject_GetAttrString(_pymsg, "__class__");
        if (class_attr) {
          PyObject * name_attr = PyObject_GetAttrString(class_attr, "__name__");
          if (name_attr) {
            class_name = (char *)PyUnicode_1BYTE_DATA(name_attr);
            Py_DECREF(name_attr);
          }
          PyObject * module_attr = PyObject_GetAttrString(class_attr, "__module__");
          if (module_attr) {
            module_name = (char *)PyUnicode_1BYTE_DATA(module_attr);
            Py_DECREF(module_attr);
          }
          Py_DECREF(class_attr);
        }
      }
      if (!class_name || !module_name) {
        return false;
      }
      snprintf(full_classname_dest, sizeof(full_classname_dest), "%s.%s", module_name, class_name);
    }
    assert(strncmp("controller.msg._joint_params.JointParams", full_classname_dest, 40) == 0);
  }
  controller__msg__JointParams * ros_message = _ros_message;
  {  // joint_index
    PyObject * field = PyObject_GetAttrString(_pymsg, "joint_index");
    if (!field) {
      return false;
    }
    assert(PyLong_Check(field));
    ros_message->joint_index = PyLong_AsUnsignedLong(field);
    Py_DECREF(field);
  }
  {  // max_pos
    PyObject * field = PyObject_GetAttrString(_pymsg, "max_pos");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->max_pos = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // min_pos
    PyObject * field = PyObject_GetAttrString(_pymsg, "min_pos");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->min_pos = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // max_vel
    PyObject * field = PyObject_GetAttrString(_pymsg, "max_vel");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->max_vel = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // max_acc
    PyObject * field = PyObject_GetAttrString(_pymsg, "max_acc");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->max_acc = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }

  return true;
}

ROSIDL_GENERATOR_C_EXPORT
PyObject * controller__msg__joint_params__convert_to_py(void * raw_ros_message)
{
  /* NOTE(esteve): Call constructor of JointParams */
  PyObject * _pymessage = NULL;
  {
    PyObject * pymessage_module = PyImport_ImportModule("controller.msg._joint_params");
    assert(pymessage_module);
    PyObject * pymessage_class = PyObject_GetAttrString(pymessage_module, "JointParams");
    assert(pymessage_class);
    Py_DECREF(pymessage_module);
    _pymessage = PyObject_CallObject(pymessage_class, NULL);
    Py_DECREF(pymessage_class);
    if (!_pymessage) {
      return NULL;
    }
  }
  controller__msg__JointParams * ros_message = (controller__msg__JointParams *)raw_ros_message;
  {  // joint_index
    PyObject * field = NULL;
    field = PyLong_FromUnsignedLong(ros_message->joint_index);
    {
      int rc = PyObject_SetAttrString(_pymessage, "joint_index", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // max_pos
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->max_pos);
    {
      int rc = PyObject_SetAttrString(_pymessage, "max_pos", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // min_pos
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->min_pos);
    {
      int rc = PyObject_SetAttrString(_pymessage, "min_pos", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // max_vel
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->max_vel);
    {
      int rc = PyObject_SetAttrString(_pymessage, "max_vel", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // max_acc
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->max_acc);
    {
      int rc = PyObject_SetAttrString(_pymessage, "max_acc", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }

  // ownership of _pymessage is transferred to the caller
  return _pymessage;
}
