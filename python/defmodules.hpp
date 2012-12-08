#pragma once
#include <boost/python/object.hpp>
#include <boost/python/scope.hpp>

void define_input_class();
void define_object_base_class(boost::python::scope& s);
void define_player_module(boost::python::object& __main__, boost::python::scope& scope_dxx);
void define_powerup_module(boost::python::object& __main__, boost::python::scope& scope_dxx);
void define_reactor_module(boost::python::object& __main__, boost::python::scope& scope_dxx);
void define_robot_module(boost::python::object& __main__, boost::python::scope& scope_dxx);
void define_segment_module(boost::python::object& __main__, boost::python::scope& scope_dxx);
void define_weapon_module(boost::python::object& __main__, boost::python::scope& scope_dxx);
