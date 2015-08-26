/*
   Copyright (C) 2003/2004 Alexandre Courbot <alexandrecourbot@linuxgames.com>
   Part of the Adonthell Project http://adonthell.linuxgames.com

   Adonthell is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Adonthell is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Adonthell; if not, write to the Free Software 
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


/**
 * @file   python.h 
 * @author Alexandre Courbot <alexandrecourbot@linuxgames.com>
 * 
 * @brief  The python module main include file.
 * 
 * 
 */

#ifndef PYTHON_PYTHON_H_
#define PYTHON_PYTHON_H_

#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif
#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>
#include <adonthell/base/flat.h>

extern "C" {
	PyObject *cxx_to_py (void *instance, const char *name, const bool & ownership);
	void py_to_cxx (PyObject *instance, const char *name, void **retval);
}

/**
 * Provides a bridge between C++ and Python,
 * as well as some convenience classes to ease
 * the use of the Adonthell library from Python.
 * 
 */
namespace python
{
    /**
     * @name Initialization and cleanup.
     * 
     */
    //@{
    /**
     * Initialize the embedded Python interpreter.
     */
    void init();

    /**
     * Shutdown the embedded Python interpreter.
     */
    void cleanup();
    //@}

    /**
     * @name Debugging.
	 *
     */
    //@{
    /**
     * Dump any error information to stderr.
     *
     * This function should be called after every operation
     * involving Python from C++, so the user can get informed if
     * something went wrong. By default, all the methods of this
     * module call it when appropriate.
     *
     */
    void show_traceback();
	//@}

    /**
     * @name High-level functions.
     * 
     */
    //@{
    /**
     * Add given path to Python's list of module directories. 
     * @param path the path to add.
     * @return \b true on success, \b false otherwise.
     */
    bool add_search_path(const std::string & path);

    /** 
     * Execute a python statement and return resulting object.
     * This is a new reference that needs to be Py_DECREF'd when done.
     * @param statement Python code to execute.
     * @param start one of Py_eval_input, Py_file_input or Py_single_input
	 * @param globals the global dictionary for the Python interpreter.
     * @return \b result on success, \b NULL otherwise.
     */
    PyObject* run_string (const std::string & statement, const int & start = Py_eval_input, PyObject *globals = NULL);
    
    /** 
     * Execute a python statement and return success.
     * @param statement Python code to execute.
     * @return \b true on success, \b false otherwise.
     */
    bool run_simple_string (const std::string & statement);

    /**
     * Import %python module with given name.
     * @param name name of module to import
     * @return imported module or \c NULL on error.
     */
    PyObject * import_module(const std::string & name);

    //@}

    /**
     * @name C++ to Python instance passing functions.
     *
     */
    //@{

    /**
     * Ownership of a python object made with pass_instance. Value
     * python::c_owns means that once the Python object is deleted, the
     * C++ instance will remain valid. On the contrary, python::python_owns
     * will cause the C++ instance to be destroyed when the Python object is
     * deleted.
     * 
     */
    typedef enum { c_owns = 0, python_owns = 1 } ownership;

    /** 
     * Default version of pass_instance - it will fetch the name of the class
     * that is passed using a specialized version of get_type_name to create
     * a Python object from a pointer.
     * 
     * @param arg a pointer to the object to pass to Python.
     * @param own ownership of the C++ object.
     * 
     * @return a Python object representing \e arg.
     */
    template <class A> inline
    PyObject * pass_instance(A arg, const ownership own = c_owns)
    { 
        return cxx_to_py ((void *) arg, arg->get_type_name(), own);
    }
    
    /** 
     * Specialized version of pass_instance which makes a Python integer
     * from a C++ one.
     * 
     * @param arg the integer to be passed to Python.
     * @param own useless here.
     * 
     * @return a Python object representing \e arg.
     */
    template <> inline
    PyObject * pass_instance<const s_int32>(const s_int32 arg, const ownership own)
    { 
        return PyInt_FromLong(arg);
        show_traceback();
    }

    /** 
     * Specialized version of pass_instance which makes a Python boolean
     * from a C++ one.
     * 
     * @param arg the boolean to be passed to Python.
     * @param own useless here.
     * 
     * @return a Python object representing \e arg.
     */
    template <> inline
    PyObject * pass_instance<bool>(bool arg, const ownership own)
    { 
        return PyInt_FromLong((s_int32)arg);
        show_traceback();
    }
    
