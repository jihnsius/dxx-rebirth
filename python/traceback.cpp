#include <boost/python/extract.hpp>
#include <boost/python/import.hpp>
#include <boost/python/list.hpp>
#include <boost/python/object.hpp>
#include "exception.hpp"

using namespace boost::python;

struct python_exception_objects
{
	typedef boost::python::handle<> handle;
	handle m_exception;
	handle m_value;
	handle m_traceback;
	python_exception_objects(PyObject *exception, PyObject *value, PyObject *traceback) :
		m_exception(exception),
		m_value(boost::python::allow_null(value)),
		m_traceback(boost::python::allow_null(traceback))
	{
	}
	static python_exception_objects fetch();
};

void load_traceback_class()
{
#if 0
	try {
		object py_traceback{import("traceback")};
		if (py_traceback.is_none())
			return;
		g_py_traceback_fe = getattr(py_traceback, "format_exception", object());
		g_py_traceback_feo = getattr(py_traceback, "format_exception_only", object());
	} catch (const error_already_set &) {
		PyErr_Print();
	}
#endif
}

python_exception_objects python_exception_objects::fetch()
{
	PyObject *exception = NULL;
	PyObject *value = NULL;
	PyObject *traceback = NULL;
	PyErr_Fetch(&exception, &value, &traceback);
	return python_exception_objects(exception, value, traceback);
}

static void set_string(std::string& o, const object& n)
{
	if (n.is_none())
	{
		o.clear();
		return;
	}
	const extract<std::string> es{n};
	if (!es.check())
	{
		o.clear();
		return;
	}
	o = es();
}

void python_exception::set_cls_name(const object& exception, const object& None)
{
	if (exception.is_none())
	{
		m_exception_class_name.clear();
		return;
	}
	const object n{getattr(exception, "__name__", None)};
	set_string(m_exception_class_name, n);
}

void python_exception::set_description(const object& value)
{
	set_string(m_exception_description, value);
}

void python_exception::set_traceback(object traceback, const object& None)
{
	for (; ! traceback.is_none(); traceback = getattr(traceback, "tb_next", None))
	{
		const object frame{getattr(traceback, "tb_frame", None)};
		if (frame.is_none())
			return;
		const object code{getattr(frame, "f_code", None)};
		if (code.is_none())
			return;
		const unsigned lineno{({
			const object olineno{getattr(traceback, "tb_lineno", None)};
			olineno.is_none()
				? 0
				: ({ extract<unsigned> es{olineno};
					es.check() ? es() : 0;
			});
		})};
		const auto as = [&code, &None](const char *attr) -> std::string {
			const object o{getattr(code, attr, None)};
			if (o.is_none())
				return "";
			const extract<std::string> es(o);
			if (!es.check())
				return "";
			return es();
		};
		m_tbframes.emplace_back(lineno, as("co_filename"), as("co_name"));
	}
}

void python_exception::fetch()
{
	const python_exception_objects peo{python_exception_objects::fetch()};
	const object None;
	set_cls_name(object(peo.m_exception), None);
	set_description(object(peo.m_value));
	if (!peo.m_traceback)
		return;
	set_traceback(object(peo.m_traceback), None);
}
