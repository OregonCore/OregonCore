
#include "Console.h"

INSTANTIATE_SINGLETON_1(Console);

Console::Console() : m_IamEnabled(false)
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

void Console::Restore()
{
}

void Console::SetLoading(bool, const char*)
{
}

void Console::SetLoadingLabel(const char*, bool)
{
}
