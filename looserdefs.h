#ifndef LOOSERDEFS_H
#define LOOSERDEFS_H

#include <dbm/xml.hpp>
#include <stdexcept>

#define looser_throw(msg) throw std::runtime_error(msg)
#define looser_assert(cond, msg) \
    if (!(cond)) looser_throw(msg)

#endif // LOOSERDEFS_H