    /** 
     * Specialized version of pass_instance which makes a Python string
     * from a C++ std::string.
     * 
     * @param arg the std::string to be passed to Python.
     * @param own useless here.
     * 
     * @return a Python object representing \e arg.
     */
    template <> inline
    PyObject * pass_instance<const std::string &>(const std::string & arg, const ownership own) 
    { 
        return PyString_FromString(arg.c_str());
        show_traceback();
    }

    /** 
     * Specialized version of pass_instance which makes a Python string
     * from a C++ char *.
     * 
     * @param arg the char * to be passed to Python.
     * @param own useless here.
     * 
     * @return a Python object representing \e arg.
     */
    template <> inline
    PyObject * pass_instance<const char *>(const char * arg, const ownership own) 
    { 
        return PyString_FromString((char *) arg);
        show_traceback();
    }

    /** 
     * Sets the ownership of an object created with pass_instance.
     * 
     * @param obj the object to change ownership of.
     * @param o the new ownership.
     */
    inline void set_ownership(PyObject * obj, const ownership o)
    {
        PyObject * n = PyInt_FromLong(o);
        PyObject_SetAttrString (obj, (char*)"thisown", n);
        Py_DECREF(n);
        show_traceback();
    }
    
    //@}

    /**
     * @name Python to C++ instance passing functions.
     *
     */
    //@{

    /** 
     * Retrieves the C++ intance of a Python object created with pass_instance.
     * 
     * This is the default version of retrieve_instance. The template element A must
     * be a pointer type.
     *
     * @param pyinstance The Python instance of the object to retrieve.
     * 
     * @return A borrowed pointer to the C++ instance of the object.
     */
    template <class A, class B> inline
    A retrieve_instance(PyObject * pyinstance)
    {
        B *retvalue = NULL;
		py_to_cxx (pyinstance, B::get_type_name_s(), (void **) &retvalue);
		
		return retvalue;
    }

    /** 
     * Retrieves the C++ value of a Python integer.
     * 
     * @param pyinstance The Python integer to retrieve.
     * 
     * @return C++ value of pyinstance.
     */
    template <> inline
    s_int32 retrieve_instance<s_int32, s_int32>(PyObject * pyinstance)
    { 
        s_int32 retval =  PyInt_AsLong(pyinstance);
        show_traceback();
        return retval;
    }

    /** 
     * Retrieves the C++ value of a Python boolean.
     * 
     * @param pyinstance The Python boolean to retrieve.
     * 
     * @return C++ value of pyinstance.
     */
    template <> inline
    bool retrieve_instance<bool, bool>(PyObject * pyinstance)
    { 
        bool retval = (bool)PyInt_AsLong(pyinstance);
        show_traceback();
        return retval;
    }

    /** 
     * Retrieves the C++ value of a Python string as a char *.
     * 
     * @param pyinstance The Python string to retrieve.
     * 
     * @return char * string value of pyinstance.
     */
    template <> inline
    const char * retrieve_instance<const char *, const char *>(PyObject * pyinstance)
    {
        const char * retval = PyString_AsString(pyinstance);
        show_traceback();
        return retval;
    }

    /** 
     * Retrieves the C++ value of a Python string as a std::string.
     * 
     * @param pyinstance The Python string to retrieve.
     * 
     * @return std::string value of pyinstance.
     */
    template <> inline
    std::string retrieve_instance<std::string, std::string>(PyObject * pyinstance)
    { 
        const std::string &retval = std::string(PyString_AsString(pyinstance));
        show_traceback();
        return retval;
    }
    //@}

    /**
     * @name Convenience functions.
     */
    //@{
    /**
     * Pads the front of the given tuple, moving the existing
     * entries to the end of the tuple. If the given tuple is
     * NULL, returns a new tuple of size len. Reference count
     * of the contents of the given tuple is increased by one.
     *
     * @return a new tuple or NULL on error.
     */
    PyObject *pad_tuple (PyObject *tuple, const u_int16 & len);
    
    /**
     * Read the contents of a tuple from given stream.
     * @param in flattener to read the tuple from.
     * @param start index from where to start filling the tuple.
     * @return a new tuple filled with the data read from stream.
     */
    PyObject *get_tuple (base::flat & in, const u_int16 & start = 0);

    /**
     * Write the contents of a tuple to given stream. Only supports integer and
     * string objects.
     * @param tuple Python tuple whose contents to save.
     * @param out flattener to store the tuple in.
     * @param start index from where to start flattening the tuple.
     */
    void put_tuple (PyObject *tuple, base::flat & out, const u_int16 & start = 0);
    //@}
}

#include "callback.h"

#endif
