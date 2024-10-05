#pragma once

#include <QObject>

// ----------------------------------------------------------
// MyTetra - программа для накопления осмысленной информации
// Волгодонск, Санкт-Петербург 2010 - 2020 г.
// Контакты: xintrea@gmail.com, www.webhamster.ru
// Данный исходный код распространяется под лицензиями:
// - GPL v.3
// - Modified BSD
// © Степанов С. М. и товарищи 2010 - 2020
// ----------------------------------------------------------

// Версия программы
#ifndef APPLICATION_VERSION
#define APPLICATION_VERSION "unknown"
#endif

#ifdef Q_CC_MSVC
[[noreturn]] __forceinline void __builtin_unreachable() { __assume(false); }
#endif
