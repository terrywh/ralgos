#pragma once

#include <chrono>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <atomic>
#include <thread>
#include <filesystem>

#include <boost/asio.hpp>
#define BOOST_BEAST_USE_STD_STRING_VIEW
#include <boost/beast.hpp>
#include <boost/context/fiber.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>

#include <hiredis/hiredis.h>

#include <cppjieba/Jieba.hpp>
