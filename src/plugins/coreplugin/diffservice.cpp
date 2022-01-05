#include "diffservice.h"

namespace Core {

static DiffService *g_instance = nullptr;

DiffService::DiffService()
{
    g_instance = this;
}

DiffService::~DiffService()
{
    g_instance = nullptr;
}

DiffService *DiffService::instance()
{
    return g_instance;
}

} // Core
