
#include "Console.h"
#include "Policies/SingletonImp.h"

INSTANTIATE_SINGLETON_1(Console);

Console::Console()
{
    SetTitle("OregonRealm");
}

Console::~Console()
{
}

void Console::FatalError(const char*)
{
}

void Console::Initialize()
{
}

void Console::SetLoading(bool, const char*)
{
}

void Console::SetLoadingLabel(const char*, bool)
{
}
