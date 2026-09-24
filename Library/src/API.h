#pragma once

#ifdef IN_LIBRARY
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif