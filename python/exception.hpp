#pragma once
#include <boost/python/object.hpp>
#include <string>
#include <vector>

class python_exception
{
public:
	class frame;
	typedef std::vector<frame> frame_container;
	python_exception()
	{
	}
	void fetch();
	const std::string& exception_class_name() const { return m_exception_class_name; }
	const std::string& exception_description() const { return m_exception_description; }
	const frame_container& tbframes() const { return m_tbframes; }
private:
	typedef boost::python::object object;
	std::string m_exception_class_name;
	std::string m_exception_description;
	frame_container m_tbframes;
	void set_cls_name(const object& exception, const object& None);
	void set_description(const object& value);
	void set_traceback(object traceback, const object& None);
};

class python_exception::frame
{
	unsigned m_lineno;
	std::string m_filename;
	std::string m_function;
public:
	frame(unsigned l, std::string&& fn, std::string&& f) :
		m_lineno(l), m_filename(fn), m_function(f)
	{
	}
	unsigned lineno() const { return m_lineno; }
	const std::string& filename() const { return m_filename; }
	const std::string& function() const { return m_function; }
};

void load_traceback_class();
