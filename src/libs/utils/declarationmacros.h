#pragma once

#define UTILS_DELETE_MOVE_AND_COPY(Class) \
    Class(const Class&) = delete; \
    Class &operator=(const Class&) = delete; \
    Class(Class&&) = delete; \
    Class &operator=(Class&&) = delete;
